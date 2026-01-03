#include "CompatibleWordFinder.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#include "../../engine/include/board.h"
#include "../../engine/include/move.h"
#include "../../engine/include/scorer.h"
#include "../../engine/include/tile_bag.h"

namespace scradle {

CompatibleWordFinder::CompatibleWordFinder(const DAWG& dawg, unsigned int seed)
    : dawg_(dawg), rng_(seed) {}

CompatibleWordFinder::Result
CompatibleWordFinder::findCompatible15LetterWordsWithSubstrings() {
    Result result;

    // Load all 15-letter words
    std::vector<std::string> words = load15LetterWords();

    if (words.empty()) {
        return result;
    }

    std::cout << "Loaded " << words.size() << " 15-letter words" << std::endl;

    // Score all words and count competitive substrings
    struct WordMetrics {
        std::string word;
        int score;
        int competitive_substrings;
        long long metric;  // score * competitive_substrings
    };

    std::vector<WordMetrics> word_metrics;
    word_metrics.reserve(words.size());

    std::cout << "Scoring words and counting competitive substrings..." << std::endl;
    for (const auto& word : words) {
        int score = score15LetterWord(word);
        int comp_substrings = countCompetitiveSubstrings(word);
        long long metric = static_cast<long long>(score) * comp_substrings;
        word_metrics.push_back({word, score, comp_substrings, metric});
    }

    // Sort by metric (score * competitive_substrings) in descending order
    std::sort(word_metrics.begin(), word_metrics.end(),
              [](const WordMetrics& a, const WordMetrics& b) {
                  return a.metric > b.metric;
              });

    // Add randomness: shuffle the top words to get different triplets each time
    int pool_size = 140;
    std::vector<WordMetrics> top_words(word_metrics.begin(),
                                        word_metrics.begin() + pool_size);
    std::shuffle(top_words.begin(), top_words.end(), rng_);

    // Try to find compatible triplets from the shuffled top words
    for (size_t i = 0; i < top_words.size(); ++i) {
        for (size_t j = i + 1; j < top_words.size(); ++j) {
            for (size_t k = j + 1; k < top_words.size(); ++k) {
                if (areWordsCompatible(top_words[i].word, top_words[j].word,
                                       top_words[k].word)) {
                    result.word1 = top_words[i].word;
                    result.word2 = top_words[j].word;
                    result.word3 = top_words[k].word;
                    result.found = true;

                    std::cout << "Found compatible 15-letter words:" << std::endl;
                    std::cout << "  Word 1: " << result.word1
                            << " (score: " << top_words[i].score
                            << ", competitive substrings: " << top_words[i].competitive_substrings
                            << ", metric: " << top_words[i].metric << ")"
                            << std::endl;
                    std::cout << "  Word 2: " << result.word2
                            << " (score: " << top_words[j].score
                            << ", competitive substrings: " << top_words[j].competitive_substrings
                            << ", metric: " << top_words[j].metric << ")"
                            << std::endl;
                    std::cout << "  Word 3: " << result.word3
                            << " (score: " << top_words[k].score
                            << ", competitive substrings: " << top_words[k].competitive_substrings
                            << ", metric: " << top_words[k].metric << ")"
                            << std::endl;

                    // Pre-calculate all valid substrings for the main words
                    std::cout << "Pre-calculating valid substrings..." << std::endl;
                    result.substrings1 = findValidSubstrings(result.word1);
                    result.substrings2 = findValidSubstrings(result.word2);
                    result.substrings3 = findValidSubstrings(result.word3);

                    std::cout << "Found " << result.substrings1.size() << " substrings for word 1" << std::endl;
                    std::cout << "Found " << result.substrings2.size() << " substrings for word 2" << std::endl;
                    std::cout << "Found " << result.substrings3.size() << " substrings for word 3" << std::endl;

                    return result;
                }
            }
        }
    }

    return result;
}

bool CompatibleWordFinder::areWordsCompatible(const std::string& word1,
                                               const std::string& word2,
                                               const std::string& word3) {
    // Three 15-letter words are compatible if we have enough tiles in a full
    // French Scrabble bag to form all three words without using jokers

    // Create a fresh tile bag to check if we can draw all three words
    TileBag temp_bag(0);  // Seed doesn't matter for this check

    // Combine all three words into a single string of required letters
    std::string combined_letters = word1 + word2 + word3;

    // Check if the bag can provide all these letters without using jokers
    return temp_bag.canDrawTilesWithoutJoker(combined_letters);
}

std::vector<std::string> CompatibleWordFinder::load15LetterWords() {
    std::vector<std::string> words;

    std::ifstream file("engine/dictionnaries/ods8_complete.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open dictionary file" << std::endl;
        return words;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove any trailing whitespace
        line.erase(line.find_last_not_of(" \n\r\t") + 1);

        // Only keep 15-letter words
        if (line.length() == 15) {
            words.push_back(line);
        }
    }
    file.close();

    return words;
}

int CompatibleWordFinder::score15LetterWord(const std::string& word) {
    // Create a board and scorer
    Board board;
    Scorer scorer;

    // Create a move: place word horizontally starting at (0, 0)
    Move move(0, 0, Direction::HORIZONTAL, word);

    // Add tile placements for all 15 letters
    for (int i = 0; i < 15; i++) {
        move.addPlacement(TilePlacement(0, i, word[i], true, false));
    }

    // Score the move
    return scorer.scoreMove(board, move);
}

std::vector<CompatibleWordFinder::SubstringInfo>
CompatibleWordFinder::findValidSubstrings(const std::string& word) {
    std::vector<SubstringInfo> valid_substrings;

    // Try all contiguous substrings of the word
    for (size_t start = 0; start < word.length(); ++start) {
        for (size_t length = 2; length <= std::min(word.length() - start, word.length() - 1); ++length) {
            std::string substring = word.substr(start, length);

            // Check if this substring is a valid word in the dictionary
            if (dawg_.contains(substring)) {
                valid_substrings.emplace_back(substring, start);
            }
        }
    }

    // Sort by length in descending order (longest substrings first)
    std::sort(valid_substrings.begin(), valid_substrings.end(),
              [](const SubstringInfo& a, const SubstringInfo& b) {
                  return a.substring.length() > b.substring.length();
              });

    return valid_substrings;
}

int CompatibleWordFinder::countCompetitiveSubstrings(const std::string& word) {
    // A 15-letter word can be placed at positions 0-14
    // Triple word squares are at positions 0, 7, and 14
    // A substring is competitive if it doesn't overlap any triple word square

    int count = 0;

    // Try all contiguous substrings
    for (size_t start = 0; start < word.length(); ++start) {
        for (size_t length = 2; length <= std::min(word.length() - start, word.length() - 1); ++length) {
            std::string substring = word.substr(start, length);

            // Check if this substring is valid in the dictionary
            if (!dawg_.contains(substring)) {
                continue;
            }

            // Check if substring overlaps any triple word square
            // The substring occupies positions [start, start + length - 1]
            size_t end = start + length - 1;

            // A substring is competitive if it doesn't include positions 0, 7, or 14
            bool overlaps_triple = (start <= 0 && 0 <= end) ||
                                   (start <= 7 && 7 <= end) ||
                                   (start <= 14 && 14 <= end);

            if (!overlaps_triple) {
                count++;
            }
        }
    }

    return count;
}

}  // namespace scradle
