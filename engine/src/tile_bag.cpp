#include "tile_bag.h"

#include <algorithm>
#include <sstream>

namespace scradle {

TileBag::TileBag(unsigned int seed) : seed_(seed) {
    if (seed == 0) {
        std::random_device rd;
        seed_ = rd();
    }
    rng_.seed(seed_);
    initializeTiles();
    shuffle();
}

void TileBag::initializeTiles() {
    tiles_.clear();
    tiles_.reserve(TOTAL_TILES);

    // Vowels (52 total)
    tiles_.insert(tiles_.end(), 9, 'A');
    tiles_.insert(tiles_.end(), 15, 'E');
    tiles_.insert(tiles_.end(), 8, 'I');
    tiles_.insert(tiles_.end(), 6, 'O');
    tiles_.insert(tiles_.end(), 6, 'U');
    tiles_.insert(tiles_.end(), 1, 'Y');  // Y is a vowel in French

    // Consonants (46 total)
    tiles_.insert(tiles_.end(), 2, 'B');
    tiles_.insert(tiles_.end(), 2, 'C');
    tiles_.insert(tiles_.end(), 3, 'D');
    tiles_.insert(tiles_.end(), 2, 'F');
    tiles_.insert(tiles_.end(), 2, 'G');
    tiles_.insert(tiles_.end(), 2, 'H');
    tiles_.insert(tiles_.end(), 1, 'J');
    tiles_.insert(tiles_.end(), 1, 'K');
    tiles_.insert(tiles_.end(), 5, 'L');
    tiles_.insert(tiles_.end(), 3, 'M');
    tiles_.insert(tiles_.end(), 6, 'N');
    tiles_.insert(tiles_.end(), 2, 'P');
    tiles_.insert(tiles_.end(), 1, 'Q');
    tiles_.insert(tiles_.end(), 6, 'R');
    tiles_.insert(tiles_.end(), 6, 'S');
    tiles_.insert(tiles_.end(), 6, 'T');
    tiles_.insert(tiles_.end(), 2, 'V');
    tiles_.insert(tiles_.end(), 1, 'W');
    tiles_.insert(tiles_.end(), 1, 'X');
    tiles_.insert(tiles_.end(), 1, 'Z');

    // Blanks (2 total)
    tiles_.insert(tiles_.end(), 2, '?');
}

void TileBag::shuffle() {
    std::shuffle(tiles_.begin(), tiles_.end(), rng_);
}

std::string TileBag::drawTiles(int count) {
    std::string drawn;
    int actual_count = std::min(count, static_cast<int>(tiles_.size()));

    for (int i = 0; i < actual_count; ++i) {
        drawn += drawTile();
    }

    return drawn;
}

char TileBag::drawTile() {
    if (tiles_.empty()) {
        return '\0';
    }

    char tile = tiles_.back();
    tiles_.pop_back();
    return tile;
}

void TileBag::returnTiles(const std::string& tiles) {
    for (char tile : tiles) {
        if (tile != '\0') {
            tiles_.push_back(tile);
        }
    }
    shuffle();
}

int TileBag::vowelCount() const {
    int count = 0;
    for (char tile : tiles_) {
        if (isVowel(tile)) {
            ++count;
        }
    }
    return count;
}

int TileBag::consonantCount() const {
    int count = 0;
    for (char tile : tiles_) {
        if (!isVowel(tile) && tile != '?') {
            ++count;
        }
    }
    return count;
}

bool TileBag::hasVowels() const {
    for (char tile : tiles_) {
        if (isVowel(tile)) {
            return true;
        }
    }
    return false;
}

bool TileBag::hasConsonants() const {
    for (char tile : tiles_) {
        if (!isVowel(tile) && tile != '?') {
            return true;
        }
    }
    return false;
}

void TileBag::reset() {
    rng_.seed(seed_);
    initializeTiles();
    shuffle();
}

std::string TileBag::toString() const {
    std::ostringstream oss;
    oss << "TileBag[" << tiles_.size() << " tiles remaining]: ";
    for (char tile : tiles_) {
        oss << tile;
    }
    return oss.str();
}

bool TileBag::isVowel(char letter) {
    return letter == '?' ||
           letter == 'A' || letter == 'E' || letter == 'I' ||
           letter == 'O' || letter == 'U' || letter == 'Y';
}

bool TileBag::isConsonant(char letter) {
    return letter == '?' || !isVowel(letter);
}

bool TileBag::canMakeValidRack(int move_count) const {
    int vowels = 0;
    int consonants = 0;

    for (char tile : tiles_) {
        if (isVowel(tile)) {
            vowels++;
        }
        if (isConsonant(tile)) {
            consonants++;
        }
    }

    // Before move 15 (moves 0-15): need at least 2 vowels AND 2 consonants
    if (move_count <= 15) {
        return vowels >= 2 && consonants >= 2;
    }
    // After move 15 (moves 16+): need at least 1 vowel AND 1 consonant
    else {
        return vowels >= 1 && consonants >= 1;
    }
}

}  // namespace scradle
