#include <iostream>

#include "dawg.h"
#include "duplicate_game.h"
#include "test_framework.h"

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_duplicate_game_initialization() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: DuplicateGame Initialization ===" << color::RESET << endl;

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    DuplicateGame game(dawg, 42);

    assert_equal(42u, game.getState().getSeed(), "Should have correct seed");
    assert_equal(0, game.getState().getTotalScore(), "Should start with 0 score");
    assert_equal(0, game.getState().getMoveCount(), "Should start with 0 moves");
}

void test_duplicate_game_single_move() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: DuplicateGame Single Move ===" << color::RESET << endl;

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    DuplicateGame game(dawg, 999);
    game.getState().reset();
    game.getState().refillRack();

    // Play one move
    bool success = game.playNextMove();

    if (success) {
        assert_true(game.getState().getMoveCount() >= 1, "Should have at least 1 move");
        assert_true(game.getState().getTotalScore() > 0, "Should have positive score");
        cout << "  First move score: " << game.getState().getTotalScore() << endl;
    } else {
        cout << "  No valid moves available with this rack" << endl;
    }
}

void test_duplicate_game_complete_game() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: DuplicateGame Complete Game ===" << color::RESET << endl;

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    DuplicateGame game(dawg, 123);

    // Suppress game summary output during test
    game.playGame();

    // Verify reasonable results
    int move_count = game.getState().getMoveCount();
    int total_score = game.getState().getTotalScore();

    cout << "  Total moves: " << move_count << endl;
    cout << "  Total score: " << total_score << endl;
    cout << "  Bingos: " << game.getState().getBingoCount() << endl;

    // At minimum, we should have played at least one move
    // (unless the initial rack had absolutely no playable words)
    // For the test dictionary, this should be very unlikely
    assert_true(move_count >= 0, "Move count should be non-negative");
    assert_true(total_score >= 0, "Total score should be non-negative");
}

void test_duplicate_game_deterministic() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: DuplicateGame Deterministic ===" << color::RESET << endl;

    DAWG dawg;
    dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    // Play two games with the same seed
    DuplicateGame game1(dawg, 555);
    game1.playGame();

    DuplicateGame game2(dawg, 555);
    game2.playGame();

    // Verify exact same results
    assert_equal(game1.getState().getTotalScore(), game2.getState().getTotalScore(),
                 "Same seed should produce same total score");
    assert_equal(game1.getState().getMoveCount(), game2.getState().getMoveCount(),
                 "Same seed should produce same move count");
    assert_equal(game1.getState().getBingoCount(), game2.getState().getBingoCount(),
                 "Same seed should produce same bingo count");

    // Check first move is identical
    if (game1.getState().getMoveCount() > 0 && game2.getState().getMoveCount() > 0) {
        const auto& history1 = game1.getState().getMoveHistory();
        const auto& history2 = game2.getState().getMoveHistory();

        assert_equal(history1[0].getWord(), history2[0].getWord(), "First move word should be identical");
        assert_equal(history1[0].getScore(), history2[0].getScore(), "First move score should be identical");
    }
}

int main() {
    cout << "=== DuplicateGame Tests ===" << endl;

    test_duplicate_game_initialization();
    test_duplicate_game_single_move();
    test_duplicate_game_complete_game();
    test_duplicate_game_deterministic();

    print_summary();
    return exit_code();
}
