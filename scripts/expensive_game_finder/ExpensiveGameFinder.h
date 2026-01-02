#ifndef SCRADLE_EXPENSIVE_GAME_FINDER_H
#define SCRADLE_EXPENSIVE_GAME_FINDER_H

#include "../../engine/include/game_state.h"
#include "../../engine/include/move_generator.h"
#include "../../engine/include/dawg.h"
#include "../../engine/include/move.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <random>

namespace scradle {

/**
 * ExpensiveGameFinder attempts to find high-scoring Duplicate Scrabble games
 * by strategically choosing tile draws and selecting among equal-scoring moves.
 */
class ExpensiveGameFinder {
public:
    /**
     * WordPlacementInfo holds information about where a word would be placed
     */
    struct WordPlacementInfo {
        std::string word;
        int row;
        int col;
        Direction direction;
        bool already_placed;  // True if the word is already fully on the board

        WordPlacementInfo() : row(-1), col(-1), direction(Direction::HORIZONTAL), already_placed(false) {}
        WordPlacementInfo(const std::string& w, int r, int c, Direction d, bool placed)
            : word(w), row(r), col(c), direction(d), already_placed(placed) {}
    };

    /**
     * PlacementConfiguration holds the complete placement info for all 3 main words
     */
    struct PlacementConfiguration {
        WordPlacementInfo word1_info;
        WordPlacementInfo word2_info;
        WordPlacementInfo word3_info;
        bool is_valid;

        PlacementConfiguration() : is_valid(false) {}
    };

    /**
     * SubstringInfo holds information about a valid substring of a main word
     */
    struct SubstringInfo {
        std::string substring;      // The substring itself
        int start_position;         // Position in the main word where it starts (0-indexed)

        SubstringInfo(const std::string& sub, int pos)
            : substring(sub), start_position(pos) {}
    };

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
     * Also verifies that triple word squares are still available for words not yet placed
     * @param word1 First 15-letter word
     * @param word2 Second 15-letter word
     * @param word3 Third 15-letter word
     * @param board The current board state
     * @return PlacementConfiguration with is_valid=true if words can be placed, false otherwise
     */
    PlacementConfiguration canPlaceWordsOnGridWithTripleWords(const std::string& word1, const std::string& word2,
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
    std::string findPlayableMainWord(const std::string& word1, const std::string& word2,
                                   const std::string& word3);

    /**
     * Play a specific word by drawing the right tiles and placing it
     * @param word The word to play
     * @return true if the word was successfully played
     */
    bool playSpecificMainWord(const std::string& word);

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

    void checkKeyPressAndPrintBoard();

    /**
     * Find all valid substrings of a word that exist in the dictionary
     * @param word The main word to extract substrings from
     * @return Vector of SubstringInfo containing valid substrings and their positions
     */
    std::vector<SubstringInfo> findValidSubstrings(const std::string& word);

    /**
     * Try to place a substring by finding the best move that matches it
     * @param substring The substring to place
     * @param word_info Placement info for the main word containing this substring
     * @return true if a substring was successfully placed
     */
    bool tryPlaceSubstring(const std::string& substring,
                          const WordPlacementInfo& word_info);

    /**
     * Try to place any substring from pre-calculated lists
     * @param substrings1 Substrings for main_word1
     * @param substrings2 Substrings for main_word2
     * @param substrings3 Substrings for main_word3
     * @param main_word1 First main word
     * @param main_word2 Second main word
     * @param main_word3 Third main word
     * @return true if a substring was successfully placed
     */
    bool tryPlaceAnySubstring(const std::vector<SubstringInfo>& substrings1,
                             const std::vector<SubstringInfo>& substrings2,
                             const std::vector<SubstringInfo>& substrings3,
                             const std::string& main_word1,
                             const std::string& main_word2,
                             const std::string& main_word3);

    GameState game_state_;
    const DAWG& dawg_;
    std::mt19937 rng_;  // Random number generator initialized with seed
};

}  // namespace scradle

#endif  // SCRADLE_EXPENSIVE_GAME_FINDER_H
