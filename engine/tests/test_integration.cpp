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

void setup_real_game_board(Board& board) {
    // Row B (index 1): RHE at columns 4-6, P at column 11
    board.setLetter(1, 4, 'R');
    board.setLetter(1, 5, 'H');
    board.setLetter(1, 6, 'E');
    board.setLetter(1, 11, 'P');

    // Row C (index 2): ENFANCES at columns 5-12
    board.setLetter(2, 5, 'E');
    board.setLetter(2, 6, 'N');
    board.setLetter(2, 7, 'F');
    board.setLetter(2, 8, 'A');
    board.setLetter(2, 9, 'N');
    board.setLetter(2, 10, 'C');
    board.setLetter(2, 11, 'E');
    board.setLetter(2, 12, 'S');

    // Row D (index 3): T at column 11
    board.setLetter(3, 11, 'T');

    // Row E (index 4): C at column 4, U at column 11
    board.setLetter(4, 3, 'C');
    board.setLetter(4, 11, 'U');

    // Row F (index 5): I at column 4, PUNIR at columns 8-12
    board.setLetter(5, 3, 'I');
    board.setLetter(5, 9, 'P');
    board.setLetter(5, 10, 'U');
    board.setLetter(5, 11, 'N');
    board.setLetter(5, 12, 'I');
    board.setLetter(5, 13, 'R');

    // Row G (index 6): T at column 4, ITOU at columns 6-9, E at column 11
    board.setLetter(6, 3, 'T');
    board.setLetter(6, 6, 'I');
    board.setLetter(6, 7, 'T');
    board.setLetter(6, 8, 'O');
    board.setLetter(6, 9, 'U');
    board.setLetter(6, 11, 'E');

    // Row H (index 7): E at column 4, LESTEZ at columns 6-11
    board.setLetter(7, 3, 'E');
    board.setLetter(7, 6, 'L');
    board.setLetter(7, 7, 'E');
    board.setLetter(7, 8, 'S');
    board.setLetter(7, 9, 'T');
    board.setLetter(7, 10, 'E');
    board.setLetter(7, 11, 'Z');

    // Row I (index 8): R at column 4, BEKE at columns 5-8
    board.setLetter(8, 3, 'R');
    board.setLetter(8, 5, 'B');
    board.setLetter(8, 6, 'E');
    board.setLetter(8, 7, 'K');
    board.setLetter(8, 8, 'E');

    // Row J (index 9): JOUI at columns 3-6
    board.setLetter(9, 2, 'J');
    board.setLetter(9, 3, 'O');
    board.setLetter(9, 4, 'U');
    board.setLetter(9, 5, 'I');

    // Row K (index 10): N at column 4
    board.setLetter(10, 3, 'N');

    // Row L (index 11): T at column 4
    board.setLetter(11, 3, 'T');
}

void print_board_state(const Board& board) {
    cout << "\n  Board State (row labels A-O, col labels 1-15):" << endl;
    cout << "     ";
    for (int col = 0; col < 15; ++col) {
        cout << " " << (col + 1);
        if (col + 1 < 10) cout << " ";
    }
    cout << endl;

    const char* row_labels = "ABCDEFGHIJKLMNO";
    for (int row = 0; row < 15; ++row) {
        cout << "  " << row_labels[row] << "  ";
        for (int col = 0; col < 15; ++col) {
            char letter = board.getLetter(row, col);
            if (letter == ' ') {
                cout << " . ";
            } else {
                cout << " " << letter << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void test_real_game_board_setup() {
    cout << "\n=== Test: Real Game Board Setup ===" << endl;

    Board board;
    setup_real_game_board(board);

    // Print the board for visual verification
    print_board_state(board);

    // Verify some key tiles are placed correctly
    assert_equal('R', board.getLetter(1, 4), "R should be at B5");
    assert_equal('H', board.getLetter(1, 5), "H should be at B6");
    assert_equal('E', board.getLetter(1, 6), "E should be at B7");
    assert_equal('E', board.getLetter(2, 5), "E should be at C6");
    assert_equal('N', board.getLetter(2, 6), "N should be at C7");
    assert_equal('S', board.getLetter(2, 12), "S should be at C13");
    assert_equal('Z', board.getLetter(7, 11), "Z should be at H12");
    assert_equal('J', board.getLetter(9, 2), "J should be at J3");

    assert_false(board.isBoardEmpty(), "Board should not be empty");
}

void test_real_game_move_generation() {
    cout << "\n=== Test: Real Game Move Generation ===" << endl;

    Board board;
    setup_real_game_board(board);

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    // Rack: A I V O E Q I
    Rack rack("AIVOEQI");
    MoveGenerator gen(board, rack, dawg);

    auto moves = gen.generateMoves();

    cout << "  Generated " << moves.size() << " valid moves" << endl;
    assert_true(moves.size() > 0, "Should generate at least some moves");

    // Check that all generated moves are valid words
    for (const auto& move : moves) {
        assert_true(dawg.contains(move.getWord()),
                    "Generated word '" + move.toString() + "' should be in dictionary");
    }

    cout << "  All generated moves are valid words" << endl;
}

void test_find_best_move() {
    cout << "\n=== Test: Find best move ===" << endl;

    Board board;
    setup_real_game_board(board);

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    // Rack: A I V O E Q I
    Rack rack("AIVOEQI");
    MoveGenerator gen(board, rack, dawg);

    auto moves = gen.generateMoves();

    cout << "  Generated " << moves.size() << " valid moves" << endl;
    assert_true(moves.size() > 0, "Should generate at least some moves");

    Scorer scorer;
    int max_score = 0;
    Move best_move;
    Move qi_move;
    bool found_qi = false;

    for (auto& move : moves) {
        int score = scorer.scoreMove(board, move);
        move.setScore(score);

        if (score > max_score) {
            max_score = score;
            best_move = move;
        }

        // Look for QI at 5D (vertical at row 3, col 4)
        if (move.getWord() == "QI" &&
            move.getStartRow() == 3 &&
            move.getStartCol() == 4 &&
            move.getDirection() == Direction::VERTICAL) {
            qi_move = move;
            found_qi = true;
        }
    }

    cout << "  Best move found: " << best_move.toString() << endl;
    assert_true(best_move.getWord() == "AVOIE", "AVOIE was found");
    assert_true(best_move.getScore() == 32, "AVOIE is worth 32 points");

    cout << "  QI move found: " << (found_qi ? qi_move.toString() : "NOT FOUND") << endl;
    assert_true(found_qi, "QI at 5D should be generated");
    assert_true(qi_move.getScore() == 26, "QI at 5D should score 26 points");
}

void test_real_game_specific_moves() {
    cout << "\n=== Test: Real Game Specific Move Scoring ===" << endl;

    Board board;
    setup_real_game_board(board);

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");
    Scorer scorer;

    Rack rack("AIVOEQI");
    MoveGenerator gen(board, rack, dawg);

    auto moves = gen.generateMoves();

    // Score all moves and find the highest scoring ones
    int max_score = 0;
    Move best_move;

    for (auto& move : moves) {
        int score = scorer.scoreMove(board, move);
        move.setScore(score);

        if (score > max_score) {
            max_score = score;
            best_move = move;
        }
    }

    cout << "  Best move found: " << best_move.toString() << endl;

    assert_true(max_score > 0, "Best move should have positive score");

    // Display top 10 moves
    std::vector<Move> sorted_moves = moves;
    std::sort(sorted_moves.begin(), sorted_moves.end(),
              [](const Move& a, const Move& b) { return a.getScore() > b.getScore(); });

    cout << "\n  Top 10 moves:" << endl;
    for (int i = 0; i < std::min(10, (int)sorted_moves.size()); ++i) {
        const auto& m = sorted_moves[i];
        cout << "    " << (i + 1) << ". " << m.toString() << endl;
    }
}

int main() {
    cout << "=== Scradle Engine - Integration Tests ===" << endl;
    cout << "Testing with real game board and rack" << endl;

    test_real_game_board_setup();
    test_real_game_move_generation();
    test_real_game_specific_moves();
    test_find_best_move();

    print_summary();

    return exit_code();
}
