#ifndef SCRADLE_MOVE_GENERATOR_H
#define SCRADLE_MOVE_GENERATOR_H

#include <memory>
#include <set>
#include <vector>

#include "board.h"
#include "dawg.h"
#include "move.h"
#include "rack.h"

namespace scradle {

// Represents a raw move before validation
struct RawMove {
    std::vector<TilePlacement> placements;
    Direction direction;
    int start_row;
    int start_col;
};

// Generates all valid moves for a given board state and rack
class MoveGenerator {
   public:
    MoveGenerator(const Board& board, const Rack& rack, const DAWG& dawg);

    // Generate all valid moves
    std::vector<Move> generateMoves();

    // Step 1: Find all start positions (exposed for testing)
    std::vector<StartPosition> findStartPositions() const;

    // Step 2: Generate all possible raw moves (exposed for testing)
    std::vector<RawMove> generateAllRawMoves(const std::vector<StartPosition>& positions) const;

    // Step 3 helpers (exposed for testing)
    std::string getMainWord(const RawMove& raw_move) const;
    std::vector<std::string> getCrossWords(const RawMove& raw_move) const;
    bool isValidMove(const RawMove& raw_move) const;

    // Get best moves (all moves with the highest score)
    std::vector<Move> getBestMove();

    // Get top X moves sorted by score (descending)
    std::vector<Move> getTopMoves(int count);

   private:
    const Board& board_;
    const Rack& rack_;
    const DAWG& dawg_;

    // Helper: Generate permutations of rack tiles
    void generatePermutations(
        const std::string& tiles,
        int min_length,
        int max_length,
        std::vector<std::string>& result) const;

    // Helper: Recursive helper for generatePermutations
    void generatePermutationsHelper(
        const std::string& tiles,
        std::vector<bool>& used,
        int remaining,
        std::string& current,
        std::vector<std::string>& result) const;

    // Helper: Expand blank tiles ('?') to all possible letters
    void expandBlanks(
        const std::string& permutation,
        size_t index,
        std::string current,
        std::vector<std::string>& result) const;

    // Helper: Generate raw move for a specific tile sequence and start position
    RawMove createRawMove(
        const std::string& tile_sequence,
        const StartPosition& pos) const;

    // Step 3: Validate moves
    std::vector<Move> filterValidMoves(const std::vector<RawMove>& raw_moves) const;

    // Helper: Convert RawMove to Move
    Move rawMoveToMove(const RawMove& raw_move, const std::string& word) const;

    // Utility functions
    void getNext(int& row, int& col, Direction dir) const;
    void getPrev(int& row, int& col, Direction dir) const;
};

}  // namespace scradle

#endif  // SCRADLE_MOVE_GENERATOR_H
