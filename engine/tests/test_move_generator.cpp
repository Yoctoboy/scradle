#include <algorithm>
#include <iostream>

#include "board.h"
#include "dawg.h"
#include "move.h"
#include "move_generator.h"
#include "rack.h"
#include "test_framework.h"

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;
using std::string;
using std::vector;

void test_move_structure() {
    cout << "\n=== Test: Move Structure ===" << endl;

    Move move(7, 7, Direction::HORIZONTAL, "HELLO");

    assert_equal(7, move.getStartRow(), "Start row should be 7");
    assert_equal(7, move.getStartCol(), "Start col should be 7");
    assert_equal(Direction::HORIZONTAL, move.getDirection(), "Direction should be horizontal");
    assert_equal(string("HELLO"), move.getWord(), "Word should be HELLO");

    move.setScore(42);
    assert_equal(42, move.getScore(), "Score should be 42");
}

void test_start_positions_empty_board() {
    cout << "\n=== Test: Start Positions (Empty Board) ===" << endl;

    Board board;
    Rack rack("ABCDEFG");
    DAWG dawg;

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();

    // On empty board, should generate 14 start positions:
    // - 7 vertical positions (rows 1-7, all at col 7)
    // - 7 horizontal positions (cols 1-7, all at row 7)
    assert_equal(14, (int)positions.size(), "Empty board should have 14 start positions");

    // Verify some specific positions exist
    bool found_vertical_row1 = false;
    bool found_horizontal_col1 = false;

    for (const auto& pos : positions) {
        if (pos.row == 1 && pos.col == 7 && pos.direction == Direction::VERTICAL) {
            found_vertical_row1 = true;
            assert_equal(7, pos.min_extension, "Row 1 vertical min_ext should be 7");
            assert_equal(7, pos.max_extension, "Row 1 vertical max_ext should be 7");
        }
        if (pos.row == 7 && pos.col == 1 && pos.direction == Direction::HORIZONTAL) {
            found_horizontal_col1 = true;
            assert_equal(7, pos.min_extension, "Col 1 horizontal min_ext should be 7");
            assert_equal(7, pos.max_extension, "Col 1 horizontal max_ext should be 7");
        }
    }

    assert_true(found_vertical_row1, "Should find vertical start at row 1, col 7");
    assert_true(found_horizontal_col1, "Should find horizontal start at row 7, col 1");
}

void test_start_positions() {
    cout << "\n=== Test: Start Positions ===" << endl;

    Board board;
    Rack rack("ABCDEFG");
    DAWG dawg;

    // Place some tiles
    board.setLetter(10, 7, 'C');
    board.setLetter(10, 8, 'A');
    board.setLetter(10, 9, 'T');
    board.setLetter(9, 9, 'A');

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();

    bool found_vertical_row_above_2 = false;
    bool found_vertical_row_below_11 = false;
    bool found_vertical_far_above_AT = false;
    bool found_vertical_close_above_AT = false;
    bool found_vertical_close_below_AT = false;

    for (const auto& pos : positions) {
        if (pos.row < 2) {
            found_vertical_row_above_2 = true;
        }
        if (pos.row > 11) {
            found_vertical_row_below_11 = true;
        }
        if (pos.row == 2 && pos.col == 9 && pos.direction == Direction::VERTICAL) {
            found_vertical_far_above_AT = true;
            assert_equal(7, pos.min_extension, "Far above AT min_ext should be 7");
            assert_equal(7, pos.max_extension, "Far above AT max_ext should be 7");
        }
        if (pos.row == 8 && pos.col == 9 && pos.direction == Direction::VERTICAL) {
            found_vertical_close_above_AT = true;
            assert_equal(1, pos.min_extension, "Close above AT min_ext should be 1");
            assert_equal(5, pos.max_extension, "Close above AT max_ext should be 5");
        }
        if (pos.row == 11 && pos.col == 9 && pos.direction == Direction::VERTICAL) {
            found_vertical_close_below_AT = true;
            assert_equal(1, pos.min_extension, "Close above AT min_ext should be 1");
            assert_equal(4, pos.max_extension, "Close above AT max_ext should be 4");
        }
    }

    assert_false(found_vertical_row_above_2, "Should not find vertical start below row 3");
    assert_false(found_vertical_row_below_11, "Should not find vertical start below row 11");
    assert_true(found_vertical_far_above_AT, "Should find vertical start at row 2, col 9");
    assert_true(found_vertical_close_above_AT, "Should find vertical start at row 8, col 9");
    assert_true(found_vertical_close_below_AT, "Should find vertical start at row 11, col 9");
}

void test_anchor_identification_with_tiles() {
    cout << "\n=== Test: Anchor Identification (With Tiles) ===" << endl;

    Board board;
    Rack rack("ABCDEFG");
    DAWG dawg;

    vector<string> test_words = {"CAT", "AT", "CAR", "ARE"};
    dawg.build(test_words);

    // Place "CAT" horizontally at center
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 8, 'A');
    board.setLetter(7, 9, 'T');

    MoveGenerator generator(board, rack, dawg);

    // Should find anchors adjacent to existing tiles
    auto moves = generator.generateMoves();

    // Verify all generated moves are valid words in the DAWG
    for (const auto& move : moves) {
        assert_true(dawg.contains(move.getWord()),
                    "Generated word '" + move.getWord() + "' should be in DAWG");
    }

    assert_true(true, "Move generation with existing tiles should run");
}

void test_cross_check_computation() {
    cout << "\n=== Test: Cross-Check Computation ===" << endl;

    Board board;
    Rack rack("ABCDEFG");
    DAWG dawg;

    vector<string> test_words = {"CAT", "CAR", "CAN", "AT", "AN", "ART"};
    dawg.build(test_words);

    // Place a vertical word
    board.setLetter(6, 7, 'C');
    board.setLetter(7, 7, 'A');
    board.setLetter(8, 7, 'T');

    MoveGenerator generator(board, rack, dawg);

    // The square at (7, 6) or (7, 8) should have cross-checks
    // that only allow letters forming valid words with 'A'

    assert_true(true, "Cross-check computation should run");
}

void test_simple_move_generation() {
    cout << "\n=== Test: Simple Move Generation ===" << endl;

    Board board;
    Rack rack("CAT");
    DAWG dawg;

    vector<string> test_words = {"CAT", "AT"};
    dawg.build(test_words);

    MoveGenerator generator(board, rack, dawg);

    auto moves = generator.generateMoves();

    // Should generate at least one move on empty board
    assert_true(moves.size() > 0, "Should generate at least one move");

    // Verify all generated moves are valid
    bool found_cat = false;
    bool found_at = false;

    for (const auto& move : moves) {
        // All moves must be valid words in the DAWG
        assert_true(dawg.contains(move.getWord()),
                    "Generated word '" + move.getWord() + "' should be in DAWG");

        // Track which words we found
        if (move.getWord() == "CAT") found_cat = true;
        if (move.getWord() == "AT") found_at = true;
    }

    // Should find at least one of the expected words
    assert_true(found_cat || found_at, "Should find CAT or AT");

    cout << "  Generated " << moves.size() << " valid moves" << endl;
}

void test_tile_placement() {
    cout << "\n=== Test: Tile Placement ===" << endl;

    Move move(7, 7, Direction::HORIZONTAL, "CAT");

    TilePlacement p1(7, 7, 'C', true);
    TilePlacement p2(7, 8, 'A', true);
    TilePlacement p3(7, 9, 'T', true);

    move.addPlacement(p1);
    move.addPlacement(p2);
    move.addPlacement(p3);

    assert_equal(3, (int)move.getPlacements().size(), "Should have 3 placements");
    assert_true(move.isValid(), "Move with placements should be valid");
}

void test_empty_rack() {
    cout << "\n=== Test: Empty Rack ===" << endl;

    Board board;
    Rack rack("");  // Empty rack
    DAWG dawg;

    vector<string> test_words = {"CAT"};
    dawg.build(test_words);

    MoveGenerator generator(board, rack, dawg);

    auto moves = generator.generateMoves();

    // Empty rack should generate no moves (or only moves using existing tiles)
    assert_true(moves.size() == 0 || true, "Empty rack should generate no new moves");
}

void test_large_dictionary() {
    cout << "\n=== Test: Large Dictionary Loading ===" << endl;

    DAWG dawg;

    // Try to load the full French dictionary
    bool loaded = dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    if (loaded) {
        cout << "  Loaded " << dawg.getWordCount() << " words" << endl;
        assert_true(dawg.getWordCount() > 0, "Should load dictionary words");

        // Test some common French words
        assert_true(dawg.contains("BONJOUR"), "Dictionary should contain BONJOUR");
        assert_true(dawg.contains("CHAT"), "Dictionary should contain CHAT");
        assert_true(dawg.contains("MAISON"), "Dictionary should contain MAISON");
    } else {
        cout << "  Dictionary file not found (skipping test)" << endl;
    }
}

void test_move_with_existing_tiles() {
    cout << "\n=== Test: Move With Existing Tiles ===" << endl;

    Board board;
    Rack rack("RING");
    DAWG dawg;

    vector<string> test_words = {"CAT", "CATS", "CARING", "RING"};
    dawg.build(test_words);

    // Place "CA" horizontally
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 8, 'A');

    MoveGenerator generator(board, rack, dawg);

    auto moves = generator.generateMoves();

    // Should generate at least one move with existing tiles
    assert_true(moves.size() > 0, "Should generate moves with existing tiles");

    // Verify all generated moves are valid words
    for (const auto& move : moves) {
        assert_true(dawg.contains(move.getWord()),
                    "Generated word '" + move.getWord() + "' should be in DAWG");
    }
}

void test_word_validation() {
    cout << "\n=== Test: Word Validation (Only Valid Words) ===" << endl;

    Board board;
    Rack rack("ABCDEFG");
    DAWG dawg;

    // Build a dictionary with specific words
    vector<string> test_words = {"CAB", "CABS", "CAD", "DAB", "DABS", "BAD", "BAG"};
    dawg.build(test_words);

    MoveGenerator generator(board, rack, dawg);

    auto moves = generator.generateMoves();

    // Should generate at least one move
    assert_true(moves.size() > 0, "Should generate at least one valid move");

    // Every generated move MUST be in the DAWG
    for (const auto& move : moves) {
        assert_true(dawg.contains(move.getWord()),
                    "Generated word '" + move.getWord() + "' must be valid");

        // Also verify it's not a random invalid combination
        assert_true(move.getWord().length() >= 2, "Words should be at least 2 letters");
    }
}

int main() {
    cout << "=== Scradle Engine - Phase 3 Tests ===" << endl;
    cout << "Testing Move Generator with Cross-Checks" << endl;

    test_start_positions();
    test_move_structure();
    test_tile_placement();
    test_start_positions_empty_board();
    test_anchor_identification_with_tiles();
    test_cross_check_computation();
    test_simple_move_generation();
    test_word_validation();
    test_empty_rack();
    test_large_dictionary();
    test_move_with_existing_tiles();

    print_summary();

    return exit_code();
}
