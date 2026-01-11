#ifndef SCRADLE_TOP_EVERYTIME_FINDER_H
#define SCRADLE_TOP_EVERYTIME_FINDER_H

#include "../../engine/include/game_state.h"
#include "../../engine/include/move_generator.h"
#include "../../engine/include/dawg.h"
#include "../../engine/include/move.h"
#include <vector>
#include <string>
#include <memory>
#include <stack>

namespace scradle {

/**
 * TopEverytimeFinder finds the most expensive duplicate Scrabble game
 * where at each step we must play the highest-scoring move possible.
 *
 * The constraint is that the rack always contains ALL tiles from the bag,
 * and we can only choose between equally good top moves.
 *
 * Uses DFS to enumerate all possibilities when there are multiple
 * equally-scoring top moves, and logs finished games to separate files.
 */
class TopEverytimeFinder {
public:
    /**
     * Constructor
     * @param dawg Reference to the dictionary DAWG for word validation
     * @param output_dir Directory to write game logs to
     */
    TopEverytimeFinder(const DAWG& dawg, const std::string& output_dir = "games_output");

    /**
     * Main entry point to find the most expensive game
     * Uses DFS to explore all paths where equal-scoring moves exist
     */
    void findTopEverytimeGames();

    /**
     * Get the best score found so far
     */
    int getBestScore() const { return best_score_; }

    /**
     * Get the number of completed games explored
     */
    int getGamesExplored() const { return games_explored_; }

private:
    /**
     * DFS recursive function to explore game tree
     * @param depth Current depth in the tree (for logging)
     */
    void dfsExploreGameTree(int depth);

    /**
     * Fill rack with ALL tiles from the bag (for "always best move" mode)
     * Returns all tiles to rack temporarily for move generation
     * @return Vector of tiles that were added to rack
     */
    std::vector<char> fillRackWithAllTiles();

    /**
     * Return previously drawn tiles back to the bag and clear rack
     * @param tiles The tiles to return to the bag
     */
    void returnAllTilesToBag(const std::vector<char>& tiles);

    /**
     * Draw the exact tiles needed for a move and apply it
     * @param move The move to apply
     * @param all_tiles All tiles that were temporarily in rack
     * @return Vector of tiles that were drawn from the bag
     */
    std::vector<char> applyMoveWithExactTiles(const Move& move, const std::vector<char>& all_tiles);

    /**
     * Log the current completed game to a file
     * @param game_id Unique identifier for this game
     */
    void logGame(int game_id);

    /**
     * Check if game is over (no more valid moves or bag empty)
     */
    bool isGameOver();

    GameState game_state_;
    const DAWG& dawg_;
    std::string output_dir_;

    int best_score_;        // Best score found so far
    int games_explored_;    // Number of complete games explored
    int nodes_explored_;    // Total nodes in DFS tree

    // Stack to track exploration state at each depth
    // Each entry is: (current_branch_index, total_branches)
    std::vector<std::pair<int, int>> exploration_stack_;
};

}  // namespace scradle

#endif  // SCRADLE_TOP_EVERYTIME_FINDER_H
