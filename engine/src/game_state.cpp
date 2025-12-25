#include "game_state.h"

#include <iostream>

namespace scradle {

GameState::GameState(unsigned int seed)
    : board_(), rack_(), tile_bag_(seed), seed_(seed), total_score_(0), bingo_count_(0), move_history_() {}

void GameState::applyMove(const Move& move) {
    // Place tiles on board
    for (const auto& placement : move.getPlacements()) {
        if (placement.is_from_rack) {
            board_.setLetter(placement.row, placement.col, placement.letter);
            // Remove tile from rack
            rack_.removeTile(placement.is_blank ? '?' : placement.letter);
        }
    }

    // Update statistics
    total_score_ += move.getScore();
    if (move.isBingo()) {
        bingo_count_++;
    }

    // Add to move history
    move_history_.push_back(move);
}

void GameState::refillRack() {
    int tiles_needed = Rack::MAX_TILES - rack_.size();
    if (tiles_needed > 0) {
        std::string new_tiles = tile_bag_.drawTiles(tiles_needed);
        for (char tile : new_tiles) {
            rack_.addTile(tile);
        }
    }

    // Check if the rack is valid according to the rules
    // If invalid and the bag can potentially make a valid rack, return tiles and try again
    int move_count = getMoveCount();
    while (!rack_.isValid(move_count) && tile_bag_.canMakeValidRack(move_count)) {
        // Return all tiles to the bag
        std::string rack_tiles = rack_.getTiles();
        tile_bag_.returnTiles(rack_tiles);
        rack_.clear();

        // Draw 7 new tiles (or as many as available)
        std::string new_tiles = tile_bag_.drawTiles(Rack::MAX_TILES);
        for (char tile : new_tiles) {
            rack_.addTile(tile);
        }
    }
}

bool GameState::isGameOver() const {
    // Game is over when bag+rack combined have no vowels OR no consonants
    // (meaning it's impossible to form valid words)

    // Count vowels and consonants in rack
    int rack_vowels = 0;
    int rack_consonants = 0;
    for (int i = 0; i < rack_.size(); ++i) {
        char tile = rack_.getTile(i);
        if (tile == '?') {
            // Blanks can be either vowel or consonant, so we skip them
            // They don't prevent game-over
            continue;
        }
        if (TileBag::isVowel(tile)) {
            rack_vowels++;
        } else {
            rack_consonants++;
        }
    }

    // Total vowels/consonants = bag + rack
    int total_vowels = tile_bag_.vowelCount() + rack_vowels;
    int total_consonants = tile_bag_.consonantCount() + rack_consonants;

    // Game over if we have no vowels OR no consonants remaining
    return total_vowels == 0 || total_consonants == 0;
}

void GameState::reset() {
    board_ = Board();
    rack_ = Rack();
    tile_bag_.reset();
    total_score_ = 0;
    bingo_count_ = 0;
    move_history_.clear();
}

void GameState::printSummary() const {
    std::cout << "\n=== Duplicate Scrabble Game ===\n";
    std::cout << "Seed: " << seed_ << "\n";
    std::cout << "Final Score: " << total_score_ << "\n";
    std::cout << "Moves: " << move_history_.size() << "\n";
    std::cout << "Bingos: " << bingo_count_ << "\n\n";

    // if (!move_history_.empty()) {
    //     std::cout << "Move History:\n";
    //     for (size_t i = 0; i < move_history_.size(); ++i) {
    //         const Move& move = move_history_[i];
    //         std::cout << (i + 1) << ". " << move.toString() << std::endl;
    //     }
    // }

    std::cout << "\nFinal Board:\n";
    board_.display();
}

}  // namespace scradle
