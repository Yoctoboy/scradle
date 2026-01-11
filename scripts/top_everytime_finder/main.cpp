#include "TopEverytimeFinder.h"
#include "../../engine/include/dawg.h"
#include <iostream>
#include <string>

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
    std::cout << std::endl;

    // Get output directory from command line args, or use default
    std::string output_dir = "games_output";
    if (argc > 1) {
        output_dir = argv[1];
    }

    std::cout << "Output directory: " << output_dir << std::endl;
    std::cout << std::endl;

    // Create the top everytime finder
    TopEverytimeFinder finder(dawg, output_dir);

    // Run the DFS exploration
    finder.findTopEverytimeGames();

    std::cout << "\n=== Top Everytime Finder Result ===" << std::endl;
    std::cout << "Best Score Found: " << finder.getBestScore() << std::endl;
    std::cout << "Total Games Explored: " << finder.getGamesExplored() << std::endl;

    return 0;
}
