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
}

void TileBag::initializeTiles() {
    tiles_.clear();

    // Vowels (45 total)
    for (int i = 0; i < 9; ++i) tiles_.insert('A');
    for (int i = 0; i < 15; ++i) tiles_.insert('E');
    for (int i = 0; i < 8; ++i) tiles_.insert('I');
    for (int i = 0; i < 6; ++i) tiles_.insert('O');
    for (int i = 0; i < 6; ++i) tiles_.insert('U');
    tiles_.insert('Y');  // Y is a vowel in French

    // Consonants (55 total)
    for (int i = 0; i < 2; ++i) tiles_.insert('B');
    for (int i = 0; i < 2; ++i) tiles_.insert('C');
    for (int i = 0; i < 3; ++i) tiles_.insert('D');
    for (int i = 0; i < 2; ++i) tiles_.insert('F');
    for (int i = 0; i < 2; ++i) tiles_.insert('G');
    for (int i = 0; i < 2; ++i) tiles_.insert('H');
    tiles_.insert('J');
    tiles_.insert('K');
    for (int i = 0; i < 5; ++i) tiles_.insert('L');
    for (int i = 0; i < 3; ++i) tiles_.insert('M');
    for (int i = 0; i < 6; ++i) tiles_.insert('N');
    for (int i = 0; i < 2; ++i) tiles_.insert('P');
    tiles_.insert('Q');
    for (int i = 0; i < 6; ++i) tiles_.insert('R');
    for (int i = 0; i < 6; ++i) tiles_.insert('S');
    for (int i = 0; i < 6; ++i) tiles_.insert('T');
    for (int i = 0; i < 2; ++i) tiles_.insert('V');
    tiles_.insert('W');
    tiles_.insert('X');
    tiles_.insert('Z');

    // Blanks (2 total)
    for (int i = 0; i < 2; ++i) tiles_.insert('?');
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

    // Select a random tile from the multiset
    std::uniform_int_distribution<size_t> dist(0, tiles_.size() - 1);
    size_t random_index = dist(rng_);

    auto it = tiles_.begin();
    std::advance(it, random_index);
    char tile = *it;
    tiles_.erase(it);
    return tile;
}

char TileBag::drawTile(char letter) {
    auto it = tiles_.find(letter);
    if (it != tiles_.end()) {
        tiles_.erase(it);
        return letter;
    }

    // If the requested letter is not available, try to draw a joker
    auto joker_it = tiles_.find('?');
    if (joker_it != tiles_.end()) {
        tiles_.erase(joker_it);
        return '?';
    }

    return '\0';
}

void TileBag::returnTiles(const std::string& tiles) {
    for (char tile : tiles) {
        if (tile != '\0') {
            tiles_.insert(tile);
        }
    }
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
        if (isConsonant(tile)) {
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

bool TileBag::contains(char letter) const {
    return tiles_.find(letter) != tiles_.end();
}

bool TileBag::canDrawTiles(const std::string& letters) {
    // Create a temporary copy of the tiles to simulate drawing
    std::multiset<char> temp_tiles = tiles_;

    for (char letter : letters) {
        auto it = temp_tiles.find(letter);
        if (it != temp_tiles.end()) {
            // Letter is available, remove it from the temp set
            temp_tiles.erase(it);
        } else {
            // Letter not available, try to use a joker
            auto joker_it = temp_tiles.find('?');
            if (joker_it != temp_tiles.end()) {
                temp_tiles.erase(joker_it);
            } else {
                // Neither the letter nor a joker is available
                return false;
            }
        }
    }

    return true;
}

}  // namespace scradle
