#include "rack.h"

#include <algorithm>
#include <cctype>

#include "tile_bag.h"

using std::count;
using std::find;
using std::string;
using std::toupper;

namespace scradle {

Rack::Rack() : tiles_("") {}

Rack::Rack(const string& tiles) : tiles_(tiles) {
    // Convert to uppercase (except for '?' which represents blank tiles)
    for (char& c : tiles_) {
        if (c != '?') {
            c = toupper(static_cast<unsigned char>(c));
        }
    }
}

char Rack::getTile(int index) const {
    if (index >= 0 && index < static_cast<int>(tiles_.size())) {
        return tiles_[index];
    }
    return ' ';
}

void Rack::setTiles(const string& tiles) {
    tiles_ = tiles;
    for (char& c : tiles_) {
        if (c != '?') {
            c = toupper(static_cast<unsigned char>(c));
        }
    }
}

bool Rack::hasTile(char letter) const {
    char upper = toupper(static_cast<unsigned char>(letter));
    return tiles_.find(upper) != string::npos;
}

int Rack::countTile(char letter) const {
    char upper = toupper(static_cast<unsigned char>(letter));
    return count(tiles_.begin(), tiles_.end(), upper);
}

void Rack::removeTile(char letter) {
    char upper = toupper(static_cast<unsigned char>(letter));
    auto it = find(tiles_.begin(), tiles_.end(), upper);
    if (it != tiles_.end()) {
        tiles_.erase(it);
    }
}

void Rack::addTile(char letter) {
    if (tiles_.size() < MAX_TILES) {
        if (letter == '?') {
            tiles_ += '?';
        } else {
            tiles_ += toupper(static_cast<unsigned char>(letter));
        }
    }
}

bool Rack::isValid(int move_count) const {
    // Count vowels and consonants (blanks count as both)
    int vowels = 0;
    int consonants = 0;

    for (char tile : tiles_) {
        if (tile == '?') {
            // Blanks count as both vowel and consonant
            vowels++;
            consonants++;
        } else if (TileBag::isVowel(tile)) {
            vowels++;
        } else {
            consonants++;
        }
    }

    // Before move 15 (moves 0-15): need at least 2 vowels AND 2 consonants
    if (move_count <= 15) {
        return vowels >= 2 && consonants >= 2;
    }
    // After move 16 (moves 16+): need at least 1 vowel AND 1 consonant
    else {
        return vowels >= 1 && consonants >= 1;
    }
}

string Rack::toString() const {
    return tiles_.empty() ? "(empty)" : tiles_;
}

}  // namespace scradle
