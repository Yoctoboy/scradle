#ifndef SCRADLE_TILE_BAG_H
#define SCRADLE_TILE_BAG_H

#include <string>
#include <random>
#include <vector>

namespace scradle {

// Manages the bag of tiles for a Scrabble game
class TileBag {
public:
    // French Scrabble has 100 tiles total
    static constexpr int TOTAL_TILES = 100;

    // Constructor with optional seed (default uses random_device)
    explicit TileBag(unsigned int seed = 0);

    // Draw N tiles from the bag (returns actual number drawn)
    std::string drawTiles(int count);

    // Draw a single tile
    char drawTile();

    // Return tiles to the bag (for testing or undo)
    void returnTiles(const std::string& tiles);

    // State queries
    int remainingCount() const { return tiles_.size(); }
    bool isEmpty() const { return tiles_.empty(); }

    // Statistics
    int vowelCount() const;
    int consonantCount() const;
    bool hasVowels() const;
    bool hasConsonants() const;

    // Check if bag has enough vowels and consonants to make a valid rack
    // Before move 15: needs >= 2 vowels AND >= 2 consonants
    // After move 15: needs >= 1 vowel AND >= 1 consonant
    bool canMakeValidRack(int move_count) const;

    // Reset the bag to initial state with same seed
    void reset();

    // Get seed for reproducibility
    unsigned int getSeed() const { return seed_; }

    // Get current state for debugging
    std::string toString() const;

    // Helpers to check letter types
    // Blanks ('?') count as both vowel and consonant
    static bool isVowel(char letter);
    static bool isConsonant(char letter);

private:
    std::vector<char> tiles_;
    std::mt19937 rng_;
    unsigned int seed_;

    // Initialize the bag with French Scrabble distribution
    void initializeTiles();

    // Shuffle the tiles
    void shuffle();
};

} // namespace scradle

#endif // SCRADLE_TILE_BAG_H
