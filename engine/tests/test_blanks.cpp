#include "board.h"
#include "rack.h"
#include "move.h"
#include "move_generator.h"
#include "dawg.h"
#include "scorer.h"
#include "test_framework.h"
#include <iostream>

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_rack_with_blanks() {
    cout << "\n=== Test: Rack With Blanks ===" << endl;

    Rack rack("ABC?D");

    assert_equal(5, rack.size(), "Rack should have 5 tiles");
    assert_true(rack.hasTile('A'), "Rack should have A");
    assert_true(rack.hasTile('?'), "Rack should have blank");
    assert_equal(1, rack.countTile('?'), "Should have 1 blank");

    // Test removing blank
    rack.removeTile('?');
    assert_equal(4, rack.size(), "Rack should have 4 tiles after removing blank");
    assert_false(rack.hasTile('?'), "Rack should not have blank after removal");

    // Test adding blank
    rack.addTile('?');
    assert_equal(5, rack.size(), "Rack should have 5 tiles after adding blank");
    assert_true(rack.hasTile('?'), "Rack should have blank after adding");
}

void test_move_generator_with_blank() {
    cout << "\n=== Test: Move Generator With Blank ===" << endl;

    Board board;
    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("BONJO?R");  // BONJOUR with one blank
    MoveGenerator gen(board, rack, dawg);

    auto moves = gen.generateMoves();

    assert_true(moves.size() > 0, "Should generate moves with blank");

    // Check that moves contain TilePlacement data with is_blank flags
    bool found_blank = false;
    int count_with_blanks = 0;
    for (const auto& move : moves) {
        bool has_blank_in_move = false;
        for (const auto& placement : move.getPlacements()) {
            if (placement.is_blank) {
                has_blank_in_move = true;
                break;
            }
        }
        if (has_blank_in_move) {
            count_with_blanks++;
            if (!found_blank) {
                cout << "  Found move using blank: " << move.getWord()
                     << " at (" << move.getStartRow() << "," << move.getStartCol() << ")" << endl;
                found_blank = true;
            }
        }
    }

    cout << "  Total moves with blanks: " << count_with_blanks << " / " << moves.size() << endl;
    assert_true(found_blank, "Should find at least one move using blank tile");
}

void test_blank_generates_more_moves() {
    cout << "\n=== Test: Blank Generates More Moves ===" << endl;

    Board board;
    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    // Generate moves without blank
    Rack rack_no_blank("BONJOUR");
    MoveGenerator gen_no_blank(board, rack_no_blank, dawg);
    auto moves_no_blank = gen_no_blank.generateMoves();

    // Generate moves with blank instead of U
    Rack rack_with_blank("BONJO?R");
    MoveGenerator gen_with_blank(board, rack_with_blank, dawg);
    auto moves_with_blank = gen_with_blank.generateMoves();

    cout << "  Moves without blank: " << moves_no_blank.size() << endl;
    cout << "  Moves with blank: " << moves_with_blank.size() << endl;

    // Blank should enable more moves (or at least equal)
    assert_true(moves_with_blank.size() >= moves_no_blank.size(),
                "Blank should enable at least as many moves");
}

void test_blank_on_board_recognition() {
    cout << "\n=== Test: Blank On Board Recognition ===" << endl;

    Board board;
    Scorer scorer;

    // Place a blank 'b' (lowercase) on the board at center
    board.setLetter(7, 7, 'b');

    // Verify board stores lowercase
    char letter = board.getLetter(7, 7);
    assert_equal('b', letter, "Board should store lowercase for blank");

    // Create a manual move that uses the blank on board
    Move move(7, 7, Direction::HORIZONTAL, "BAT");
    move.addPlacement(TilePlacement(7, 7, 'B', false, true));  // existing blank
    move.addPlacement(TilePlacement(7, 8, 'A', true, false));   // new tile
    move.addPlacement(TilePlacement(7, 9, 'T', true, false));   // new tile

    // Score should reflect that B is a blank (0 points), and DW doesn't apply since B is existing
    // B=0 (blank, existing), A=1, T=1 = 2 points total
    int score = scorer.scoreMove(board, move);
    assert_equal(2, score, "BAT with existing blank should score 2 points");

    cout << "  Blank on board correctly recognized and scored as 0 points" << endl;
}

void test_blank_on_board_in_generated_move() {
    cout << "\n=== Test: Blank On Board In Generated Move ===" << endl;

    Board board;
    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    // Place a blank 'o' on the board at center (lowercase = blank)
    board.setLetter(7, 7, 'o');

    // Place 'N' next to it so we have "ON" on board (with blank 'o')
    board.setLetter(7, 8, 'N');

    // Try to generate moves adjacent to these tiles
    Rack rack("JUR");
    MoveGenerator gen(board, rack, dawg);
    auto moves = gen.generateMoves();

    // Look for a move containing "ON" that goes through one of our tiles
    bool found_move_with_on = false;
    for (const auto& move : moves) {
        bool has_o_or_n = false;
        bool has_blank_at_7_7 = false;

        for (const auto& placement : move.getPlacements()) {
            if ((placement.row == 7 && placement.col == 7) ||
                (placement.row == 7 && placement.col == 8)) {
                has_o_or_n = true;
                if (placement.row == 7 && placement.col == 7 && placement.is_blank) {
                    has_blank_at_7_7 = true;
                }
            }
        }

        if (has_o_or_n) {
            found_move_with_on = true;
            cout << "  Found move using existing tiles: " << move.getWord()
                 << " at (" << move.getStartRow() << "," << move.getStartCol() << ")";
            if (has_blank_at_7_7) {
                cout << " (includes blank 'o')";
            }
            cout << endl;
            break;
        }
    }

    assert_true(found_move_with_on, "Should find moves using existing tiles including blank");
}

void test_integrated_blank_scoring() {
    cout << "\n=== Test: Integrated Blank Scoring ===" << endl;

    Board board;
    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");
    Scorer scorer;

    Rack rack("BONJO?R");
    MoveGenerator gen(board, rack, dawg);

    auto moves = gen.generateMoves();

    // Find a move that uses the blank and verify its score
    int moves_checked = 0;
    for (auto& move : moves) {
        bool has_blank = false;
        for (const auto& placement : move.getPlacements()) {
            if (placement.is_blank) {
                has_blank = true;
                break;
            }
        }

        if (has_blank) {
            int score = scorer.scoreMove(board, move);
            move.setScore(score);

            if (moves_checked == 0) {
                cout << "  Move: " << move.getWord()
                     << " at (" << move.getStartRow() << "," << move.getStartCol() << ")"
                     << " Score: " << score << endl;
            }

            // Score should be >= 0
            assert_true(score >= 0, "Move with blank should have non-negative score");

            moves_checked++;
            if (moves_checked >= 3) break;  // Check first 3 moves with blanks
        }
    }

    if (moves_checked > 0) {
        cout << "  Verified " << moves_checked << " moves with blanks" << endl;
    }
}

int main() {
    cout << "=== Scradle Engine - Blank Tile Tests ===" << endl;

    test_rack_with_blanks();
    test_move_generator_with_blank();
    test_blank_generates_more_moves();
    test_blank_on_board_recognition();
    test_blank_on_board_in_generated_move();
    test_integrated_blank_scoring();

    print_summary();

    return exit_code();
}
