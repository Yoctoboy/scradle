#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../engine/include/board.h"
#include "../../engine/include/move.h"
#include "../../engine/include/scorer.h"

using namespace scradle;

struct WordScore {
    std::string word;
    int score;
};

int main() {
    // Read all words from dictionary
    std::ifstream file("../../engine/dictionnaries/ods8_complete.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open dictionary file" << std::endl;
        return 1;
    }

    std::vector<std::string> words_15_letters;
    std::string line;
    while (std::getline(file, line)) {
        // Remove any trailing whitespace
        line.erase(line.find_last_not_of(" \n\r\t") + 1);

        // Only keep 15-letter words
        if (line.length() == 15) {
            words_15_letters.push_back(line);
        }
    }
    file.close();

    std::cout << "Found " << words_15_letters.size() << " words with 15 letters" << std::endl;

    // Create a board and scorer
    Board board;
    Scorer scorer;

    // Vector to store word scores
    std::vector<WordScore> word_scores;

    // Compute score for each 15-letter word placed horizontally in first row (row 0)
    for (const auto& word : words_15_letters) {
        // Create a move: place word horizontally starting at (0, 0)
        Move move(0, 0, Direction::HORIZONTAL, word);

        // Add tile placements for all 15 letters
        for (int i = 0; i < 15; i++) {
            move.addPlacement(TilePlacement(0, i, word[i], true, false));
        }

        // Score the move
        int score = scorer.scoreMove(board, move);
        word_scores.push_back({word, score});
    }

    // Sort by score (descending)
    std::sort(word_scores.begin(), word_scores.end(),
              [](const WordScore& a, const WordScore& b) { return a.score > b.score; });

    // Display results
    std::cout << "\n=== Top 20 HIGHEST scoring 15-letter words (first row, horizontal) ===" << std::endl;
    for (int i = 0; i < std::min(20, (int)word_scores.size()); i++) {
        std::cout << i + 1 << ". " << word_scores[i].word << " - " << word_scores[i].score
                  << " points" << std::endl;
    }

    std::cout << "\n=== Statistics ===" << std::endl;
    if (!word_scores.empty()) {
        int total_score = 0;
        for (const auto& ws : word_scores) {
            total_score += ws.score;
        }
        double avg_score = (double)total_score / word_scores.size();
        std::cout << "Average score: " << avg_score << " points" << std::endl;
        std::cout << "Highest score: " << word_scores.front().score << " points" << std::endl;
        std::cout << "Lowest score: " << word_scores.back().score << " points" << std::endl;
    }

    return 0;
}
