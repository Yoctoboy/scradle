#include "board.h"

#include <iomanip>
#include <sstream>

using std::cout;
using std::endl;
using std::setw;
using std::string;
using std::stringstream;

namespace scradle {

Board::Board() {
    initializePremiumSquares();
}

Cell& Board::getCell(int row, int col) {
    return cells_[getIndex(row, col)];
}

const Cell& Board::getCell(int row, int col) const {
    return cells_[getIndex(row, col)];
}

char Board::getLetter(int row, int col) const {
    return getCell(row, col).letter;
}

void Board::setLetter(int row, int col, char letter) {
    cells_[getIndex(row, col)].letter = letter;
}

bool Board::isEmpty(int row, int col) const {
    return getCell(row, col).isEmpty();
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool Board::isCenterOccupied() const {
    return !isEmpty(CENTER, CENTER);
}

bool Board::isBoardEmpty() const {
    for (const auto& cell : cells_) {
        if (!cell.isEmpty()) {
            return false;
        }
    }
    return true;
}

bool Board::isAnchor(int row, int col) const {
    bool isAnchor = false;
    if (row > 0) isAnchor |= !isEmpty(row - 1, col);
    if (row < 14) isAnchor |= !isEmpty(row + 1, col);
    if (col > 0) isAnchor |= !isEmpty(row, col - 1);
    if (col < 14) isAnchor |= !isEmpty(row, col + 1);
    return isAnchor;
}

void Board::display() const {
    cout << toString() << endl;
}

string Board::toString() const {
    stringstream ss;

    // Column headers (A-O)
    ss << "    ";
    for (int col = 0; col < SIZE; ++col) {
        ss << " " << static_cast<char>('A' + col) << " ";
    }
    ss << "\n";

    // Top border
    ss << "   +";
    for (int col = 0; col < SIZE; ++col) {
        ss << "--+";
    }
    ss << "\n";

    // Board rows (1-15)
    for (int row = 0; row < SIZE; ++row) {
        ss << setw(2) << (row + 1) << " |";

        for (int col = 0; col < SIZE; ++col) {
            const Cell& cell = getCell(row, col);

            if (!cell.isEmpty()) {
                // Display letter
                ss << " " << cell.letter << "|";
            } else {
                // Display premium square symbol
                char symbol = ' ';
                switch (cell.premium) {
                    case PremiumType::TRIPLE_WORD:
                        symbol = '*';
                        break;
                    case PremiumType::DOUBLE_WORD:
                        symbol = '=';
                        break;
                    case PremiumType::TRIPLE_LETTER:
                        symbol = '^';
                        break;
                    case PremiumType::DOUBLE_LETTER:
                        symbol = '"';
                        break;
                    case PremiumType::NONE:
                        symbol = ' ';
                        break;
                }
                ss << " " << symbol << "|";
            }
        }

        ss << "\n   +";
        for (int col = 0; col < SIZE; ++col) {
            ss << "--+";
        }
        ss << "\n";
    }

    ss << "\nLegend: * = TW (Triple Word), = = DW (Double Word)\n";
    ss << "        ^ = TL (Triple Letter), \" = DL (Double Letter)\n";

    return ss.str();
}

Board Board::parseBoard(const std::string& ascii) {
    Board board;

    int row = 0;
    int col = 0;

    for (size_t i = 0; i < ascii.length(); ++i) {
        char c = ascii[i];

        // Skip whitespace at the beginning or end of lines
        if (c == '\n') {
            // Move to next row
            if (col > 0) {  // Only advance if we've read some columns
                row++;
                col = 0;
            }
            continue;
        }

        // Skip spaces/tabs at line start
        if (col == 0 && (c == ' ' || c == '\t')) {
            continue;
        }

        // Ensure we're within board bounds
        if (row >= SIZE) {
            break;
        }

        if (col < SIZE) {
            if (c == '.' || c == ' ') {
                // Empty cell - already initialized
                col++;
            } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                // Letter tile - preserve case (lowercase = blank)
                board.setLetter(row, col, c);
                col++;
            }
            // Ignore other characters
        }
    }

    return board;
}

void Board::initializePremiumSquares() {
    // Standard Scrabble premium square layout
    // Triple Word Score (TW)
    const int tw_positions[][2] = {
        {0, 0}, {0, 7}, {0, 14}, {7, 0}, {7, 14}, {14, 0}, {14, 7}, {14, 14}};

    for (const auto& pos : tw_positions) {
        getCell(pos[0], pos[1]).premium = PremiumType::TRIPLE_WORD;
    }

    // Double Word Score (DW)
    const int dw_positions[][2] = {
        {1, 1}, {2, 2}, {3, 3}, {4, 4}, {1, 13}, {2, 12}, {3, 11}, {4, 10}, {13, 1}, {12, 2}, {11, 3}, {10, 4}, {13, 13}, {12, 12}, {11, 11}, {10, 10}, {7, 7}  // Center square
    };

    for (const auto& pos : dw_positions) {
        getCell(pos[0], pos[1]).premium = PremiumType::DOUBLE_WORD;
    }

    // Triple Letter Score (TL)
    const int tl_positions[][2] = {
        {1, 5}, {1, 9}, {5, 1}, {5, 5}, {5, 9}, {5, 13}, {9, 1}, {9, 5}, {9, 9}, {9, 13}, {13, 5}, {13, 9}};

    for (const auto& pos : tl_positions) {
        getCell(pos[0], pos[1]).premium = PremiumType::TRIPLE_LETTER;
    }

    // Double Letter Score (DL)
    const int dl_positions[][2] = {
        {0, 3}, {0, 11}, {2, 6}, {2, 8}, {3, 0}, {3, 7}, {3, 14}, {6, 2}, {6, 6}, {6, 8}, {6, 12}, {7, 3}, {7, 11}, {8, 2}, {8, 6}, {8, 8}, {8, 12}, {11, 0}, {11, 7}, {11, 14}, {12, 6}, {12, 8}, {14, 3}, {14, 11}};

    for (const auto& pos : dl_positions) {
        getCell(pos[0], pos[1]).premium = PremiumType::DOUBLE_LETTER;
    }
}

}  // namespace scradle
