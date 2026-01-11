#include "TopEverytimeFinder.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

namespace scradle {

TopEverytimeFinder::TopEverytimeFinder(const DAWG& dawg, const std::string& output_dir)
    : game_state_(), dawg_(dawg), output_dir_(output_dir),
      best_score_(0), games_explored_(0), nodes_explored_(0) {
    // Create output directory if it doesn't exist
    mkdir(output_dir_.c_str(), 0755);
}

void TopEverytimeFinder::findTopEverytimeGames() {
    std::cout << "Starting DFS exploration of all top-scoring game paths..." << std::endl;
    std::cout << std::endl;

    // Start DFS from initial empty board
    dfsExploreGameTree(0);

    std::cout << std::endl;
    std::cout << "=== Exploration Complete ===" << std::endl;
    std::cout << "Total games explored: " << games_explored_ << std::endl;
    std::cout << "Total nodes explored: " << nodes_explored_ << std::endl;
    std::cout << "Best score found: " << best_score_ << std::endl;
}

void TopEverytimeFinder::dfsExploreGameTree(int depth) {
    nodes_explored_++;

    // Print progress periodically
    if (nodes_explored_ % 100 == 0) {
        std::cout << "Nodes explored: " << nodes_explored_
                  << ", Games completed: " << games_explored_
                  << ", Current depth: " << depth
                  << ", Best score: " << best_score_ << std::endl;
    }

    // Check if game is over
    if (isGameOver()) {
        // Log this completed game
        games_explored_++;
        int final_score = game_state_.getTotalScore();

        std::cout << "[Depth " << depth << "] Game over! Final score: " << final_score << std::endl;

        if (final_score > best_score_) {
            best_score_ = final_score;
            std::cout << "*** NEW BEST SCORE: " << best_score_
                      << " (Game #" << games_explored_ << ") ***" << std::endl;
            std::cout << game_state_.toString() << std::endl;
        }

        logGame(games_explored_);
        return;
    }

    // Fill rack with ALL tiles from bag temporarily
    std::vector<char> all_tiles = fillRackWithAllTiles();

    // Generate all moves with this super-rack
    MoveGenerator move_gen(game_state_.getBoard(), game_state_.getRack(), dawg_);
    std::vector<Move> best_moves = move_gen.getBestMove();
    // Return all tiles back to bag before we start exploring
    returnAllTilesToBag(all_tiles);

    // If no valid moves, game is over
    if (best_moves.empty()) {
        games_explored_++;
        int final_score = game_state_.getTotalScore();

        std::cout << "[Depth " << depth << "] No valid moves. Game over! Final score: " << final_score << std::endl;

        if (final_score > best_score_) {
            best_score_ = final_score;
            std::cout << "*** NEW BEST SCORE: " << best_score_
                      << " (Game #" << games_explored_ << ") ***" << std::endl;
            std::cout << game_state_.toString() << std::endl;
        }

        logGame(games_explored_);
        return;
    }

    // For first move, filter to only horizontal moves (convention)
    if (game_state_.getMoveCount() == 0) {
        std::vector<Move> horizontal_moves;
        for (const auto& move : best_moves) {
            if (move.getDirection() == Direction::HORIZONTAL) {
                horizontal_moves.push_back(move);
            }
        }
        best_moves = horizontal_moves;
    }

    // Log available moves at this node
    int best_score = best_moves.empty() ? 0 : best_moves[0].getScore();
    std::cout << "[Node " << nodes_explored_ << ", Depth " << depth
              << "] " << best_moves.size() << " best move(s) available for "
              << best_score << " points" << std::endl;

    // Print current exploration path
    std::cout << "[Node " << nodes_explored_ << "] Current path: ";
    for (size_t i = 0; i < exploration_stack_.size(); i++) {
        std::cout << (exploration_stack_[i].first + 1) << "/" << exploration_stack_[i].second;
        if (i < exploration_stack_.size() - 1) std::cout << " -> ";
    }
    std::cout << " -> exploring " << best_moves.size() << " branches" << std::endl;

    // Calculate remaining unexplored nodes at current level
    int remaining_at_level = 0;
    for (const auto& level : exploration_stack_) {
        remaining_at_level += (level.second - level.first - 1);
    }
    std::cout << "[Node " << nodes_explored_ << "] Remaining unexplored siblings in current path: "
              << remaining_at_level << std::endl;

    // DFS: Try each of the equally-scoring best moves
    for (size_t i = 0; i < best_moves.size(); i++) {
        const Move& move = best_moves[i];

        // Update exploration stack for this branch
        exploration_stack_.push_back({static_cast<int>(i), static_cast<int>(best_moves.size())});

        // Log adding move
        std::cout << "[Node " << nodes_explored_ << ", Branch " << (i+1) << "/"
                  << best_moves.size() << "] Adding move: "
                  << move.toString() << " for " << move.getScore() << " points" << std::endl;

        // Draw exact tiles needed for this move and apply it
        std::vector<char> tiles_drawn = applyMoveWithExactTiles(move, all_tiles);

        // Recurse to next depth
        dfsExploreGameTree(depth + 1);

        // Pop from exploration stack
        exploration_stack_.pop_back();

        // Log removing move
        std::cout << "[Node " << nodes_explored_ << ", Branch " << (i+1) << "/"
                  << best_moves.size() << "] Removing move: "
                  << move.toString() << " for " << move.getScore() << " points" << std::endl;

        // Backtrack: undo the move and return tiles to bag
        game_state_.undoLastMove();

        // Return the tiles that were in the rack back to the bag
        game_state_.getTileBag().returnTiles(game_state_.getRack().getTiles());
        game_state_.getRack().clear();
    }
}

std::vector<char> TopEverytimeFinder::fillRackWithAllTiles() {
    std::vector<char> drawn_tiles;

    // Draw all tiles from bag into a string (bypass rack size limitation)
    std::string all_tiles_str;
    while (game_state_.getTileBag().remainingCount() > 0) {
        char tile = game_state_.getTileBag().drawTile();
        all_tiles_str += tile;
        drawn_tiles.push_back(tile);
    }
    std::sort(all_tiles_str.begin(), all_tiles_str.end());

    // Set the rack directly with all tiles (bypassing addTile's size check)
    game_state_.getRack().setTiles(all_tiles_str);

    return drawn_tiles;
}

void TopEverytimeFinder::returnAllTilesToBag(const std::vector<char>& tiles) {
    // Clear rack
    game_state_.getRack().clear();

    // Return all tiles to bag
    for (char tile : tiles) {
        game_state_.getTileBag().returnTiles(std::string(1, tile));
    }
}

std::vector<char> TopEverytimeFinder::applyMoveWithExactTiles(const Move& move, const std::vector<char>& all_tiles) {
    // Figure out which tiles we need from the rack
    std::vector<char> needed_tiles;
    for (const auto& placement : move.getPlacements()) {
        if (placement.is_from_rack) {
            needed_tiles.push_back(placement.letter);
        }
    }

    // Draw exactly these tiles from the bag
    game_state_.getRack().clear();
    std::vector<char> drawn_tiles;
    for (char needed : needed_tiles) {
        // Find if we need a blank for this letter
        bool use_blank = false;
        char tile_in_bag = needed;

        // Check if this letter is available in bag
        if (!game_state_.getTileBag().canDrawTilesWithoutJoker(std::string(1, needed))) {
            // Need to use a blank
            use_blank = true;
            tile_in_bag = '?';  // Blank tile
        }

        char drawn = game_state_.getTileBag().drawTile(tile_in_bag);
        game_state_.getRack().addTile(drawn);
        drawn_tiles.push_back(drawn);
    }

    // Apply the move
    game_state_.applyMove(move);

    return drawn_tiles;
}

void TopEverytimeFinder::logGame(int game_id) {
    // Create filename with game ID and score
    std::ostringstream filename;
    filename << output_dir_ << "/game_"
             << std::setfill('0') << std::setw(6) << game_id
             << "_score_" << game_state_.getTotalScore() << ".txt";

    std::ofstream outfile(filename.str());
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file " << filename.str() << std::endl;
        return;
    }

    // Write game summary
    outfile << "=== Game #" << game_id << " ===" << std::endl;
    outfile << "Total Score: " << game_state_.getTotalScore() << std::endl;
    outfile << "Move Count: " << game_state_.getMoveCount() << std::endl;
    outfile << "Bingo Count: " << game_state_.getBingoCount() << std::endl;
    outfile << std::endl;

    // Write final board
    outfile << "Final Board:" << std::endl;
    outfile << game_state_.getBoard().toString() << std::endl;
    outfile << std::endl;

    // Write move history
    outfile << "Move History:" << std::endl;
    const auto& moves = game_state_.getMoveHistory();
    for (size_t i = 0; i < moves.size(); i++) {
        outfile << "Move " << (i + 1) << ": " << moves[i].toString() << std::endl;
    }

    outfile.close();
}

bool TopEverytimeFinder::isGameOver() {
    // Game is over if no vowels or no consonants in bag
    if (game_state_.getTileBag().vowelCount() == 0 ||
        game_state_.getTileBag().consonantCount() == 0) {
        return true;
    }

    return false;
}

}  // namespace scradle
