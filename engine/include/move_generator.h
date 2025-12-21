#ifndef SCRADLE_MOVE_GENERATOR_H
#define SCRADLE_MOVE_GENERATOR_H

#include "board.h"
#include "rack.h"
#include "dawg.h"
#include "move.h"
#include <vector>
#include <set>
#include <memory>

namespace scradle {

// Represents a starting position for word placement
struct StartPosition {
    int row;
    int col;
    Direction direction;
    int min_extension;  // Minimum tiles needed to connect to existing tiles
    int max_extension;  // Maximum tiles that can be placed before hitting board edge

    StartPosition(int r, int c, Direction dir, int min_ext, int max_ext)
        : row(r), col(c), direction(dir), min_extension(min_ext), max_extension(max_ext) {}
};

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

private:
    const Board& board_;
    const Rack& rack_;
    const DAWG& dawg_;

    // Step 2: Generate all possible raw moves
    std::vector<RawMove> generateAllRawMoves(const std::vector<StartPosition>& positions) const;

    // Helper: Generate permutations of rack tiles
    void generatePermutations(
        const std::string& tiles,
        int min_length,
        int max_length,
        std::vector<std::string>& result
    ) const;

    // Helper: Recursive helper for generatePermutations
    void generatePermutationsHelper(
        const std::string& tiles,
        std::vector<bool>& used,
        int remaining,
        std::string& current,
        std::vector<std::string>& result
    ) const;

    // Helper: Generate raw move for a specific tile sequence and start position
    RawMove createRawMove(
        const std::string& tile_sequence,
        const StartPosition& pos
    ) const;

    // Step 3: Validate moves
    std::vector<Move> filterValidMoves(const std::vector<RawMove>& raw_moves) const;

    // Helper: Check if a raw move forms valid words
    bool isValidMove(const RawMove& raw_move, std::string& main_word) const;

    // Helper: Get the complete main word from a raw move
    std::string getMainWord(const RawMove& raw_move) const;

    // Helper: Get all cross-words formed by new tiles
    std::vector<std::string> getCrossWords(const RawMove& raw_move) const;

    // Helper: Convert RawMove to Move
    Move rawMoveToMove(const RawMove& raw_move, const std::string& word) const;

    // Utility functions
    void getNext(int& row, int& col, Direction dir) const;
    void getPrev(int& row, int& col, Direction dir) const;
};

} // namespace scradle

#endif // SCRADLE_MOVE_GENERATOR_H
