#ifndef SCRADLE_BOARD_H
#define SCRADLE_BOARD_H

#include <array>
#include <iostream>
#include <string>

#include "cell.h"

namespace scradle {

// 15x15 Scrabble board
class Board {
   public:
    static constexpr int SIZE = 15;
    static constexpr int CENTER = 7;

    Board();

    // Board access
    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;

    char getLetter(int row, int col) const;
    void setLetter(int row, int col, char letter);

    bool isEmpty(int row, int col) const;
    bool isAnchor(int row, int col) const;
    bool isValidPosition(int row, int col) const;

    // Board state
    bool isCenterOccupied() const;
    bool isBoardEmpty() const;

    // Display
    void display() const;
    std::string toString() const;

    // Parse board from ASCII representation
    // Format: 15 lines of 15 characters each, '.' for empty, letter for tile
    // Example:
    //   "....CAT........"
    //   ".....R........."
    //   ".....M........."
    static Board parseBoard(const std::string& ascii);

   private:
    std::array<Cell, SIZE * SIZE> cells_;

    // Helper to get flat index
    int getIndex(int row, int col) const { return row * SIZE + col; }

    // Initialize premium squares
    void initializePremiumSquares();
};

}  // namespace scradle

#endif  // SCRADLE_BOARD_H
