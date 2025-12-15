#include "board.h"
#include "rack.h"
#include "test_framework.h"
#include <iostream>

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_board_creation() {
    cout << "\n=== Test: Board Creation ===" << endl;

    Board board;

    assert_true(board.isBoardEmpty(), "New board should be empty");
    assert_true(!board.isCenterOccupied(), "New board center should not be occupied");
    assert_true(board.isValidPosition(0, 0), "Position (0,0) should be valid");
    assert_true(board.isValidPosition(14, 14), "Position (14,14) should be valid");
    assert_true(!board.isValidPosition(-1, 0), "Position (-1,0) should be invalid");
    assert_true(!board.isValidPosition(0, 15), "Position (0,15) should be invalid");
}

void test_board_premium_squares() {
    cout << "\n=== Test: Premium Squares ===" << endl;

    Board board;

    // Test corner triple word squares
    assert_equal(PremiumType::TRIPLE_WORD, board.getCell(0, 0).premium, "A1 should be triple word");
    assert_equal(PremiumType::TRIPLE_WORD, board.getCell(0, 14).premium, "O1 should be triple word");
    assert_equal(PremiumType::TRIPLE_WORD, board.getCell(14, 0).premium, "A15 should be triple word");
    assert_equal(PremiumType::TRIPLE_WORD, board.getCell(14, 14).premium, "O15 should be triple word");

    // Test center double word
    assert_equal(PremiumType::DOUBLE_WORD, board.getCell(7, 7).premium, "H8 (center) should be double word");

    // Test some double letter squares
    assert_equal(PremiumType::DOUBLE_LETTER, board.getCell(0, 3).premium, "D1 should be double letter");
    assert_equal(PremiumType::DOUBLE_LETTER, board.getCell(7, 3).premium, "D8 should be double letter");

    // Test some triple letter squares
    assert_equal(PremiumType::TRIPLE_LETTER, board.getCell(1, 5).premium, "F2 should be triple letter");
    assert_equal(PremiumType::TRIPLE_LETTER, board.getCell(5, 1).premium, "B6 should be triple letter");
}

void test_board_letter_placement() {
    cout << "\n=== Test: Letter Placement ===" << endl;

    Board board;

    // Initially empty
    assert_true(board.isEmpty(7, 7), "H8 should be empty initially");
    assert_equal(' ', board.getLetter(7, 7), "H8 should contain space initially");

    // Place a letter
    board.setLetter(7, 7, 'A');

    assert_true(!board.isEmpty(7, 7), "H8 should not be empty after placing letter");
    assert_equal('A', board.getLetter(7, 7), "H8 should contain 'A'");
    assert_true(!board.isBoardEmpty(), "Board should not be empty after placing letter");
    assert_true(board.isCenterOccupied(), "Center should be occupied");

    // Place another letter
    board.setLetter(7, 8, 'B');
    assert_equal('B', board.getLetter(7, 8), "I8 should contain 'B'");
}

void test_rack_creation() {
    cout << "\n=== Test: Rack Creation ===" << endl;

    Rack empty_rack;
    assert_equal(0, empty_rack.size(), "Empty rack should have size 0");

    Rack rack("abcdefg");
    assert_equal(7, rack.size(), "Rack with 7 letters should have size 7");
    assert_equal(string("ABCDEFG"), rack.toString(), "Rack should uppercase letters");
}

void test_rack_operations() {
    cout << "\n=== Test: Rack Operations ===" << endl;

    Rack rack("ABCDEFG");

    // Test hasTile
    assert_true(rack.hasTile('A'), "Rack should have 'A'");
    assert_true(rack.hasTile('a'), "Rack should have 'a' (case insensitive)");
    assert_true(!rack.hasTile('Z'), "Rack should not have 'Z'");

    // Test countTile
    assert_equal(1, rack.countTile('A'), "Rack should have 1 'A'");
    assert_equal(0, rack.countTile('Z'), "Rack should have 0 'Z'");

    // Test removeTile
    rack.removeTile('A');
    assert_equal(6, rack.size(), "Rack should have size 6 after removing 'A'");
    assert_true(!rack.hasTile('A'), "Rack should not have 'A' after removal");
    assert_equal(string("BCDEFG"), rack.toString(), "Rack should be 'BCDEFG' after removing 'A'");

    // Test addTile
    rack.addTile('X');
    assert_equal(7, rack.size(), "Rack should have size 7 after adding 'X'");
    assert_true(rack.hasTile('X'), "Rack should have 'X' after adding it");

    // Test max capacity
    rack.addTile('Y'); // Should not be added (already at max)
    assert_equal(7, rack.size(), "Rack should still have size 7 (max capacity)");
    assert_true(!rack.hasTile('Y'), "Rack should not have 'Y' (exceeded capacity)");
}

void test_rack_duplicate_letters() {
    cout << "\n=== Test: Rack Duplicate Letters ===" << endl;

    Rack rack("AABBBCC");

    assert_equal(7, rack.size(), "Rack should have size 7");
    assert_equal(2, rack.countTile('A'), "Rack should have 2 'A's");
    assert_equal(3, rack.countTile('B'), "Rack should have 3 'B's");
    assert_equal(2, rack.countTile('C'), "Rack should have 2 'C's");

    rack.removeTile('B');
    assert_equal(2, rack.countTile('B'), "Rack should have 2 'B's after removing one");
    assert_equal(6, rack.size(), "Rack should have size 6 after removal");
}

void test_cell_properties() {
    cout << "\n=== Test: Cell Properties ===" << endl;

    Cell empty_cell;
    assert_true(empty_cell.isEmpty(), "Default cell should be empty");
    assert_equal(' ', empty_cell.letter, "Default cell should have space");
    assert_equal(PremiumType::NONE, empty_cell.premium, "Default cell should have no premium");

    Cell letter_cell('A', PremiumType::DOUBLE_WORD);
    assert_true(!letter_cell.isEmpty(), "Cell with letter should not be empty");
    assert_equal('A', letter_cell.letter, "Cell should have letter 'A'");
    assert_equal(PremiumType::DOUBLE_WORD, letter_cell.premium, "Cell should have double word premium");
}

int main() {
    cout << "=== Scradle Engine - Phase 1 Tests ===" << endl;

    test_board_creation();
    test_board_premium_squares();
    test_board_letter_placement();
    test_rack_creation();
    test_rack_operations();
    test_rack_duplicate_letters();
    test_cell_properties();

    print_summary();

    return exit_code();
}
