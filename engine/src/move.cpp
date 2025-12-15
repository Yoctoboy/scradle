#include "move.h"
#include <sstream>

namespace scradle {

Move::Move()
    : start_row_(0), start_col_(0), direction_(Direction::HORIZONTAL),
      word_(""), score_(0) {}

Move::Move(int start_row, int start_col, Direction dir, const std::string& word)
    : start_row_(start_row), start_col_(start_col), direction_(dir),
      word_(word), score_(0) {}

void Move::addPlacement(const TilePlacement& placement) {
    placements_.push_back(placement);
}

bool Move::isValid() const {
    return !word_.empty() && !placements_.empty();
}

std::string Move::toString() const {
    std::stringstream ss;
    const char* row_labels = "ABCDEFGHIJKLMNO";

    ss << word_ << " at ";

    if (direction_ == Direction::HORIZONTAL) {
        // Horizontal: ROW+COL (e.g., H4)
        ss << row_labels[start_row_] << (start_col_ + 1);
    } else {
        // Vertical: COL+ROW (e.g., 4H)
        ss << (start_col_ + 1) << row_labels[start_row_];
    }

    // Also show internal coordinates for debugging
    ss << " (" << start_row_ << "," << start_col_ << " "
       << (direction_ == Direction::HORIZONTAL ? "H" : "V") << ")";

    ss << " [" << score_ << " pts]";
    return ss.str();
}

} // namespace scradle
