#ifndef SCRADLE_EXPENSIVE_GAME_FINDER_H
#define SCRADLE_EXPENSIVE_GAME_FINDER_H

#include "../../engine/include/game_state.h"
#include "../../engine/include/move_generator.h"
#include "../../engine/include/dawg.h"
#include "../../engine/include/move.h"
#include <vector>
#include <string>

namespace scradle {

/**
 * ExpensiveGameFinder attempts to find high-scoring Duplicate Scrabble games
 * by strategically choosing tile draws and selecting among equal-scoring moves.
 */
class ExpensiveGameFinder {
public:
    /**
     * Constructor
     * @param dawg Reference to the dictionary DAWG for word validation
     * @param seed Random seed for reproducibility
     */
    ExpensiveGameFinder(const DAWG& dawg, unsigned int seed = 0);

    /**
     * Main entry point to find an expensive game
     * @return The final score of the expensive game found
     */
    int findExpensiveGame();

    /**
     * Get the current game state
     */
    const GameState& getGameState() const { return game_state_; }

    /**
     * Get the DAWG reference
     */
    const DAWG& getDAWG() const { return dawg_; }

private:
    /**
     * Find three mutually compatible high-scoring 15-letter words
     * These words should be placeable simultaneously on the grid
     * @return A tuple of 3 15-letter words that are compatible
     */
    std::tuple<std::string, std::string, std::string> findCompatible15LetterWords();

    /**
     * Check if three 15-letter words can be placed simultaneously on the board
     * @param word1 First 15-letter word
     * @param word2 Second 15-letter word
     * @param word3 Third 15-letter word
     * @return true if they can be placed without conflict
     */
    bool areWordsCompatible(const std::string& word1, const std::string& word2, const std::string& word3);

    /**
     * Load all 15-letter words from the dictionary
     * @return Vector of all valid 15-letter words
     */
    std::vector<std::string> load15LetterWords();

    /**
     * Score a 15-letter word placed horizontally on the first row
     * @param word The word to score
     * @return The score of the word
     */
    int score15LetterWord(const std::string& word);

    /**
     * Check if three words can be placed on the grid in specific positions
     * Tries both orientations: vertical (columns 0,7,14) and horizontal (rows 0,7,14)
     * @param word1 First 15-letter word
     * @param word2 Second 15-letter word
     * @param word3 Third 15-letter word
     * @param board The current board state
     * @return true if the words can be placed in either orientation
     */
    bool canPlaceWordsOnGrid(const std::string& word1, const std::string& word2,
                             const std::string& word3, const Board& board);

    /**
     * Create a RawMove for placing a 15-letter word at a specific position
     * @param word The word to place
     * @param board The current board state
     * @param row Starting row
     * @param col Starting column
     * @param direction Direction (HORIZONTAL or VERTICAL)
     * @return RawMove representing the placement
     */
    RawMove createRawMoveForWord(const std::string& word, const Board& board,
                                  int row, int col, Direction direction);

    /**
     * Check if any of the 3 words can be played in a single move (≤7 tiles from rack)
     * @param word1 First word
     * @param word2 Second word
     * @param word3 Third word
     * @return The word that can be played, or empty string if none can be played
     */
    std::string findPlayableWord(const std::string& word1, const std::string& word2,
                                   const std::string& word3);

    /**
     * Play a specific word by drawing the right tiles and placing it
     * @param word The word to play
     * @return true if the word was successfully played
     */
    bool playSpecificWord(const std::string& word);

    /**
     * Count how many tiles from rack are needed to place a word at a specific position
     * @param word The word to place
     * @param board The current board state
     * @param row Starting row
     * @param col Starting column
     * @param direction Direction (HORIZONTAL or VERTICAL)
     * @return Number of tiles needed from rack, or -1 if placement is impossible
     */
    int countNeededTiles(const std::string& word, const Board& board,
                         int row, int col, Direction direction);

    /**
     * Calculate the total number of tiles needed to place all 3 main words
     * @param word1 First main word
     * @param word2 Second main word
     * @param word3 Third main word
     * @param board The current board state
     * @return Total sum of tiles needed for all 3 words, or -1 if impossible to place all
     */
    int calculateTotalNeededTiles(const std::string& word1, const std::string& word2,
                                    const std::string& word3, const Board& board);

    GameState game_state_;
    const DAWG& dawg_;
};

}  // namespace scradle

#endif  // SCRADLE_EXPENSIVE_GAME_FINDER_H
