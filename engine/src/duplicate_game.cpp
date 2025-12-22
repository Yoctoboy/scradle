#include "duplicate_game.h"

#include <algorithm>
#include <iostream>

namespace scradle {

DuplicateGame::DuplicateGame(const DAWG& dawg, unsigned int seed)
    : dawg_(dawg), state_(seed), scorer_() {}

void DuplicateGame::playGame() {
    // Initialize game
    state_.reset();
    state_.refillRack();

    // Main game loop
    while (!shouldTerminate()) {
        bool success = findAndPlayBestMove();

        if (!success) {
            // No valid moves available
            break;
        }

        // Refill rack after playing move
        state_.refillRack();
    }

    // Output summary
    state_.printSummary();
}

bool DuplicateGame::playNextMove() {
    if (shouldTerminate()) {
        return false;
    }

    bool success = findAndPlayBestMove();

    if (success) {
        state_.refillRack();
    }

    return success;
}

bool DuplicateGame::findAndPlayBestMove() {
    // Generate and get best move (already scored)
    MoveGenerator move_gen(state_.getBoard(), state_.getRack(), dawg_);
    std::vector<Move> best_moves = move_gen.getBestMove();

    if (best_moves.empty()) {
        return false;
    }

    // Take the first best move (if there are ties, we just take the first one)
    state_.applyMove(best_moves[0]);
    return true;
}

bool DuplicateGame::shouldTerminate() const {
    // Game should terminate if:
    // 1. No vowels OR no consonants remaining (checked by isGameOver)
    // 2. Rack is empty (can't make any moves)
    return state_.isGameOver() || state_.getRack().size() == 0;
}

}  // namespace scradle
