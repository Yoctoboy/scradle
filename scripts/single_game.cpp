#include <iostream>
#include <iomanip>

#include "dawg.h"
#include "duplicate_game.h"

using namespace scradle;
using namespace std;

int main(int argc, char* argv[]) {
    // Check for seed argument
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <seed>" << endl;
        cerr << "  seed: Game seed (required)" << endl;
        cerr << "\nExample:" << endl;
        cerr << "  " << argv[0] << " 12345" << endl;
        return 1;
    }

    // Parse seed
    unsigned int seed = static_cast<unsigned int>(atoi(argv[1]));

    // Load dictionary
    DAWG dawg;
    if (!dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt")) {
        cerr << "Failed to load dictionary" << endl;
        return 1;
    }

    // Create and run game
    DuplicateGame game(dawg, seed);
    game.playGame(true);  // true to display summary

    return 0;
}
