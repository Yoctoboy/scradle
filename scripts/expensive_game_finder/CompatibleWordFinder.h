#ifndef SCRADLE_COMPATIBLE_WORD_FINDER_H
#define SCRADLE_COMPATIBLE_WORD_FINDER_H

#include <string>
#include <vector>
#include <tuple>
#include <random>

#include "../../engine/include/dawg.h"

namespace scradle {

/**
 * CompatibleWordFinder is responsible for finding three mutually compatible
 * high-scoring 15-letter words that can be placed simultaneously on a Scrabble grid.
 */
class CompatibleWordFinder {
public:
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
     * Result holds the three compatible words and their valid substrings
     */
    struct Result {
        std::string word1;
        std::string word2;
        std::string word3;
        std::vector<SubstringInfo> substrings1;
        std::vector<SubstringInfo> substrings2;
        std::vector<SubstringInfo> substrings3;
        bool found;  // True if compatible words were found

        Result() : found(false) {}
    };

    /**
     * Constructor
     * @param dawg Reference to the dictionary DAWG for word validation
     * @param seed Random seed for reproducibility (used for shuffling words)
     */
    CompatibleWordFinder(const DAWG& dawg, unsigned int seed = 0);

    /**
     * Find three mutually compatible high-scoring 15-letter words
     * and compute their valid substrings
     * @return Result struct containing the words and their substrings
     */
    Result findCompatible15LetterWordsWithSubstrings();

private:
    /**
     * Check if three 15-letter words can be placed simultaneously on the board
     * @param word1 First 15-letter word
     * @param word2 Second 15-letter word
     * @param word3 Third 15-letter word
     * @return true if they can be placed without conflict
     */
    bool areWordsCompatible(const std::string& word1, const std::string& word2,
                           const std::string& word3);

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
     * Find all valid substrings of a word that exist in the dictionary
     * @param word The main word to extract substrings from
     * @return Vector of SubstringInfo containing valid substrings and their positions
     */
    std::vector<SubstringInfo> findValidSubstrings(const std::string& word);

    /**
     * Count the number of competitive substrings for a 15-letter word
     * A competitive substring is one that doesn't overlap triple word squares at positions 0, 7, 14
     * @param word The 15-letter word to analyze
     * @return Number of competitive substrings
     */
    int countCompetitiveSubstrings(const std::string& word);

    const DAWG& dawg_;
    std::mt19937 rng_;  // Random number generator initialized with seed
};

}  // namespace scradle

#endif  // SCRADLE_COMPATIBLE_WORD_FINDER_H
