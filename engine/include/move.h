#ifndef SCRADLE_MOVE_H
#define SCRADLE_MOVE_H

#include <ostream>
#include <string>
#include <vector>

namespace scradle {

// Direction of play
enum class Direction {
    HORIZONTAL,
    VERTICAL
};

// Stream operator for Direction (for testing and debugging)
inline std::ostream& operator<<(std::ostream& os, Direction dir) {
    switch (dir) {
        case Direction::HORIZONTAL:
            return os << "HORIZONTAL";
        case Direction::VERTICAL:
            return os << "VERTICAL";
        default:
            return os << "UNKNOWN";
    }
}

// Represents a single tile placement in a move
struct TilePlacement {
    int row;
    int col;
    char letter;        // uppercase letter it represents
    bool is_from_rack;  // true if placed this turn, false if already on board
    bool is_blank;      // true if this is a blank tile (joker)

    TilePlacement(int r, int c, char l, bool from_rack = true, bool blank = false)
        : row(r), col(c), letter(l), is_from_rack(from_rack), is_blank(blank) {}
};

// Represents a complete Scrabble move
class Move {
   public:
    Move();
    Move(int start_row, int start_col, Direction dir, const std::string& word);

    // Getters
    int getStartRow() const { return start_row_; }
    int getStartCol() const { return start_col_; }
    Direction getDirection() const { return direction_; }
    std::string getWord() const { return word_; }
    const std::vector<TilePlacement>& getPlacements() const { return placements_; }
    int getScore() const { return score_; }

    // Setters
    void setScore(int score) { score_ = score; }
    void addPlacement(const TilePlacement& placement);

    // Utility
    bool isValid() const;
    std::string toString() const;
    bool isBingo() const;

   private:
    int start_row_;
    int start_col_;
    Direction direction_;
    std::string word_;
    std::vector<TilePlacement> placements_;
    int score_;
};

}  // namespace scradle

#endif  // SCRADLE_MOVE_H
