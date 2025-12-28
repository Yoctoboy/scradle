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
}

void test_example_board() {
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
    auto moves = gen.getBestMove();

    cout << "  Generated " << moves.size() << " moves with rack HELLO" << endl;
    assert_equal(1, (int)moves.size(), "Should generate at least some moves");
    assert_equal(moves[0].toString(), std::string("OHE at G8 [25 pts]"), "Best move found is \"OHE\"");
}

void test_board_scenario_1() {
    cout << "\n=== Test: Example Complex Board 1 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ...............
        ...............
        ...............
        ..........M....
        ..........U....
        ..........L....
        ..........U....
        ...MIXTES.D....
        ......O.KAS....
        ......U.AH.....
        ......R.T......
        ......N.E......
        ..CE..O.RABIOLE
        CEIGNAIS.......
        INFO..E........
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("WANRRQU");
    MoveGenerator gen(board, rack, dawg);
    auto top_moves = gen.getTopMoves(5);

    assert_equal(std::string("QUARRE at 15H [42 pts]"), top_moves[0].toString(), "Top 1 move is QUARRE at 15H [42 pts]");
    assert_equal(std::string("WAX at 6F [41 pts]"), top_moves[1].toString(), "Top 2 move is WAX at 6F [41 pts]");
    assert_equal(std::string("QUENA at 15K [39 pts]"), top_moves[2].toString(), "Top 3 move is QUENA at 15K [39 pts]");
}

void test_board_scenario_2() {
    cout << "\n=== Test: Example Complex Board 2 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ...........H...
        ...........A...
        ...........Z...
        ..........MA...
        ..........UN...
        ..........L....
        ..........U....
        ...MIXTES.D...Q
        ......O.KAS...U
        ......U.AH....A
        ......R.T.....R
        ......N.E.....R
        ..CE..O.RABIOLE
        CEIGNAIS......N
        INFO..E.......T
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("APUUWE?");
    MoveGenerator gen(board, rack, dawg);
    auto top_moves = gen.getTopMoves(5);

    cout << "  Top moves:" << endl;
    for (size_t i = 0; i < top_moves.size() && i < 5; i++) {
        cout << "    " << i << ": " << top_moves[i].toString() << endl;
    }

    assert_equal(std::string("WAOUH at A8 [48 pts]"), top_moves[0].toString(), "Top 1 move is WAOUH at A8 [48 pts]");
    assert_equal(std::string("WAX at 6F [41 pts]"), top_moves[1].toString(), "Top 2 move is WAX at 6F [41 pts]");
    assert_equal(std::string("WAX at 6F [40 pts]"), top_moves[2].toString(), "Top 3 move is WAX at 6F [40 pts]");
}

void test_board_scenario_3() {
    cout << "\n=== Test: Example Complex Board 3 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ...JELLOS......
        ......I........
        ....T.M........
        ....I.O........
        ....P.U........
        ....E.S........
        ....R.I........
        ..SCANNE.......
        ....I..........
        ....TWEETEUR...
        ...............
        ...............
        ...............
        ...............
        ...............
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("EAITDON");
    MoveGenerator gen(board, rack, dawg);
    auto top_moves = gen.getBestMove();

    if(!assert_equal(1, (int)top_moves.size(), "Only one best move found")){
        return;
    }
    assert_equal(std::string("DORAIENT at 12H [74 pts] [BINGO]"), top_moves[0].toString(), "Top move is DORAIENT at 12H [74 pts]");
}

void test_board_scenario_4() {
    cout << "\n=== Test: Example Complex Board 4 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ......OVOIDE..E
        .........X....V
        ........DOLCE.Z
        .........N.INFO
        ........OSE...N
        ........U.X...E
        ........P.T....
        .....EWES.R....
        ..........U....
        ..........D....
        .......MATELOTS
        ......RAIERA...
        LAMIFIE........
        ...............
        ...............
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("CQSUUGH");
    MoveGenerator gen(board, rack, dawg);
    auto top_moves = gen.getBestMove();

    if(!assert_equal(2, (int)top_moves.size(), "Two best moves found")){
        return;
    }
    assert_equal(std::string("THUGS at G11 [29 pts]"), top_moves[0].toString(), "Top move is THUGS at G11 [29 pts]");
    assert_equal(std::string("CHUS at I5 [29 pts]"), top_moves[1].toString(), "Second top move is CHUS at I5 [29 pts]");
}

void test_board_scenario_5() {
    cout << "\n=== Test: Example Complex Board 4 (Blank on board 'r' in MOrGUE) ===" << endl;

    // Example board - 'r' is lowercase = blank tile worth 0 points
    Board board = Board::parseBoard(R"(
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...MOrGUE......
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

    Rack rack("TEONAEE");
    MoveGenerator gen(board, rack, dawg);
    auto top_moves = gen.getTopMoves(5);

    // The issue: cross-words that include the blank 'r' should score it as 0 points
    // Check if we're scoring correctly
    assert_equal(std::string("ENTAMEE at 4D [16 pts]"), top_moves[0].toString(), "Top move is ENTAMEE at 4D [16 pts]");
    assert_equal(15, top_moves[1].getScore(), "Second top move scores 15 pts");

    // Check that NOTEE scores 15 (not 16), showing the blank 'r' is counted as 0
    bool found_notee_15pts = false;
    for (const auto& move : top_moves) {
        if (move.getWord() == "NOTEE" && move.getScore() == 15) {
            found_notee_15pts = true;
            break;
        }
    }
    assert_true(found_notee_15pts, "NOTEE should score 15 pts (blank 'r' counted as 0)");
}

void test_board_scenario_6() {
    cout << "\n=== Test: Example Complex Board 6 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        ...............
        .......ON......
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

    Rack rack("JUR");
    MoveGenerator gen(board, rack, dawg);

    auto top_moves = gen.getBestMove();

    assert_equal(std::string("JURON at H5 [12 pts]"), top_moves[0].toString(), "Top move is JURON at H5 [12 pts]");
}

void test_board_scenario_7() {
    cout << "\n=== Test: Example Complex Board 7 ===" << endl;

    // Example board - replace with your own test cases
    Board board = Board::parseBoard(R"(
        ......CABILLAU.
        ...........E.NE
        ...........G..S
        .......VERMOUT.
        .............AY
        .........TURNE.
        .............NO
        ...KLAXONNERAI.
        ...I...T.....AH
        WURTEMbERGEOIS.
        .....A.E......Q
        .....N..S...D.U
        .....SOLO.L.API
        ......RAFFErMI.
        DESUBJECTIVISE.
    )");

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    Rack rack("DEEHPTZ");
    MoveGenerator gen(board, rack, dawg);

    auto top_moves = gen.getBestMove();

    assert_equal(std::string("DESHYPOTHEQUIEZ at 15A [1797 pts] [BINGO]"), top_moves[0].toString(), "Top move is DESHYPOTHEQUIEZ at 15A [1797 pts]");
}

int main() {
    cout << "=== Scradle Engine - Complex Board Tests ===" << endl;

    // test_board_parser();
    // test_example_board();
    TIME_TEST("scenario 1", test_board_scenario_1);
    TIME_TEST("scenario 2", test_board_scenario_2);
    // test_board_scenario_2();
    // test_board_scenario_3();
    // test_board_scenario_4();
    // test_board_scenario_5();
    // test_board_scenario_6();
    // test_board_scenario_7();

    print_summary();

    return exit_code();
}
