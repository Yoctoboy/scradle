#ifndef SCRADLE_RACK_H
#define SCRADLE_RACK_H

#include <string>

namespace scradle {

// Player's rack of tiles
class Rack {
public:
    static constexpr int MAX_TILES = 7;

    Rack();
    explicit Rack(const std::string& tiles);

    // Tile access
    int size() const { return tiles_.size(); }
    char getTile(int index) const;
    void setTiles(const std::string& tiles);

    // Tile operations
    bool hasTile(char letter) const;
    int countTile(char letter) const;
    void removeTile(char letter);
    void addTile(char letter);

    // Display
    std::string toString() const;

private:
    std::string tiles_;  // Current tiles in rack
};

} // namespace scradle

#endif // SCRADLE_RACK_H
