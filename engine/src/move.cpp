#include "move.h"

#include <sstream>

using std::string;

namespace scradle {

const char* row_labels = "ABCDEFGHIJKLMNO";

Move::Move()
    : start_row_(0), start_col_(0), direction_(Direction::HORIZONTAL), word_(""), score_(0) {}

Move::Move(int start_row, int start_col, Direction dir, const std::string& word)
    : start_row_(start_row), start_col_(start_col), direction_(dir), word_(word), score_(0) {}

void Move::addPlacement(const TilePlacement& placement) {
    placements_.push_back(placement);
}

bool Move::isValid() const {
    return !word_.empty() && !placements_.empty();
}

bool Move::isBingo() const {
    // Count tiles placed from rack
    int tiles_from_rack = 0;
    for (const auto& placement : getPlacements()) {
        if (placement.is_from_rack) {
            tiles_from_rack++;
        }
    }
    return tiles_from_rack == 7;
}

std::string Move::toString() const {
    std::stringstream ss;

    // Print the whole word, accounting for blank tiles
    for (size_t i = 0; i < word_.length(); i++) {
        // Find if this position has a blank tile
        bool is_blank = false;
        int row = (direction_ == Direction::HORIZONTAL) ? start_row_ : start_row_ + i;
        int col = (direction_ == Direction::HORIZONTAL) ? start_col_ + i : start_col_;

        for (const auto& placement : placements_) {
            if (placement.row == row && placement.col == col && placement.is_blank) {
                is_blank = true;
                break;
            }
        }

        ss << (is_blank ? char(std::tolower(static_cast<unsigned char>(word_[i]))) : word_[i]);
    }
    ss << " at ";

    if (direction_ == Direction::HORIZONTAL) {
        // Horizontal: ROW+COL (e.g., H4)
        ss << row_labels[start_row_] << (start_col_ + 1);
    } else {
        // Vertical: COL+ROW (e.g., 4H)
        ss << (start_col_ + 1) << row_labels[start_row_];
    }

    ss << " [" << score_ << " pts]";

    if (isBingo()) {
        ss << " [BINGO]";
    }
    return ss.str();
}

}  // namespace scradle
