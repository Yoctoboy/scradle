#include <iostream>

#include "game_state.h"
#include "test_framework.h"

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_game_state_initialization() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Initialization ===" << color::RESET << endl;

    GameState state(42);

    assert_equal(42u, state.getSeed(), "Should have correct seed");
    assert_equal(0, state.getTotalScore(), "Should start with 0 score");
    assert_equal(0, state.getMoveCount(), "Should start with 0 moves");
    assert_equal(0, state.getBingoCount(), "Should start with 0 bingos");
    assert_true(state.getBoard().isBoardEmpty(), "Board should be empty");
    assert_equal(0, state.getRack().size(), "Rack should be empty");
    assert_equal(102, state.getTileBag().remainingCount(), "Tile bag should have 102 tiles");
}

void test_game_state_refill_rack() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Refill Rack ===" << color::RESET << endl;

    GameState state(999);

    state.refillRack();
    assert_equal(7, state.getRack().size(), "Should draw 7 tiles for initial rack");
    assert_equal(95, state.getTileBag().remainingCount(), "Should have 95 tiles left in bag");

    // Draw 3 more tiles manually (simulate using tiles)
    state.getRack().removeTile(state.getRack().getTile(0));
    state.getRack().removeTile(state.getRack().getTile(0));
    state.getRack().removeTile(state.getRack().getTile(0));
    assert_equal(4, state.getRack().size(), "Should have 4 tiles after removing 3");

    state.refillRack();
    assert_equal(7, state.getRack().size(), "Should refill back to 7 tiles");
    assert_equal(92, state.getTileBag().remainingCount(), "Should have 92 tiles left in bag");
}

void test_game_state_apply_move() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Apply Move ===" << color::RESET << endl;

    GameState state(123);
    state.refillRack();

    // Create a simple move (placing "CAT" horizontally at center)
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', true, false));
    move.addPlacement(TilePlacement(7, 8, 'A', true, false));
    move.addPlacement(TilePlacement(7, 9, 'T', true, false));
    move.setScore(15);

    state.applyMove(move);

    assert_equal(15, state.getTotalScore(), "Score should be 15");
    assert_equal(1, state.getMoveCount(), "Should have 1 move");
    assert_equal(0, state.getBingoCount(), "Should have 0 bingos (only 3 tiles)");
    assert_equal('C', state.getBoard().getLetter(7, 7), "C should be at (7,7)");
    assert_equal('A', state.getBoard().getLetter(7, 8), "A should be at (7,8)");
    assert_equal('T', state.getBoard().getLetter(7, 9), "T should be at (7,9)");
}

void test_game_state_bingo_detection() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Bingo Detection ===" << color::RESET << endl;

    GameState state(456);
    state.refillRack();

    // Create a 7-tile move (bingo)
    Move bingo_move(7, 7, Direction::HORIZONTAL, "PLAYING");
    bingo_move.addPlacement(TilePlacement(7, 7, 'P', true, false));
    bingo_move.addPlacement(TilePlacement(7, 8, 'L', true, false));
    bingo_move.addPlacement(TilePlacement(7, 9, 'A', true, false));
    bingo_move.addPlacement(TilePlacement(7, 10, 'Y', true, false));
    bingo_move.addPlacement(TilePlacement(7, 11, 'I', true, false));
    bingo_move.addPlacement(TilePlacement(7, 12, 'N', true, false));
    bingo_move.addPlacement(TilePlacement(7, 13, 'G', true, false));
    bingo_move.setScore(100);

    state.applyMove(bingo_move);

    assert_equal(100, state.getTotalScore(), "Score should be 100");
    assert_equal(1, state.getBingoCount(), "Should have 1 bingo");
}

void test_game_state_is_game_over() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Game Over Conditions ===" << color::RESET << endl;

    GameState state(789);

    // At start, game is not over
    assert_false(state.isGameOver(), "Game should not be over at start");

    // Draw all tiles from bag
    while (!state.getTileBag().isEmpty()) {
        state.getTileBag().drawTile();
    }

    // Still not over because rack might have both vowels and consonants
    // Let's check the actual state
    state.refillRack();  // This won't draw anything since bag is empty

    // Game over depends on whether we have vowels AND consonants
    // For this test, we'll verify the logic works
    bool has_vowels = state.getTileBag().vowelCount() > 0 || state.getRack().size() > 0;
    bool has_consonants = state.getTileBag().consonantCount() > 0 || state.getRack().size() > 0;

    // If we have neither vowels nor consonants (or missing one), game should be over
    // This is implementation-dependent, so we just verify the method runs
    assert_true(state.isGameOver(), "Game should be over");
}

void test_game_state_reset() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Reset ===" << color::RESET << endl;

    GameState state(555);
    state.refillRack();

    // Make some moves
    Move move(7, 7, Direction::HORIZONTAL, "CAT");
    move.addPlacement(TilePlacement(7, 7, 'C', true, false));
    move.addPlacement(TilePlacement(7, 8, 'A', true, false));
    move.addPlacement(TilePlacement(7, 9, 'T', true, false));
    move.setScore(20);
    state.applyMove(move);

    assert_equal(20, state.getTotalScore(), "Score should be 20 before reset");
    assert_equal(1, state.getMoveCount(), "Should have 1 move before reset");

    state.reset();

    assert_equal(0, state.getTotalScore(), "Score should be 0 after reset");
    assert_equal(0, state.getMoveCount(), "Should have 0 moves after reset");
    assert_equal(0, state.getBingoCount(), "Should have 0 bingos after reset");
    assert_true(state.getBoard().isBoardEmpty(), "Board should be empty after reset");
    assert_equal(0, state.getRack().size(), "Rack should be empty after reset");
    assert_equal(102, state.getTileBag().remainingCount(), "Tile bag should have 102 tiles after reset");
}

void test_game_state_move_history() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: GameState Move History ===" << color::RESET << endl;

    GameState state(321);
    state.refillRack();

    Move move1(7, 7, Direction::HORIZONTAL, "CAT");
    move1.setScore(10);
    state.applyMove(move1);

    Move move2(8, 7, Direction::VERTICAL, "DOG");
    move2.setScore(15);
    state.applyMove(move2);

    const auto& history = state.getMoveHistory();
    assert_equal(2, (int)history.size(), "Should have 2 moves in history");
    assert_equal(10, history[0].getScore(), "First move score should be 10");
    assert_equal(15, history[1].getScore(), "Second move score should be 15");
    assert_equal(25, state.getTotalScore(), "Total score should be 25");
}

void test_rack_validity_before_move_15() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: Rack Validity Before Move 15 ===" << color::RESET << endl;

    // Test valid racks (before move 15)
    Rack valid_rack1("AABBCCD");
    assert_true(valid_rack1.isValid(0), "Rack with 2+ vowels and 2+ consonants should be valid");

    Rack valid_rack2("EEIIRRR");
    assert_true(valid_rack2.isValid(10), "Rack with 2+ vowels and 2+ consonants should be valid");

    // Test with blanks (they count as both)
    Rack valid_rack3("A?BBBBB");
    assert_true(valid_rack3.isValid(5), "Rack with blank counting as vowel should be valid");

    Rack valid_rack4("AA?CCCC");
    assert_true(valid_rack4.isValid(14), "Rack with blank should be valid");

    // Test invalid racks (before move 15)
    Rack invalid_rack1("ABBBBBB");
    assert_false(invalid_rack1.isValid(0), "Rack with only 1 vowel should be invalid before move 15");

    Rack invalid_rack2("AAAAAAB");
    assert_false(invalid_rack2.isValid(10), "Rack with only 1 consonant should be invalid before move 15");

    Rack invalid_rack3("AAAAAAA");
    assert_false(invalid_rack3.isValid(5), "Rack with no consonants should be invalid");

    Rack invalid_rack4("BBBBBBB");
    assert_false(invalid_rack4.isValid(14), "Rack with no vowels should be invalid");
}

void test_rack_validity_after_move_15() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: Rack Validity After Move 15 ===" << color::RESET << endl;

    // Test valid racks (after move 15)
    Rack valid_rack1("ABBBBBB");
    assert_true(valid_rack1.isValid(16), "Rack with 1+ vowels and 1+ consonants should be valid after move 15");

    Rack valid_rack2("AAAAAAB");
    assert_true(valid_rack2.isValid(20), "Rack with 1+ vowels and 1+ consonants should be valid after move 15");

    Rack valid_rack3("AB");
    assert_true(valid_rack3.isValid(50), "Rack with 1 vowel and 1 consonant should be valid");

    // Test with blanks
    Rack valid_rack4("?BBBBBB");
    assert_true(valid_rack4.isValid(16), "Rack with blank should be valid");

    // Test invalid racks (after move 15)
    Rack invalid_rack1("AAAAAAA");
    assert_false(invalid_rack1.isValid(16), "Rack with no consonants should be invalid");

    Rack invalid_rack2("BBBBBBB");
    assert_false(invalid_rack2.isValid(20), "Rack with no vowels should be invalid");
}

void test_refill_rack_handles_invalid_racks() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: Refill Rack Handles Invalid Racks ===" << color::RESET << endl;

    // Use a specific seed to test invalid rack handling
    GameState state(12345);
    state.refillRack();

    // Rack should be valid after refill (before move 15)
    assert_true(state.getRack().isValid(0), "Initial rack should be valid");

    // Simulate several moves
    for (int i = 0; i < 5; i++) {
        Move move(7 + i, 7, Direction::HORIZONTAL, "A");
        move.setScore(1);
        state.applyMove(move);

        // Remove a tile and refill
        state.getRack().removeTile(state.getRack().getTile(0));
        state.refillRack();

        // Rack should still be valid after each refill
        int move_count = state.getMoveCount();
        assert_true(state.getRack().isValid(move_count), "Rack should be valid after refill");
    }
}

int main() {
    cout << "=== GameState Tests ===" << endl;

    test_game_state_initialization();
    test_game_state_refill_rack();
    test_game_state_apply_move();
    test_game_state_bingo_detection();
    test_game_state_is_game_over();
    test_game_state_reset();
    test_game_state_move_history();
    test_rack_validity_before_move_15();
    test_rack_validity_after_move_15();
    test_refill_rack_handles_invalid_racks();

    print_summary();
    return exit_code();
}
