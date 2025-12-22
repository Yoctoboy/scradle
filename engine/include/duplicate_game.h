#ifndef SCRADLE_DUPLICATE_GAME_H
#define SCRADLE_DUPLICATE_GAME_H

#include "dawg.h"
#include "game_state.h"
#include "move_generator.h"
#include "scorer.h"

namespace scradle {

// Orchestrates a complete Duplicate Scrabble game
// In Duplicate Scrabble, the player always plays the highest-scoring move
class DuplicateGame {
   public:
    // Constructor requires DAWG for move generation
    explicit DuplicateGame(const DAWG& dawg, unsigned int seed = 0);

    // Run a complete game from start to finish
    void playGame();

    // Step-by-step execution (for debugging/visualization)
    bool playNextMove();  // Returns false when game is over

    // Access current state
    const GameState& getState() const { return state_; }
    GameState& getState() { return state_; }

   private:
    const DAWG& dawg_;
    GameState state_;
    Scorer scorer_;

    // Find and play the best move from current state
    // Returns true if a move was played, false if no valid moves
    bool findAndPlayBestMove();

    // Check if game should terminate
    // Returns true if:
    // - Game is over (no vowels or no consonants in bag+rack)
    // - No valid moves available
    bool shouldTerminate() const;
};

}  // namespace scradle

#endif  // SCRADLE_DUPLICATE_GAME_H
