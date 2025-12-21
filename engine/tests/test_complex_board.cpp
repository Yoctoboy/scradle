#include <algorithm>
#include <iostream>

#include "board.h"
#include "dawg.h"
#include "move_generator.h"
#include "rack.h"
#include "scorer.h"
#include "test_framework.h"

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_board_parser() {
    cout << "\n=== Test: Board Parser ===" << endl;

    // Simple test case - creating a board with CAT horizontally and ARM vertically
    // Using C++11 raw string literal for clean multi-line format
    // The parser automatically skips leading spaces/tabs for indentation
    Board board = Board::parseBoard(R"(
        ....CAT........
        .....R.........
        .....M.........
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
    )");

    // Verify CAT is placed horizontally
    assert_equal('C', board.getLetter(0, 4), "C should be at row 0, col 4");
    assert_equal('A', board.getLetter(0, 5), "A should be at row 0, col 5");
    assert_equal('T', board.getLetter(0, 6), "T should be at row 0, col 6");

    // Verify ARM is placed vertically (A from CAT, then R, M)
    assert_equal('R', board.getLetter(1, 5), "R should be at row 1, col 5");
    assert_equal('M', board.getLetter(2, 5), "M should be at row 2, col 5");

    // Verify empty cells
    assert_equal(' ', board.getLetter(0, 0), "Row 0, col 0 should be empty");
    assert_equal(' ', board.getLetter(5, 5), "Row 5, col 5 should be empty");

    cout << "  Board parsing works! You can use raw strings with indentation" << endl;
}

void test_example_complex_board() {
    cout << "\n=== Test: Example Complex Board ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        .......CAT.....
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("HELLO");
    MoveGenerator gen(board, rack, dawg);
    auto moves = gen.generateMoves();

    cout << "  Generated " << moves.size() << " moves with rack HELLO" << endl;
    assert_true(moves.size() > 0, "Should generate at least some moves");

    Scorer scorer;
    for (auto& move : moves) {
        int score = scorer.scoreMove(board, move);
        move.setScore(score);
    }

    // Sort by score
    std::sort(moves.begin(), moves.end(),
              [](const Move& a, const Move& b) { return a.getScore() > b.getScore(); });

    // Show top 5 moves
    cout << "\n  Top 5 moves:" << endl;
    for (int i = 0; i < std::min(5, (int)moves.size()); ++i) {
        const auto& m = moves[i];
        cout << "    " << (i + 1) << ". " << m.toString() << endl;
    }
}

int main() {
    cout << "=== Scradle Engine - Complex Board Tests ===" << endl;

    test_board_parser();
    test_example_complex_board();

    // TODO: Add more test functions here
    // test_my_custom_scenario_1();
    // test_my_custom_scenario_2();
    // etc.

    print_summary();

    return exit_code();
}
