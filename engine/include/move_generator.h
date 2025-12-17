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

// Represents an anchor square (a valid starting position for a word)
struct Anchor {
    int row;
    int col;
    int max_left_extension;  // How many tiles can extend left (horizontal) or up (vertical)
    std::set<char> cross_checks;  // Letters that form valid cross-words

    Anchor(int r, int c) : row(r), col(c), max_left_extension(0) {}
};

// Generates all valid moves for a given board state and rack
class MoveGenerator {
public:
    MoveGenerator(const Board& board, const Rack& rack, const DAWG& dawg);

    // Generate all valid moves
    std::vector<Move> generateMoves();

    // Generate moves in a specific direction
    std::vector<Move> generateMovesHorizontal();
    std::vector<Move> generateMovesVertical();

private:
    const Board& board_;
    const Rack& rack_;
    const DAWG& dawg_;

    // Anchor identification
    std::vector<Anchor> findAnchors(Direction dir);
    bool isAnchor(int row, int col) const;

    // Cross-check computation
    std::set<char> computeCrossChecks(int row, int col, Direction dir);

    // Helper to get cross-word (perpendicular to main direction)
    std::string getCrossWord(int row, int col, Direction main_dir);

    // Move generation via DAWG traversal
    void extendRight(
        const std::shared_ptr<DAWG::Node>& node,
        const std::string& partial_word,
        int row, int col,
        Direction dir,
        Rack temp_rack,
        std::vector<Move>& moves,
        bool anchor_placed,
        std::vector<int> blank_positions = std::vector<int>()
    );

    void extendRightWithAnchor(
        const std::shared_ptr<DAWG::Node>& node,
        const std::string& partial_word,
        int row, int col,
        Direction dir,
        Rack temp_rack,
        std::vector<Move>& moves,
        bool anchor_placed,
        int anchor_row, int anchor_col,
        std::vector<int> blank_positions
    );

    void extendLeft(
        const Anchor& anchor,
        Direction dir,
        std::vector<Move>& moves
    );

    // Get next position in the given direction
    void getNext(int& row, int& col, Direction dir) const;
    void getPrev(int& row, int& col, Direction dir) const;
};

} // namespace scradle

#endif // SCRADLE_MOVE_GENERATOR_H
