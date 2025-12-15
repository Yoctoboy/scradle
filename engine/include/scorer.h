#ifndef SCRADLE_SCORER_H
#define SCRADLE_SCORER_H

#include "board.h"
#include "move.h"
#include <unordered_map>

namespace scradle {

// Handles scoring of Scrabble moves
class Scorer {
public:
    Scorer();

    // Score a complete move on the board
    int scoreMove(const Board& board, const Move& move) const;

    // Get the value of a single letter
    int getLetterValue(char letter) const;

    // Constants
    static constexpr int BINGO_BONUS = 50;  // Bonus for using all 7 tiles

private:
    std::unordered_map<char, int> letter_values_;

    // Initialize French Scrabble letter values
    void initializeLetterValues();

    // Calculate score for the main word
    int scoreMainWord(const Board& board, const Move& move) const;

    // Calculate scores for all cross-words formed
    int scoreCrossWords(const Board& board, const Move& move) const;

    // Helper to check if a move uses all 7 tiles
    bool isBingo(const Move& move) const;
};

} // namespace scradle

#endif // SCRADLE_SCORER_H
