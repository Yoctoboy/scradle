#include "board.h"
#include "scorer.h"
#include "move.h"
#include "test_framework.h"
#include <iostream>

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_letter_values() {
    cout << "\n=== Test: Letter Values ===" << endl;

    Scorer scorer;

    // Test 1-point letters
    assert_equal(1, scorer.getLetterValue('E'), "'E' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('A'), "'A' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('I'), "'I' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('N'), "'N' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('O'), "'O' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('R'), "'R' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('S'), "'S' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('T'), "'T' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('U'), "'U' should be worth 1 point");
    assert_equal(1, scorer.getLetterValue('L'), "'L' should be worth 1 point");

    // Test 2-point letters
    assert_equal(2, scorer.getLetterValue('D'), "'D' should be worth 2 points");
    assert_equal(2, scorer.getLetterValue('M'), "'M' should be worth 2 points");
    assert_equal(2, scorer.getLetterValue('G'), "'G' should be worth 2 points");

    // Test 3-point letters
    assert_equal(3, scorer.getLetterValue('B'), "'B' should be worth 3 points");
    assert_equal(3, scorer.getLetterValue('C'), "'C' should be worth 3 points");
    assert_equal(3, scorer.getLetterValue('P'), "'P' should be worth 3 points");

    // Test 4-point letters
    assert_equal(4, scorer.getLetterValue('F'), "'F' should be worth 4 points");
    assert_equal(4, scorer.getLetterValue('H'), "'H' should be worth 4 points");
    assert_equal(4, scorer.getLetterValue('V'), "'V' should be worth 4 points");

    // Test 8-point letters
    assert_equal(8, scorer.getLetterValue('J'), "'J' should be worth 8 points");
    assert_equal(8, scorer.getLetterValue('Q'), "'Q' should be worth 8 points");

    // Test 10-point letters
    assert_equal(10, scorer.getLetterValue('K'), "'K' should be worth 10 points");
    assert_equal(10, scorer.getLetterValue('W'), "'W' should be worth 10 points");
    assert_equal(10, scorer.getLetterValue('X'), "'X' should be worth 10 points");
    assert_equal(10, scorer.getLetterValue('Y'), "'Y' should be worth 10 points");
    assert_equal(10, scorer.getLetterValue('Z'), "'Z' should be worth 10 points");

    // Test case insensitivity
    assert_equal(1, scorer.getLetterValue('e'), "lowercase 'e' should be worth 1 point");
    assert_equal(10, scorer.getLetterValue('z'), "lowercase 'z' should be worth 10 points");
}

void test_basic_word_scoring() {
    cout << "\n=== Test: Basic Word Scoring ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" horizontally at center (no premiums except center DW)
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', true));
    move.addPlacement(TilePlacement(7, 8, 'A', true));
    move.addPlacement(TilePlacement(7, 9, 'T', true));

    // C=3, A=1, T=1 = 5 points base
    // Center square is DW, so 5 * 2 = 10 points
    int score = scorer.scoreMove(board, move);
    assert_equal(10, score, "CAT on center should score 10 points (5 * 2 for DW)");
}

void test_double_letter_scoring() {
    cout << "\n=== Test: Double Letter Scoring ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" with 'C' on double letter at (0, 3)
    Move move(0, 3, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(0, 3, 'C', true));
    move.addPlacement(TilePlacement(0, 4, 'A', true));
    move.addPlacement(TilePlacement(0, 5, 'T', true));

    // C=3*2=6 (DL), A=1, T=1 = 8 points
    int score = scorer.scoreMove(board, move);
    assert_equal(8, score, "CAT with C on DL should score 8 points");
}

void test_triple_letter_scoring() {
    cout << "\n=== Test: Triple Letter Scoring ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" with 'C' on triple letter at (1, 5)
    Move move(1, 5, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(1, 5, 'C', true));
    move.addPlacement(TilePlacement(1, 6, 'A', true));
    move.addPlacement(TilePlacement(1, 7, 'T', true));

    // C=3*3=9 (TL), A=1, T=1 = 11 points
    int score = scorer.scoreMove(board, move);
    assert_equal(11, score, "CAT with C on TL should score 11 points");
}

void test_triple_word_scoring() {
    cout << "\n=== Test: Triple Word Scoring ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" on triple word at corner (0, 0)
    Move move(0, 0, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(0, 0, 'C', true));
    move.addPlacement(TilePlacement(0, 1, 'A', true));
    move.addPlacement(TilePlacement(0, 2, 'T', true));

    // C=3, A=1, T=1 = 5 points base
    // TW multiplier: 5 * 3 = 15 points
    int score = scorer.scoreMove(board, move);
    assert_equal(15, score, "CAT on TW should score 15 points (5 * 3)");
}

void test_existing_tiles_no_premium() {
    cout << "\n=== Test: Existing Tiles Don't Get Premium ===" << endl;

    Board board;
    Scorer scorer;

    // Pre-place "CA" on the board
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 8, 'A');

    // Add only "T" to make "CAT"
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', false));  // Already on board
    move.addPlacement(TilePlacement(7, 8, 'A', false));  // Already on board
    move.addPlacement(TilePlacement(7, 9, 'T', true));   // New tile

    // Only the 'T' gets scored: T=1 point
    // No premium applied to existing tiles
    int score = scorer.scoreMove(board, move);
    assert_equal(5, score, "Adding T to CA should score 5 points (all letters counted, no premium)");
}

void test_bingo_bonus() {
    cout << "\n=== Test: Bingo Bonus ===" << endl;

    Board board;
    Scorer scorer;

    // Place a 7-letter word "PLAYERS" - using all 7 tiles from rack
    Move move(7, 7, Direction::HORIZONTAL, "PLAYERS");
    move.addPlacement(TilePlacement(7, 7, 'P', true));
    move.addPlacement(TilePlacement(7, 8, 'L', true));
    move.addPlacement(TilePlacement(7, 9, 'A', true));
    move.addPlacement(TilePlacement(7, 10, 'Y', true));
    move.addPlacement(TilePlacement(7, 11, 'E', true));  // (7,11) is DL
    move.addPlacement(TilePlacement(7, 12, 'R', true));
    move.addPlacement(TilePlacement(7, 13, 'S', true));

    // P=3, L=1, A=1, Y=10, E=1*2(DL)=2, R=1, S=1 = 19 points base
    // Center DW at (7,7): 19 * 2 = 38 points
    // Bingo bonus: +50 points
    // Total: 88 points
    int score = scorer.scoreMove(board, move);
    assert_equal(88, score, "7-letter word with DL should get 50-point bingo bonus");
}

void test_no_bingo_with_existing_tiles() {
    cout << "\n=== Test: No Bingo With Existing Tiles ===" << endl;

    Board board;
    Scorer scorer;

    // Pre-place one letter
    board.setLetter(7, 7, 'P');

    // Add 6 more letters (not a bingo since one was already there)
    Move move(7, 7, Direction::HORIZONTAL, "PLAYERS");
    move.addPlacement(TilePlacement(7, 7, 'P', false));  // Existing
    move.addPlacement(TilePlacement(7, 8, 'L', true));
    move.addPlacement(TilePlacement(7, 9, 'A', true));
    move.addPlacement(TilePlacement(7, 10, 'Y', true));
    move.addPlacement(TilePlacement(7, 11, 'E', true));  // (7,11) is DL
    move.addPlacement(TilePlacement(7, 12, 'R', true));
    move.addPlacement(TilePlacement(7, 13, 'S', true));

    // Should NOT get bingo bonus (only 6 new tiles)
    int score = scorer.scoreMove(board, move);
    // P=3, L=1, A=1, Y=10, E=1*2(DL)=2, R=1, S=1 = 19 points
    // No word multiplier (DW only applies to new tiles, and P is not new)
    // No bingo bonus
    assert_equal(19, score, "7-letter word with 1 existing tile should NOT get bingo bonus");
}

void test_premium_not_reused() {
    cout << "\n=== Test: Premium Square Not Reused ===" << endl;

    Board board;
    Scorer scorer;

    // First move: Place "CAT" on center (7,7) which has DW
    board.setLetter(7, 7, 'C');
    board.setLetter(7, 8, 'A');
    board.setLetter(7, 9, 'T');

    // Second move: Add "S" to make "CATS" - the DW should NOT apply again
    Move move(7, 7, Direction::HORIZONTAL, "CATS");
    move.addPlacement(TilePlacement(7, 7, 'C', false));  // Existing - DW already used
    move.addPlacement(TilePlacement(7, 8, 'A', false));  // Existing
    move.addPlacement(TilePlacement(7, 9, 'T', false));  // Existing
    move.addPlacement(TilePlacement(7, 10, 'S', true));  // New tile

    // All letters are scored: C=3, A=1, T=1, S=1 = 6 points
    // No premium multipliers apply (C is existing so DW doesn't apply)
    int score = scorer.scoreMove(board, move);
    assert_equal(6, score, "Adding S to existing CAT should score 6 points (no premium reuse)");
}

void test_high_value_letters() {
    cout << "\n=== Test: High Value Letters ===" << endl;

    Board board;
    Scorer scorer;

    // Place "QUIZ" starting at (5,5) which is TL
    // Q=8, U=1, I=1, Z=10
    Move move(5, 5, Direction::HORIZONTAL, "QUIZ");
    move.addPlacement(TilePlacement(5, 5, 'Q', true));  // (5,5) is TL
    move.addPlacement(TilePlacement(5, 6, 'U', true));
    move.addPlacement(TilePlacement(5, 7, 'I', true));
    move.addPlacement(TilePlacement(5, 8, 'Z', true));

    // Q=8*3(TL)=24, U=1, I=1, Z=10 = 36 points
    int score = scorer.scoreMove(board, move);
    assert_equal(36, score, "QUIZ with Q on TL should score 36 points");
}

void test_blank_tile_basic() {
    cout << "\n=== Test: Blank Tile Basic Scoring ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" with C as a blank (worth 0 points)
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', true, true));  // blank as C
    move.addPlacement(TilePlacement(7, 8, 'A', true, false)); // regular A
    move.addPlacement(TilePlacement(7, 9, 'T', true, false)); // regular T

    // C=0 (blank), A=1, T=1 = 2 points base
    // Center DW at (7,7): 2 * 2 = 4 points
    int score = scorer.scoreMove(board, move);
    assert_equal(4, score, "CAT with blank C on center should score 4 points (2 * 2 for DW)");
}

void test_blank_on_premium_square() {
    cout << "\n=== Test: Blank On Premium Square ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" with C as blank on TL at (1,5)
    Move move(1, 5, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(1, 5, 'C', true, true));  // blank on TL
    move.addPlacement(TilePlacement(1, 6, 'A', true, false));
    move.addPlacement(TilePlacement(1, 7, 'T', true, false));

    // C=0*3(TL)=0 (blank), A=1, T=1 = 2 points
    // No word multiplier
    int score = scorer.scoreMove(board, move);
    assert_equal(2, score, "Blank on TL should still be worth 0 points");
}

void test_multiple_blanks() {
    cout << "\n=== Test: Multiple Blank Tiles ===" << endl;

    Board board;
    Scorer scorer;

    // Place "CAT" with C and T as blanks
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', true, true));  // blank
    move.addPlacement(TilePlacement(7, 8, 'A', true, false));
    move.addPlacement(TilePlacement(7, 9, 'T', true, true));  // blank

    // C=0, A=1, T=0 = 1 point base
    // Center DW: 1 * 2 = 2 points
    int score = scorer.scoreMove(board, move);
    assert_equal(2, score, "Two blanks should both be worth 0 points");
}

void test_blank_on_board() {
    cout << "\n=== Test: Blank Already On Board ===" << endl;

    Board board;
    Scorer scorer;

    // Pre-place a blank 'c' (lowercase) on board
    board.setLetter(7, 7, 'c');  // lowercase indicates blank

    // Add "AT" to make "CAT"
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', false, true)); // existing blank
    move.addPlacement(TilePlacement(7, 8, 'A', true, false));
    move.addPlacement(TilePlacement(7, 9, 'T', true, false));

    // C=0 (blank, already on board), A=1, T=1 = 2 points
    // No premium (blank already placed)
    int score = scorer.scoreMove(board, move);
    assert_equal(2, score, "Existing blank should be worth 0 points");
}

int main() {
    cout << "=== Scradle Engine - Phase 4 Tests ===" << endl;
    cout << "Testing Scoring System" << endl;

    test_letter_values();
    test_basic_word_scoring();
    test_double_letter_scoring();
    test_triple_letter_scoring();
    test_triple_word_scoring();
    test_existing_tiles_no_premium();
    test_bingo_bonus();
    test_no_bingo_with_existing_tiles();
    test_premium_not_reused();
    test_high_value_letters();
    test_blank_tile_basic();
    test_blank_on_premium_square();
    test_multiple_blanks();
    test_blank_on_board();

    print_summary();

    return exit_code();
}
