#include "duplicate_game.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace scradle {

DuplicateGame::DuplicateGame(const DAWG& dawg, unsigned int seed)
    : dawg_(dawg), state_(seed), scorer_(), rng_(seed) {}

void DuplicateGame::playGame(bool display) {
    // Initialize game
    state_.reset();
    state_.refillRack();

    // Main game loop
    while (!shouldTerminate()) {
        bool success = findAndPlayBestMove(display);

        if (!success) {
            // No valid moves available
            break;
        }

        // Refill rack after playing move
        state_.refillRack();
    }

    // Output summary
    if (display) state_.printSummary();
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

bool DuplicateGame::findAndPlayBestMove(bool display) {
    // Generate and get best move (already scored)
    MoveGenerator move_gen(state_.getBoard(), state_.getRack(), dawg_);
    if (display) {
        std::cout << "Move " << state_.getMoveCount() + 1 << ": rack=" << state_.getRack().toString();
    }
    std::vector<Move> best_moves = move_gen.getBestMove();

    if (best_moves.empty()) {
        return false;
    }

    // Filter moves based on move number
    std::vector<Move> candidates;
    if (state_.getMoveCount() == 0) {
        // For the first move, prefer horizontal moves
        for (const auto& move : best_moves) {
            if (move.getDirection() == Direction::HORIZONTAL) {
                candidates.push_back(move);
            }
        }
        // If no horizontal moves, use all moves
        if (candidates.empty()) {
            candidates = best_moves;
        }
    } else {
        candidates = best_moves;
    }

    // Randomly select from candidates
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    Move selected_move = candidates[dist(rng_)];

    state_.applyMove(selected_move);
    if (display) std::cout << " -- move: " << selected_move.toString() << std::endl;
    return true;
}

bool DuplicateGame::shouldTerminate() const {
    // Game should terminate if:
    // 1. No vowels OR no consonants remaining (checked by isGameOver)
    // 2. Rack is empty (can't make any moves)
    return state_.isGameOver() || state_.getRack().size() == 0;
}

}  // namespace scradle
