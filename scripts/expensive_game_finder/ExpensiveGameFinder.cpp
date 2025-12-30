#include "ExpensiveGameFinder.h"
#include "../../engine/include/scorer.h"
#include "../../engine/include/tile_bag.h"
#include "../../engine/include/move_generator.h"
#include "../../engine/include/rack.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>

namespace scradle {

ExpensiveGameFinder::ExpensiveGameFinder(const DAWG& dawg, unsigned int seed)
    : game_state_(seed), dawg_(dawg) {}

int ExpensiveGameFinder::findExpensiveGame() {
    std::cout << "Starting expensive game search..." << std::endl;

    // Step 1: Find three mutually compatible high-scoring 15-letter words
    auto [main_word1, main_word2, main_word3] = findCompatible15LetterWords();

    if (main_word1.empty() || main_word2.empty() || main_word3.empty()) {
        std::cerr << "Could not find compatible 15-letter words" << std::endl;
        return 0;
    }

    std::cout << "Found compatible 15-letter words:" << std::endl;
    std::cout << "  Word 1: " << main_word1 << " (score: " << score15LetterWord(main_word1) << ")" << std::endl;
    std::cout << "  Word 2: " << main_word2 << " (score: " << score15LetterWord(main_word2) << ")" << std::endl;
    std::cout << "  Word 3: " << main_word3 << " (score: " << score15LetterWord(main_word3) << ")" << std::endl;

    // Main game loop
    int attempts = 0;
    int rejected_in_a_row = 0;
    const int MAX_ATTEMPTS = 50000; // Prevent infinite loops
    const int MAX_REJECTED_BEFORE_BACKTRACK = 1000; // Backtrack if stuck
    int previous_needed_tiles = 45; // Start with maximum (3 words × 15 letters)

    while (!game_state_.isGameOver() && attempts < MAX_ATTEMPTS) {
        attempts++;

        // Check if we're stuck - too many rejections in a row
        if (rejected_in_a_row >= MAX_REJECTED_BEFORE_BACKTRACK && game_state_.getMoveCount() > 3) {

            // Undo the last accepted move to try a different path
            game_state_.undoLastMove();

            // Recalculate progress tracking
            previous_needed_tiles = calculateTotalNeededTiles(main_word1, main_word2, main_word3, game_state_.getBoard());

            // Reset rejection counter
            rejected_in_a_row = 0;

            // Clear rack to get a fresh draw
            std::string rack_tiles = game_state_.getRack().getTiles();
            game_state_.getTileBag().returnTiles(rack_tiles);
            game_state_.getRack().clear();
            std::cout << std::endl << "Grid is stuck after " << MAX_REJECTED_BEFORE_BACKTRACK
                      << " rejections. Backtracking one move (back to " << previous_needed_tiles << " needed tiles)" << std::endl;
        }

        // Check if any of the 3 target words can be played in a single move
        std::string playable_word = findPlayableWord(main_word1, main_word2, main_word3);
        if (!playable_word.empty()) {
            // Play the target word directly!
            if (playSpecificWord(playable_word)) {
                rejected_in_a_row = 0;
                // Reset progress tracking after placing a target word
                previous_needed_tiles = calculateTotalNeededTiles(main_word1, main_word2, main_word3, game_state_.getBoard());
                continue;
            }
        }

        // Refill rack with random tiles
        game_state_.refillRack();

        // Generate all possible moves
        MoveGenerator move_gen(game_state_.getBoard(), game_state_.getRack(), dawg_);
        std::vector<Move> best_moves = move_gen.getBestMove();

        if (best_moves.empty()) {
            std::cout << "No valid moves available, game over." << std::endl;
            break;
        }

        // Play the first best move (they all have the same score in duplicate Scrabble)
        Move best_move = best_moves[0];
        game_state_.applyMove(best_move);

        // Calculate needed tiles after the move
        int needed_after_move = calculateTotalNeededTiles(main_word1, main_word2, main_word3, game_state_.getBoard());

        // Check if we made progress AND it's still possible to place all 3 words
        bool still_possible = canPlaceWordsOnGrid(main_word1, main_word2, main_word3, game_state_.getBoard());
        bool made_progress = needed_after_move != -1 && needed_after_move < previous_needed_tiles;
        bool early_move = game_state_.getMoveCount() <= 3;

        if (still_possible && (made_progress || early_move)) {
            // Good move - we made progress (or it's early) and placement is still possible
            std::cout << std::endl << "Move " << game_state_.getMoveCount() << ": "
                      << best_move.toString()
                      << " - Total: " << game_state_.getTotalScore()
                      << " | Needed tiles: " << needed_after_move << " (was " << previous_needed_tiles << ")" << std::endl;
            rejected_in_a_row = 0;
            previous_needed_tiles = needed_after_move;
        } else {
            // Bad move - no progress made or placement became impossible
            game_state_.undoLastMove();

            // Return tiles to bag and clear rack to get a fresh draw
            std::string rack_tiles = game_state_.getRack().getTiles();
            game_state_.getTileBag().returnTiles(rack_tiles);
            game_state_.getRack().clear();
            rejected_in_a_row++;

            std::cout << ((rejected_in_a_row > 1) ? "\r" : "") << "Move rejected (" << rejected_in_a_row << "), "
                      << (!still_possible ? "placement impossible" : "no progress made") << "..."
                      << "                  " << std::flush;

        }
    }

    std::cout << std::endl << "Max attempts reached,  board state:" << std::endl;
    std::cout << game_state_.getBoard().toString() << std::endl;
    std::cout << "Finishing normally" << std::endl;
    game_state_.refillRack();
    while (!game_state_.isGameOver() && !game_state_.getRack().size() == 0){
        game_state_.findAndPlayBestMove(dawg_, true);
        game_state_.refillRack();
    }

    std::cout << "\n=== Game Complete ===" << std::endl;
    std::cout << std::endl << "With words:" << std::endl;
    std::cout << "- " << main_word1  << std::endl;
    std::cout << "- " << main_word2 << std::endl;
    std::cout << "- " << main_word3 << std::endl;
    game_state_.printSummary();

    return game_state_.getTotalScore();
}

std::tuple<std::string, std::string, std::string> ExpensiveGameFinder::findCompatible15LetterWords() {
    // Load all 15-letter words
    std::vector<std::string> words = load15LetterWords();

    if (words.empty()) {
        return {"", "", ""};
    }

    std::cout << "Loaded " << words.size() << " 15-letter words" << std::endl;

    // Score all words
    struct WordScore {
        std::string word;
        int score;
    };

    std::vector<WordScore> scored_words;
    scored_words.reserve(words.size());

    for (const auto& word : words) {
        int score = score15LetterWord(word);
        scored_words.push_back({word, score});
    }

    // Sort by score (descending)
    std::sort(scored_words.begin(), scored_words.end(),
              [](const WordScore& a, const WordScore& b) { return a.score > b.score; });

    // Add randomness: shuffle the top words to get different triplets each time
    int pool_size = 140;
    std::vector<WordScore> top_words(scored_words.begin(), scored_words.begin() + pool_size);
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(top_words.begin(), top_words.end(), rng);

    // Try to find compatible triplets from the shuffled top words
    for (size_t i = 0; i < top_words.size(); ++i) {
        for (size_t j = i + 1; j < top_words.size(); ++j) {
            for (size_t k = j + 1; k < top_words.size(); ++k) {
                if (areWordsCompatible(top_words[i].word, top_words[j].word, top_words[k].word)) {
                    return {top_words[i].word, top_words[j].word, top_words[k].word};
                }
            }
        }
    }

    return {"", "", ""};
}

bool ExpensiveGameFinder::areWordsCompatible(const std::string& word1, const std::string& word2, const std::string& word3) {
    // Three 15-letter words are compatible if we have enough tiles in a full French Scrabble
    // bag to form all three words without using jokers

    // Create a fresh tile bag to check if we can draw all three words
    TileBag temp_bag(0);  // Seed doesn't matter for this check

    // Combine all three words into a single string of required letters
    std::string combined_letters = word1 + word2 + word3;

    // Check if the bag can provide all these letters without using jokers
    return temp_bag.canDrawTilesWithoutJoker(combined_letters);
}

std::vector<std::string> ExpensiveGameFinder::load15LetterWords() {
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

int ExpensiveGameFinder::score15LetterWord(const std::string& word) {
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

RawMove ExpensiveGameFinder::createRawMoveForWord(const std::string& word, const Board& board,
                                                   int row, int col, Direction direction) {
    RawMove raw_move;
    raw_move.start_row = row;
    raw_move.start_col = col;
    raw_move.direction = direction;

    // Place all 15 letters of the word
    for (int i = 0; i < 15; i++) {
        int current_row = row;
        int current_col = col;

        if (direction == Direction::HORIZONTAL) {
            current_col += i;
        } else {
            current_row += i;
        }

        char existing_letter = board.getLetter(current_row, current_col);

        if (existing_letter == ' ') {
            // Empty cell - need to place this letter from rack
            raw_move.placements.push_back(
                TilePlacement(current_row, current_col, word[i], true, false));
        } else if (existing_letter == word[i]) {
            // Letter already exists and matches - use it
            raw_move.placements.push_back(
                TilePlacement(current_row, current_col, word[i], false, false));
        } else {
            // Letter exists but doesn't match - invalid placement
            // Return an empty RawMove to signal failure
            raw_move.placements.clear();
            return raw_move;
        }
    }

    return raw_move;
}

bool ExpensiveGameFinder::canPlaceWordsOnGrid(const std::string& word1, const std::string& word2,
                                               const std::string& word3, const Board& board) {
    // Create a vector of all three words to try all permutations
    std::vector<std::string> words = {word1, word2, word3};

    // Try all 6 permutations of the three words
    std::sort(words.begin(), words.end());
    do {
        // Try vertical placement: columns 0, 7, 14 with current permutation
        RawMove raw1_vert = createRawMoveForWord(words[0], board, 0, 0, Direction::VERTICAL);
        RawMove raw2_vert = createRawMoveForWord(words[1], board, 0, 7, Direction::VERTICAL);
        RawMove raw3_vert = createRawMoveForWord(words[2], board, 0, 14, Direction::VERTICAL);

        // Check if all vertical placements are possible (non-empty placements)
        if (!raw1_vert.placements.empty() && !raw2_vert.placements.empty() && !raw3_vert.placements.empty()) {
            // Collect all letters needed from rack (tiles not already on board)
            std::string needed_tiles;
            for (const auto& p : raw1_vert.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }
            for (const auto& p : raw2_vert.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }
            for (const auto& p : raw3_vert.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }

            // Check if the tile bag can provide all needed tiles (with jokers)
            TileBag temp_bag(0);
            if (!temp_bag.canDrawTiles(needed_tiles)) {
                continue; // Try next permutation
            }

            // Create a temporary rack with all needed letters
            Rack temp_rack;
            for (char c : needed_tiles) {
                temp_rack.addTile(c);
            }

            // Validate each move using MoveGenerator
            MoveGenerator move_gen(board, temp_rack, dawg_);

            if (move_gen.isValidMove(raw1_vert) &&
                move_gen.isValidMove(raw2_vert) &&
                move_gen.isValidMove(raw3_vert)) {
                return true;
            }
        }

        // Try horizontal placement: rows 0, 7, 14 with current permutation
        RawMove raw1_horiz = createRawMoveForWord(words[0], board, 0, 0, Direction::HORIZONTAL);
        RawMove raw2_horiz = createRawMoveForWord(words[1], board, 7, 0, Direction::HORIZONTAL);
        RawMove raw3_horiz = createRawMoveForWord(words[2], board, 14, 0, Direction::HORIZONTAL);

        // Check if all horizontal placements are possible
        if (!raw1_horiz.placements.empty() && !raw2_horiz.placements.empty() && !raw3_horiz.placements.empty()) {
            // Collect all letters needed from rack (tiles not already on board)
            std::string needed_tiles;
            for (const auto& p : raw1_horiz.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }
            for (const auto& p : raw2_horiz.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }
            for (const auto& p : raw3_horiz.placements) {
                if (p.is_from_rack) needed_tiles += p.letter;
            }

            // Check if the tile bag can provide all needed tiles (with jokers)
            TileBag temp_bag(0);
            if (!temp_bag.canDrawTiles(needed_tiles)) {
                continue; // Try next permutation
            }

            // Create a temporary rack with all needed letters
            Rack temp_rack;
            for (char c : needed_tiles) {
                temp_rack.addTile(c);
            }

            // Validate each move using MoveGenerator
            MoveGenerator move_gen(board, temp_rack, dawg_);

            if (move_gen.isValidMove(raw1_horiz) &&
                move_gen.isValidMove(raw2_horiz) &&
                move_gen.isValidMove(raw3_horiz)) {
                return true;
            }
        }
    } while (std::next_permutation(words.begin(), words.end()));

    return false;
}

std::string ExpensiveGameFinder::findPlayableWord(const std::string& word1, const std::string& word2,
                                                    const std::string& word3) {
    std::vector<std::string> words = {word1, word2, word3};
    const Board& board = game_state_.getBoard();

    for (const auto& word : words) {
        // Try all permutations and both orientations to find a valid placement
        std::vector<std::string> temp_words = {word1, word2, word3};
        std::sort(temp_words.begin(), temp_words.end());

        do {
            // Find which position this word would be in
            int word_index = -1;
            for (size_t i = 0; i < temp_words.size(); ++i) {
                if (temp_words[i] == word) {
                    word_index = i;
                    break;
                }
            }
            if (word_index == -1) continue;

            // Try vertical placement
            int vert_cols[] = {0, 7, 14};
            RawMove raw_vert = createRawMoveForWord(word, board, 0, vert_cols[word_index], Direction::VERTICAL);

            if (!raw_vert.placements.empty()) {
                // Count tiles needed from rack
                int tiles_from_rack = 0;
                std::string needed_tiles;
                for (const auto& p : raw_vert.placements) {
                    if (p.is_from_rack) {
                        tiles_from_rack++;
                        needed_tiles += p.letter;
                    }
                }

                // Check if we can play it in a single move (≤7 tiles)
                if (tiles_from_rack <= 7) {
                    // Check if tiles are available in the bag
                    TileBag temp_bag = game_state_.getTileBag();
                    if (temp_bag.canDrawTiles(needed_tiles)) {
                        // Validate the move
                        Rack temp_rack;
                        for (char c : needed_tiles) {
                            temp_rack.addTile(c);
                        }
                        MoveGenerator move_gen(board, temp_rack, dawg_);
                        if (move_gen.isValidMove(raw_vert)) {
                            return word;
                        }
                    }
                }
            }

            // Try horizontal placement
            int horiz_rows[] = {0, 7, 14};
            RawMove raw_horiz = createRawMoveForWord(word, board, horiz_rows[word_index], 0, Direction::HORIZONTAL);

            if (!raw_horiz.placements.empty()) {
                // Count tiles needed from rack
                int tiles_from_rack = 0;
                std::string needed_tiles;
                for (const auto& p : raw_horiz.placements) {
                    if (p.is_from_rack) {
                        tiles_from_rack++;
                        needed_tiles += p.letter;
                    }
                }

                // Check if we can play it in a single move (≤7 tiles)
                if (tiles_from_rack <= 7) {
                    // Check if tiles are available in the bag
                    TileBag temp_bag = game_state_.getTileBag();
                    if (temp_bag.canDrawTiles(needed_tiles)) {
                        // Validate the move
                        Rack temp_rack;
                        for (char c : needed_tiles) {
                            temp_rack.addTile(c);
                        }
                        MoveGenerator move_gen(board, temp_rack, dawg_);
                        if (move_gen.isValidMove(raw_horiz)) {
                            return word;
                        }
                    }
                }
            }
        } while (std::next_permutation(temp_words.begin(), temp_words.end()));
    }

    return "";
}

bool ExpensiveGameFinder::playSpecificWord(const std::string& word) {
    const Board& board = game_state_.getBoard();

    // Find the correct position and orientation for this word
    // Try all positions to find where it can be placed
    std::vector<std::pair<int, int>> positions_vert = {{0, 0}, {0, 7}, {0, 14}};
    std::vector<std::pair<int, int>> positions_horiz = {{0, 0}, {7, 0}, {14, 0}};

    RawMove valid_move;
    std::string needed_tiles;
    bool found = false;

    // Try vertical placements
    for (const auto& [row, col] : positions_vert) {
        RawMove raw = createRawMoveForWord(word, board, row, col, Direction::VERTICAL);
        if (!raw.placements.empty()) {
            int tiles_from_rack = 0;
            std::string tiles;
            for (const auto& p : raw.placements) {
                if (p.is_from_rack) {
                    tiles_from_rack++;
                    tiles += p.letter;
                }
            }

            if (0 < tiles_from_rack && tiles_from_rack <= 7) {
                TileBag temp_bag = game_state_.getTileBag();
                if (temp_bag.canDrawTiles(tiles)) {
                    Rack temp_rack;
                    for (char c : tiles) {
                        temp_rack.addTile(c);
                    }
                    MoveGenerator move_gen(board, temp_rack, dawg_);
                    if (move_gen.isValidMove(raw)) {
                        valid_move = raw;
                        needed_tiles = tiles;
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    // Try horizontal placements if not found
    if (!found) {
        for (const auto& [row, col] : positions_horiz) {
            RawMove raw = createRawMoveForWord(word, board, row, col, Direction::HORIZONTAL);
            if (!raw.placements.empty()) {
                int tiles_from_rack = 0;
                std::string tiles;
                for (const auto& p : raw.placements) {
                    if (p.is_from_rack) {
                        tiles_from_rack++;
                        tiles += p.letter;
                    }
                }

                if (0 < tiles_from_rack && tiles_from_rack <= 7) {
                    TileBag temp_bag = game_state_.getTileBag();
                    if (temp_bag.canDrawTiles(tiles)) {
                        Rack temp_rack;
                        for (char c : tiles) {
                            temp_rack.addTile(c);
                        }
                        MoveGenerator move_gen(board, temp_rack, dawg_);
                        if (move_gen.isValidMove(raw)) {
                            valid_move = raw;
                            needed_tiles = tiles;
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!found) {
        return false;
    }

    // Draw the exact tiles needed
    game_state_.getRack().clear();
    for (char c : needed_tiles) {
        // Draw the specific tile from the bag
        char drawn = game_state_.getTileBag().drawTile(c);
        game_state_.getRack().addTile(drawn);
    }

    // Create the Move object from RawMove
    Move move(valid_move.start_row, valid_move.start_col, valid_move.direction, word);
    for (const auto& p : valid_move.placements) {
        move.addPlacement(p);
    }

    // Score the move
    Scorer scorer;
    int score = scorer.scoreMove(board, move);
    move.setScore(score);

    // Apply the move
    game_state_.applyMove(move);

    std::cout << "\n\n*** PLAYED TARGET WORD: " << word << " (" << score << " pts) ***" << std::endl << std::endl;
    std::cout << "Total score: " << game_state_.getTotalScore() << std::endl;

    return true;
}

int ExpensiveGameFinder::countNeededTiles(const std::string& word, const Board& board,
                                           int row, int col, Direction direction) {
    // Create a RawMove to see what tiles would be needed
    RawMove raw_move = createRawMoveForWord(word, board, row, col, direction);

    // If placement is impossible (empty placements), return -1
    if (raw_move.placements.empty()) {
        return -1;
    }

    // Count how many tiles need to come from the rack
    int needed = 0;
    for (const auto& p : raw_move.placements) {
        if (p.is_from_rack) {
            needed++;
        }
    }

    return needed;
}

int ExpensiveGameFinder::calculateTotalNeededTiles(const std::string& word1, const std::string& word2,
                                                     const std::string& word3, const Board& board) {
    std::vector<std::string> words = {word1, word2, word3};

    // We need to find the best permutation and orientation that minimizes total needed tiles
    // Try all 6 permutations and both orientations
    int min_total_needed = -1;

    std::sort(words.begin(), words.end());
    do {
        // Try vertical placement: columns 0, 7, 14
        int vert_needed1 = countNeededTiles(words[0], board, 0, 0, Direction::VERTICAL);
        int vert_needed2 = countNeededTiles(words[1], board, 0, 7, Direction::VERTICAL);
        int vert_needed3 = countNeededTiles(words[2], board, 0, 14, Direction::VERTICAL);

        if (vert_needed1 != -1 && vert_needed2 != -1 && vert_needed3 != -1) {
            int total = vert_needed1 + vert_needed2 + vert_needed3;
            if (min_total_needed == -1 || total < min_total_needed) {
                min_total_needed = total;
            }
        }

        // Try horizontal placement: rows 0, 7, 14
        int horiz_needed1 = countNeededTiles(words[0], board, 0, 0, Direction::HORIZONTAL);
        int horiz_needed2 = countNeededTiles(words[1], board, 7, 0, Direction::HORIZONTAL);
        int horiz_needed3 = countNeededTiles(words[2], board, 14, 0, Direction::HORIZONTAL);

        if (horiz_needed1 != -1 && horiz_needed2 != -1 && horiz_needed3 != -1) {
            int total = horiz_needed1 + horiz_needed2 + horiz_needed3;
            if (min_total_needed == -1 || total < min_total_needed) {
                min_total_needed = total;
            }
        }
    } while (std::next_permutation(words.begin(), words.end()));

    return min_total_needed;
}

}  // namespace scradle
