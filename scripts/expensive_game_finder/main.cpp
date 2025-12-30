#include "ExpensiveGameFinder.h"
#include "../../engine/include/dawg.h"
#include <iostream>
#include <random>

using namespace scradle;

int main(int argc, char* argv[]) {
    // Load the DAWG dictionary
    DAWG dawg;
    std::cout << "Loading DAWG dictionary..." << std::endl;
    if (!dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt")) {
        std::cerr << "Error: Could not load DAWG file" << std::endl;
        return 1;
    }
    std::cout << "DAWG loaded successfully" << std::endl;

    // Get seed from command line args, or use a random seed
    unsigned int seed;
    if (argc > 1) {
        seed = std::stoi(argv[1]);
    } else {
        std::random_device rd;
        seed = rd();
    }

    std::cout << "Using seed: " << seed << std::endl;

    // Create the expensive game finder
    ExpensiveGameFinder finder(dawg, seed);

    // Run the expensive game search
    int final_score = finder.findExpensiveGame();

    std::cout << "\n=== Expensive Game Finder Result ===" << std::endl;
    std::cout << "Final Score: " << final_score << std::endl;

    return 0;
}
