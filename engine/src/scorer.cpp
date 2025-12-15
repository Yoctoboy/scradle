#include "scorer.h"
#include <cctype>

namespace scradle {

Scorer::Scorer() {
    initializeLetterValues();
}

void Scorer::initializeLetterValues() {
    // French Scrabble letter values
    // 0 points: blank tiles (represented as '?')
    letter_values_['?'] = 0;

    // 1 point
    letter_values_['E'] = 1;
    letter_values_['A'] = 1;
    letter_values_['I'] = 1;
    letter_values_['N'] = 1;
    letter_values_['O'] = 1;
    letter_values_['R'] = 1;
    letter_values_['S'] = 1;
    letter_values_['T'] = 1;
    letter_values_['U'] = 1;
    letter_values_['L'] = 1;

    // 2 points
    letter_values_['D'] = 2;
    letter_values_['M'] = 2;
    letter_values_['G'] = 2;

    // 3 points
    letter_values_['B'] = 3;
    letter_values_['C'] = 3;
    letter_values_['P'] = 3;

    // 4 points
    letter_values_['F'] = 4;
    letter_values_['H'] = 4;
    letter_values_['V'] = 4;

    // 8 points
    letter_values_['J'] = 8;
    letter_values_['Q'] = 8;

    // 10 points
    letter_values_['K'] = 10;
    letter_values_['W'] = 10;
    letter_values_['X'] = 10;
    letter_values_['Y'] = 10;
    letter_values_['Z'] = 10;
}

int Scorer::getLetterValue(char letter) const {
    char upper = std::toupper(static_cast<unsigned char>(letter));
    auto it = letter_values_.find(upper);
    if (it != letter_values_.end()) {
        return it->second;
    }
    return 0;  // Unknown letters have 0 value
}

int Scorer::scoreMove(const Board& board, const Move& move) const {
    int total_score = 0;

    // Score the main word
    total_score += scoreMainWord(board, move);

    // Score all cross-words
    total_score += scoreCrossWords(board, move);

    // Add bingo bonus if all 7 tiles were used
    if (isBingo(move)) {
        total_score += BINGO_BONUS;
    }

    return total_score;
}

int Scorer::scoreMainWord(const Board& board, const Move& move) const {
    int word_score = 0;
    int word_multiplier = 1;

    int row = move.getStartRow();
    int col = move.getStartCol();
    Direction dir = move.getDirection();

    // Iterate through each letter in the word
    for (size_t i = 0; i < move.getWord().length(); ++i) {
        char letter = move.getWord()[i];

        // Check if this is a new tile (from rack) and if it's a blank
        bool is_new_tile = false;
        bool is_blank = false;
        for (const auto& placement : move.getPlacements()) {
            if (placement.row == row && placement.col == col) {
                if (placement.is_from_rack) {
                    is_new_tile = true;
                }
                is_blank = placement.is_blank;
                break;
            }
        }

        // Calculate letter value: 0 for blanks, normal value otherwise
        int letter_value = is_blank ? 0 : getLetterValue(letter);

        // Apply premium squares only for new tiles
        if (is_new_tile) {
            const Cell& cell = board.getCell(row, col);

            switch (cell.premium) {
                case PremiumType::DOUBLE_LETTER:
                    letter_value *= 2;
                    break;
                case PremiumType::TRIPLE_LETTER:
                    letter_value *= 3;
                    break;
                case PremiumType::DOUBLE_WORD:
                    word_multiplier *= 2;
                    break;
                case PremiumType::TRIPLE_WORD:
                    word_multiplier *= 3;
                    break;
                case PremiumType::NONE:
                    break;
            }
        }

        word_score += letter_value;

        // Move to next position
        if (dir == Direction::HORIZONTAL) {
            col++;
        } else {
            row++;
        }
    }

    return word_score * word_multiplier;
}

int Scorer::scoreCrossWords(const Board& board, const Move& move) const {
    int cross_score = 0;

    // For each tile placed from the rack, check if it forms a cross-word
    for (const auto& placement : move.getPlacements()) {
        if (!placement.is_from_rack) {
            continue;  // Only new tiles can form cross-words
        }

        int row = placement.row;
        int col = placement.col;
        Direction main_dir = move.getDirection();

        // Determine the perpendicular direction
        Direction cross_dir = (main_dir == Direction::HORIZONTAL)
                            ? Direction::VERTICAL
                            : Direction::HORIZONTAL;

        // Find the start and end of the cross-word
        int start_row = row, start_col = col;
        int end_row = row, end_col = col;

        if (cross_dir == Direction::VERTICAL) {
            // Look up
            while (start_row > 0 && !board.isEmpty(start_row - 1, col)) {
                start_row--;
            }
            // Look down
            while (end_row < Board::SIZE - 1 && !board.isEmpty(end_row + 1, col)) {
                end_row++;
            }
        } else {
            // Look left
            while (start_col > 0 && !board.isEmpty(row, start_col - 1)) {
                start_col--;
            }
            // Look right
            while (end_col < Board::SIZE - 1 && !board.isEmpty(row, end_col + 1)) {
                end_col++;
            }
        }

        // If there's a cross-word (not just a single letter)
        if (start_row != end_row || start_col != end_col) {
            int cross_word_score = 0;
            int cross_word_multiplier = 1;

            int r = start_row, c = start_col;
            while (r <= end_row && c <= end_col) {
                char letter = board.getLetter(r, c);
                bool is_blank = false;

                // If this is the newly placed tile, it might be empty on board
                if (r == row && c == col) {
                    letter = placement.letter;
                    is_blank = placement.is_blank;
                }

                // Calculate letter value: 0 for blanks, normal value otherwise
                int letter_value = is_blank ? 0 : getLetterValue(letter);

                // Apply premium only for the newly placed tile
                if (r == row && c == col) {
                    const Cell& cell = board.getCell(r, c);

                    switch (cell.premium) {
                        case PremiumType::DOUBLE_LETTER:
                            letter_value *= 2;
                            break;
                        case PremiumType::TRIPLE_LETTER:
                            letter_value *= 3;
                            break;
                        case PremiumType::DOUBLE_WORD:
                            cross_word_multiplier *= 2;
                            break;
                        case PremiumType::TRIPLE_WORD:
                            cross_word_multiplier *= 3;
                            break;
                        case PremiumType::NONE:
                            break;
                    }
                }

                cross_word_score += letter_value;

                if (cross_dir == Direction::VERTICAL) {
                    r++;
                } else {
                    c++;
                }
            }

            cross_score += cross_word_score * cross_word_multiplier;
        }
    }

    return cross_score;
}

bool Scorer::isBingo(const Move& move) const {
    // Count how many tiles from rack were placed
    int tiles_from_rack = 0;
    for (const auto& placement : move.getPlacements()) {
        if (placement.is_from_rack) {
            tiles_from_rack++;
        }
    }
    return tiles_from_rack == 7;
}

} // namespace scradle
