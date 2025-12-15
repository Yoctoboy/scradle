#ifndef SCRADLE_CELL_H
#define SCRADLE_CELL_H

#include <ostream>

namespace scradle {

// Premium square types
enum class PremiumType {
    NONE,           // Normal square
    DOUBLE_LETTER,  // DL
    TRIPLE_LETTER,  // TL
    DOUBLE_WORD,    // DW
    TRIPLE_WORD     // TW
};

// Stream operator for PremiumType (for testing and debugging)
inline std::ostream& operator<<(std::ostream& os, PremiumType type) {
    switch (type) {
        case PremiumType::NONE:          return os << "NONE";
        case PremiumType::DOUBLE_LETTER: return os << "DL";
        case PremiumType::TRIPLE_LETTER: return os << "TL";
        case PremiumType::DOUBLE_WORD:   return os << "DW";
        case PremiumType::TRIPLE_WORD:   return os << "TW";
        default:                         return os << "UNKNOWN";
    }
}

// A single cell on the board
struct Cell {
    char letter;              // 'A'-'Z' (or 'a'-'z' for blank tiles) or ' ' for empty
    PremiumType premium;      // Premium square type

    Cell() : letter(' '), premium(PremiumType::NONE) {}
    Cell(char l, PremiumType p) : letter(l), premium(p) {}

    bool isEmpty() const { return letter == ' '; }
};

} // namespace scradle

#endif // SCRADLE_CELL_H
