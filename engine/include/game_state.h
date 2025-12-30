#ifndef SCRADLE_GAME_STATE_H
#define SCRADLE_GAME_STATE_H

#include <string>
#include <vector>

#include "board.h"
#include "dawg.h"
#include "move.h"
#include "rack.h"
#include "tile_bag.h"

namespace scradle {

// Represents the complete state of a Scrabble game at a point in time
class GameState {
   public:
    // Constructor with optional seed
    explicit GameState(unsigned int seed = 0);

    // Access game components
    Board& getBoard() { return board_; }
    const Board& getBoard() const { return board_; }

    Rack& getRack() { return rack_; }
    const Rack& getRack() const { return rack_; }

    TileBag& getTileBag() { return tile_bag_; }
    const TileBag& getTileBag() const { return tile_bag_; }

    // Apply a move and update state
    void applyMove(const Move& move);
    bool findAndPlayBestMove(const DAWG& dawg, bool display = false);

    // Undo the last move and restore previous state
    void undoLastMove();

    // Refill rack from tile bag (up to 7 tiles)
    // Checks for invalid racks and returns them to bag if necessary
    void refillRack();

    // Game status
    bool isGameOver() const;
    int getTotalScore() const { return total_score_; }
    int getMoveCount() const { return move_history_.size(); }
    int getBingoCount() const { return bingo_count_; }
    unsigned int getSeed() const { return seed_; }

    // Move history
    const std::vector<Move>& getMoveHistory() const { return move_history_; }

    // Reset to initial state
    void reset();

    // Output game summary
    void printSummary() const;
    std::string toString() const;

   private:
    Board board_;
    Rack rack_;
    TileBag tile_bag_;
    unsigned int seed_;

    int total_score_;
    int bingo_count_;
    std::vector<Move> move_history_;
};

}  // namespace scradle

#endif  // SCRADLE_GAME_STATE_H
