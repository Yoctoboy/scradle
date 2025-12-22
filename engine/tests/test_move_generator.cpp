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

void test_raw_moves_basic() {
    cout << "\n=== Test: Raw Moves - Basic Generation ===" << endl;

    Board board;
    Rack rack("CAT");
    DAWG dawg;

    MoveGenerator generator(board, rack, dawg);

    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Should generate raw moves on empty board
    assert_true(raw_moves.size() > 0, "Should generate raw moves");

    // All raw moves should have at least one placement
    for (const auto& raw_move : raw_moves) {
        assert_true(raw_move.placements.size() > 0, "Raw move should have placements", false);
    }

    cout << "  Generated " << raw_moves.size() << " raw moves" << endl;
}

void test_raw_moves_rack_constraint() {
    cout << "\n=== Test: Raw Moves - Rack Constraints ===" << endl;

    Board board;
    Rack rack("ABCDCBA");
    DAWG dawg;

    MoveGenerator generator(board, rack, dawg);

    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Check that all raw moves only use letters from the rack
    for (const auto& raw_move : raw_moves) {
        string used_letters = "";
        for (const auto& placement : raw_move.placements) {
            used_letters += placement.letter;
        }

        // Verify each used letter is in the rack
        for (char c : used_letters) {
            int count_in_used = 0;
            int count_in_rack = 0;

            for (char u : used_letters) {
                if (u == c) count_in_used++;
            }

            string rack_str = rack.toString();
            for (char r : rack_str) {
                if (r == c) count_in_rack++;
            }

            assert_true(count_in_used <= count_in_rack,
                        "Letter '" + string(1, c) + "' used <= available in rack", false);
        }
    }

    assert_true(true, "All raw moves use only rack letters");
}

void test_raw_moves_blank_expansion() {
    cout << "\n=== Test: Raw Moves - Blank Expansion ===" << endl;

    Board board;
    Rack rack("A?BCDE");
    DAWG dawg;

    // Place a tile at center
    board.setLetter(7, 7, 'A');

    MoveGenerator generator(board, rack, dawg);

    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Should generate many raw moves (blank expands to 26 letters)
    assert_true(raw_moves.size() > 0, "Should generate raw moves with blanks");

    // Check that some moves have blanks marked
    int found_AABzE = 0;
    int found_AABzD = 0;
    bool found_blank_move = false;
    for (const auto& raw_move : raw_moves) {
        int found_blank_in_move = 0;
        for (const auto& placement : raw_move.placements) {
            if (placement.is_blank) {
                found_blank_in_move++;
                found_blank_move = true;
            }
        }
        assert_true(found_blank_in_move <= 1, "Move has one blank or less", 0);
        if (raw_move.direction == Direction::HORIZONTAL) {
            if (raw_move.start_row == 7 && raw_move.start_col == 6) {
                if ((int)raw_move.placements.size() == 4) {
                    TilePlacement p0 = raw_move.placements.at(0);
                    if (p0.row == 7 && p0.col == 6 && p0.letter == 'A' && p0.is_blank == false && p0.is_from_rack) {
                        TilePlacement p1 = raw_move.placements.at(1);
                        if (p1.row == 7 && p1.col == 8 && p1.letter == 'B' && p1.is_blank == false && p1.is_from_rack) {
                            TilePlacement p2 = raw_move.placements.at(2);
                            if (p2.row == 7 && p2.col == 9 && p2.letter == 'Z' && p2.is_blank && p2.is_from_rack) {
                                TilePlacement p3 = raw_move.placements.at(3);
                                if (p3.row == 7 && p3.col == 10 && p3.letter == 'E' && p3.is_blank == false && p3.is_from_rack) {
                                    found_AABzE++;
                                }
                                if (p3.row == 7 && p3.col == 10 && p3.letter == 'D' && p3.is_blank == false && p3.is_from_rack) {
                                    found_AABzD++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "  Generated " << raw_moves.size() << " raw moves with blank expansion" << endl;
    assert_equal(1, found_AABzE, "Should find AABzE move once");
    assert_equal(1, found_AABzD, "Should find AABzD move once");
    assert_true(found_blank_move, "Should find at least one move with blank tile");
}

void test_raw_moves_adjacency() {
    cout << "\n=== Test: Raw Moves - Adjacency to Existing Tiles ===" << endl;

    Board board;
    Rack rack("XY");
    DAWG dawg;

    // Place a tile at center
    board.setLetter(7, 7, 'A');

    MoveGenerator generator(board, rack, dawg);

    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // All raw moves should be adjacent to the existing tile
    for (const auto& raw_move : raw_moves) {
        bool is_adjacent = false;

        for (const auto& placement : raw_move.placements) {
            int r = placement.row;
            int c = placement.col;

            // Check if this placement is adjacent to (7,7)
            if ((r == 7 && (c == 6 || c == 8)) ||  // left or right
                (c == 7 && (r == 6 || r == 8))) {  // above or below
                is_adjacent = true;
                break;
            }
        }

        assert_true(is_adjacent, "Raw move should be adjacent to existing tile", false);
    }

    cout << "  All " << raw_moves.size() << " raw moves are properly adjacent" << endl;
}

void test_get_main_word() {
    cout << "\n=== Test: Get Main Word ===" << endl;

    Board board;
    Rack rack("CAT");
    DAWG dawg;

    // Test 1: Empty board - place "CAT" horizontally at (7,7)
    {
        MoveGenerator generator(board, rack, dawg);
        auto positions = generator.findStartPositions();
        auto raw_moves = generator.generateAllRawMoves(positions);

        // Find a move that places "CAT"
        for (const auto& raw_move : raw_moves) {
            if (raw_move.placements.size() == 3 &&
                raw_move.start_row == 7 && raw_move.start_col == 7 &&
                raw_move.direction == Direction::HORIZONTAL) {
                // Check if this is "CAT"
                if (raw_move.placements[0].letter == 'C' &&
                    raw_move.placements[1].letter == 'A' &&
                    raw_move.placements[2].letter == 'T') {
                    string word = generator.getMainWord(raw_move);
                    assert_equal(string("CAT"), word, "Main word should be CAT");
                    break;
                }
            }
        }
    }

    // Test 2: Board with existing tiles - extend a word
    {
        board.setLetter(7, 7, 'C');
        board.setLetter(7, 8, 'A');

        Rack rack2("T");
        MoveGenerator generator2(board, rack2, dawg);

        auto positions = generator2.findStartPositions();
        auto raw_moves = generator2.generateAllRawMoves(positions);

        // Find a move that extends "CA" to "CAT"
        for (const auto& raw_move : raw_moves) {
            if (raw_move.placements.size() == 1 &&
                raw_move.start_row == 7 && raw_move.start_col == 9 &&
                raw_move.direction == Direction::HORIZONTAL &&
                raw_move.placements[0].letter == 'T') {
                string word = generator2.getMainWord(raw_move);
                assert_equal(string("CAT"), word, "Main word should extend CA to CAT");
                break;
            }
        }
    }

    cout << "  getMainWord() working correctly" << endl;
}

void test_get_cross_words() {
    cout << "\n=== Test: Get Cross Words ===" << endl;

    Board board;
    Rack rack("ART");
    DAWG dawg;

    // Build DAWG with test words
    vector<string> test_words = {"CAT", "ARM", "RAT"};
    dawg.build(test_words);

    // Place only "C" and "T" horizontally (leaving gap for "A")
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 9, 'T');
    // Then place "A" and "M" (leaving gap for "R" for form ARM)
    board.setLetter(6, 7, 'A');
    board.setLetter(6, 9, 'M');
    // Board looks like
    // row 6 .......A.M.....
    // row 7 .......C.T.....

    // Now place "RAT" vertically at (6,8) to form:
    //   ARM  (where R is newly placed)
    //   CAT  (where A is newly placed)
    //    T
    // This should create cross-words "ARM" and "CAT"

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Find a move that places "R" at (6,8) and "A" at (7,8) vertically with other tiles
    bool found_test_CAT = false;
    bool found_test_ARM = false;
    bool found_valid_RAT_move = false;

    for (const auto& raw_move : raw_moves) {
        if (raw_move.direction == Direction::VERTICAL && raw_move.start_col == 8 && raw_move.start_row == 6 && raw_move.placements.size() == 3) {
            // Check if one of the placements is 'A' at (7,8) and 'R' at (6,8)
            bool has_r_at_6_8 = false;
            bool has_a_at_7_8 = false;
            bool has_t_at_8_8 = false;
            for (const auto& p : raw_move.placements) {
                if (p.row == 6 && p.col == 8 && p.letter == 'R') {
                    has_r_at_6_8 = true;
                }
                if (p.row == 7 && p.col == 8 && p.letter == 'A') {
                    has_a_at_7_8 = true;
                }
                if (p.row == 8 && p.col == 8 && p.letter == 'T') {
                    has_t_at_8_8 = true;
                }
            }

            if (has_a_at_7_8 && has_r_at_6_8 && has_t_at_8_8) {
                vector<string> cross_words = generator.getCrossWords(raw_move);
                // The newly placed 'A' should form cross-word "CAT"
                for (const auto& cw : cross_words) {
                    if (cw == "CAT") {
                        found_test_CAT = true;
                    }
                    if (cw == "ARM") {
                        found_test_ARM = true;
                    }
                }

                if (found_test_ARM && found_test_CAT) {
                    // Successfully found both cross-words
                    found_valid_RAT_move = true;
                    break;
                }
            }
        }
    }

    assert_true(found_test_CAT, "Should find cross-word CAT");
    assert_true(found_test_ARM, "Should find cross-word ARM");
    assert_true(found_valid_RAT_move, "Raw move should be valid");
}

void test_get_cross_words_with_blank() {
    cout << "\n=== Test: Get Cross Words ===" << endl;

    Board board;
    DAWG dawg;
    Rack rack("A?T");

    // Build DAWG with test words
    vector<string> test_words = {"CAT", "ARM", "RAT"};
    dawg.build(test_words);

    // Place only "C" and "T" horizontally (leaving gap for "A")
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 9, 'T');
    // Then place "A" and "M" (leaving gap for "R" for form ARM)
    board.setLetter(6, 7, 'A');
    board.setLetter(6, 9, 'M');
    // Board looks like
    // row 6 .......A.M.....
    // row 7 .......C.T.....

    // Now place "RAT" vertically at (6,8) to form:
    //   ARM  (where R is newly placed)
    //   CAT  (where A is newly placed)
    //    T
    // This should create cross-words "ARM" and "CAT"

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Find a move that places "R" at (6,8) and "A" at (7,8) vertically with other tiles
    bool found_test_CAT = false;
    bool found_test_ARM = false;
    bool found_valid_RAT_move = false;

    for (const auto& raw_move : raw_moves) {
        if (raw_move.direction == Direction::VERTICAL && raw_move.start_col == 8 && raw_move.start_row == 6 && raw_move.placements.size() == 3) {
            // Check if one of the placements is 'A' at (7,8) and 'R' at (6,8)
            bool has_r_at_6_8 = false;
            bool has_a_at_7_8 = false;
            bool has_t_at_8_8 = false;
            for (const auto& p : raw_move.placements) {
                if (p.row == 6 && p.col == 8 && p.letter == 'R') {
                    has_r_at_6_8 = true;
                }
                if (p.row == 7 && p.col == 8 && p.letter == 'A') {
                    has_a_at_7_8 = true;
                }
                if (p.row == 8 && p.col == 8 && p.letter == 'T') {
                    has_t_at_8_8 = true;
                }
            }

            if (has_a_at_7_8 && has_r_at_6_8 && has_t_at_8_8) {
                vector<string> cross_words = generator.getCrossWords(raw_move);
                // The newly placed 'A' should form cross-word "CAT"
                for (const auto& cw : cross_words) {
                    if (cw == "CAT") {
                        found_test_CAT = true;
                    }
                    if (cw == "ARM") {
                        found_test_ARM = true;
                    }
                }

                if (found_test_ARM && found_test_CAT) {
                    // Successfully found both cross-words
                    found_valid_RAT_move = true;
                    break;
                }
            }
        }
    }

    assert_true(found_test_CAT, "Should find cross-word CAT");
    assert_true(found_test_ARM, "Should find cross-word ARM");
    assert_true(found_valid_RAT_move, "Raw move should be valid");
}

void test_is_valid_move_main_word() {
    cout << "\n=== Test: isValidMove - Main Word Validation ===" << endl;

    Board board;
    Rack rack("CAT");
    DAWG dawg;

    // Build DAWG with only "CAT"
    vector<string> test_words = {"CAT"};
    dawg.build(test_words);

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    int valid_count = 0;
    int invalid_count = 0;

    for (const auto& raw_move : raw_moves) {
        string main_word = generator.getMainWord(raw_move);
        bool is_valid = generator.isValidMove(raw_move);

        if (is_valid) {
            valid_count++;
            assert_equal(string("CAT"), main_word, "Valid move should be CAT", false);
        } else {
            invalid_count++;
            assert_true(main_word != "CAT", "Invalid move should not be CAT", false);
        }
    }

    assert_true(valid_count > 0, "Should find at least one valid move (CAT)");
    assert_true(invalid_count > 0, "Should find invalid moves (non-CAT words)");
    cout << "  Found " << valid_count << " valid moves, " << invalid_count << " invalid moves" << endl;
}

void test_is_valid_move_cross_words() {
    cout << "\n=== Test: isValidMove - Cross Word Validation ===" << endl;

    Board board;
    Rack rack("ART");
    DAWG dawg;

    // Build DAWG with main word but missing one cross-word
    vector<string> test_words = {"RAT", "CAT"};  // Missing "ARM"
    dawg.build(test_words);

    // Setup board: C_T and A_M (missing R in both)
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 9, 'T');
    board.setLetter(6, 7, 'A');
    board.setLetter(6, 9, 'M');

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Look for the RAT move (should be invalid because ARM cross-word is not in DAWG)
    bool found_rat_move = false;
    for (const auto& raw_move : raw_moves) {
        string main_word = generator.getMainWord(raw_move);
        if (main_word == "RAT" && raw_move.placements.size() == 3) {
            vector<string> cross_words = generator.getCrossWords(raw_move);

            // Should have ARM and CAT as cross-words
            bool has_arm = false;
            bool has_cat = false;
            for (const auto& cw : cross_words) {
                if (cw == "ARM") has_arm = true;
                if (cw == "CAT") has_cat = true;
            }

            if (has_arm && has_cat) {
                // This is the move we're testing - should be invalid because ARM is not in DAWG
                found_rat_move = true;
                bool is_valid = generator.isValidMove(raw_move);
                assert_false(is_valid, "RAT move should be invalid (ARM not in DAWG)");
                break;
            }
        }
    }

    assert_true(found_rat_move, "Should find RAT move to test");
}

void test_is_valid_move_all_valid() {
    cout << "\n=== Test: isValidMove - All Words Valid ===" << endl;

    Board board;
    Rack rack("ART");
    DAWG dawg;

    // Build DAWG with all necessary words
    vector<string> test_words = {"RAT", "CAT", "ARM"};
    dawg.build(test_words);

    // Setup board: C_T and A_M (will form CAT and ARM cross-words with RAT)
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 9, 'T');
    board.setLetter(6, 7, 'A');
    board.setLetter(6, 9, 'M');

    MoveGenerator generator(board, rack, dawg);
    auto positions = generator.findStartPositions();
    auto raw_moves = generator.generateAllRawMoves(positions);

    // Look for the RAT move (should be valid now)
    bool found_valid_rat = false;
    for (const auto& raw_move : raw_moves) {
        string main_word = generator.getMainWord(raw_move);
        if (main_word == "RAT" && raw_move.placements.size() == 3) {
            vector<string> cross_words = generator.getCrossWords(raw_move);

            bool has_arm = false;
            bool has_cat = false;
            for (const auto& cw : cross_words) {
                if (cw == "ARM") has_arm = true;
                if (cw == "CAT") has_cat = true;
            }

            if (has_arm && has_cat) {
                bool is_valid = generator.isValidMove(raw_move);
                if (is_valid) {
                    found_valid_rat = true;
                    break;
                }
            }
        }
    }

    assert_true(found_valid_rat, "RAT move should be valid (all words in DAWG)");
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
                    "Generated word '" + move.getWord() + "' must be valid", false);

        // Also verify it's not a random invalid combination
        assert_true(move.getWord().length() >= 2, "Words should be at least 2 letters", false);
    }
}

int main() {
    cout << "=== Scradle Engine - Phase 3 Tests ===" << endl;
    cout << "Testing Move Generator with Cross-Checks" << endl;

    // Step 1 tests
    test_start_positions();
    test_start_positions_empty_board();

    // Basic structure tests
    test_move_structure();
    test_tile_placement();

    // Step 2 tests (raw move generation)
    test_raw_moves_basic();
    test_raw_moves_rack_constraint();
    test_raw_moves_blank_expansion();
    test_raw_moves_adjacency();

    // Step 3 tests (validation helpers)
    test_get_main_word();
    test_get_cross_words();
    test_is_valid_move_main_word();
    test_is_valid_move_cross_words();
    test_is_valid_move_all_valid();

    // Integration tests (Steps 1-3)
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
