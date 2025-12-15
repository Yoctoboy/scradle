#include "rack.h"
#include <algorithm>
#include <cctype>

using std::string;
using std::toupper;
using std::count;
using std::find;

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

string Rack::toString() const {
    return tiles_.empty() ? "(empty)" : tiles_;
}

} // namespace scradle
