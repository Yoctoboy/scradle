#include <omp.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include "dawg.h"
#include "duplicate_game.h"

using namespace scradle;
using namespace std;

struct GameStats {
    int seed;
    int total_score;
    int move_count;
    int bingo_count;
    long long duration_ms;
};

int randomBetween1AndMax(int max) {
    static std::random_device rd;   // seed source
    static std::mt19937 gen(rd());  // Mersenne Twister engine
    std::uniform_int_distribution<int> dist(1, max);
    return dist(gen);
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int num_games = 10;
    int num_threads = 0;  // 0 means use OpenMP default (typically all cores)

    // Show usage if requested
    if (argc > 1 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")) {
        cout << "Usage: " << argv[0] << " [num_games] [num_threads]" << endl;
        cout << "  num_games:   Number of games to simulate (default: 10)" << endl;
        cout << "  num_threads: Number of parallel threads to use (default: all available cores)" << endl;
        cout << "\nExample:" << endl;
        cout << "  " << argv[0] << " 100 4    # Simulate 100 games using 4 threads" << endl;
        return 0;
    }

    if (argc > 1) {
        num_games = atoi(argv[1]);
        if (num_games <= 0) {
            cerr << "Invalid number of games: " << argv[1] << endl;
            return 1;
        }
    }

    if (argc > 2) {
        num_threads = atoi(argv[2]);
        if (num_threads <= 0) {
            cerr << "Invalid number of threads: " << argv[2] << endl;
            return 1;
        }
        omp_set_num_threads(num_threads);
    }

    cout << endl
         << "=== Duplicate Scrabble Game Simulator ===" << endl;

    // Display thread configuration
    int actual_threads = num_threads > 0 ? num_threads : omp_get_max_threads();
    cout << "Using " << actual_threads << " thread" << (actual_threads > 1 ? "s" : "") << endl;

    cout << "Loading dictionary..." << endl;

    // Load dictionary
    DAWG dawg;
    if (!dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt")) {
        cerr << "Failed to load dictionary" << endl;
        return 1;
    }

    cout << "Dictionary loaded: " << dawg.getWordCount() << " words" << endl;
    cout << "Simulating " << num_games << " games..." << endl
         << endl;

    // Run games and collect stats
    vector<GameStats> all_stats(num_games);

    // Generate seeds upfront to ensure reproducibility
    vector<unsigned int> seeds(num_games);
    for (int i = 0; i < num_games; i++) {
        seeds[i] = randomBetween1AndMax(1000000);
    }

    auto total_start = chrono::high_resolution_clock::now();

// Parallel game simulation
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < num_games; i++) {
        unsigned int seed = seeds[i];
#pragma omp critical
        {
            cout << "Starting Game " << (i + 1) << "/" << num_games << " with seed " << seed << endl;
        }

        auto game_start = chrono::high_resolution_clock::now();

        DuplicateGame game(dawg, seed);
        game.playGame(false);

        auto game_end = chrono::high_resolution_clock::now();
        auto game_duration = chrono::duration_cast<chrono::milliseconds>(game_end - game_start).count();

        GameStats stats;
        stats.seed = seed;
        stats.total_score = game.getState().getTotalScore();
        stats.move_count = game.getState().getMoveCount();
        stats.bingo_count = game.getState().getBingoCount();
        stats.duration_ms = game_duration;

        all_stats[i] = stats;

#pragma omp critical
        {
            cout << "Game " << (i + 1) << "/" << num_games
                 << " (seed " << seed << "): "
                 << stats.total_score << " pts, "
                 << stats.move_count << " moves, "
                 << stats.bingo_count << " bingos, "
                 << stats.duration_ms << " ms" << endl;
        }
    }

    auto total_end = chrono::high_resolution_clock::now();
    auto total_duration = chrono::duration_cast<chrono::milliseconds>(total_end - total_start).count();

    // Calculate statistics
    cout << "\n=== Statistics ===" << endl;
    cout << "Total games: " << num_games << endl;
    cout << "Total time: " << total_duration << " ms" << endl;
    cout << "Average time per game: " << (total_duration / num_games) << " ms" << endl
         << endl;

    // Score statistics
    vector<int> scores;
    for (const auto& stats : all_stats) {
        scores.push_back(stats.total_score);
    }
    sort(scores.begin(), scores.end());

    int min_score = scores.front();
    int max_score = scores.back();
    int median_score = scores[scores.size() / 2];
    double avg_score = 0;
    for (int score : scores) {
        avg_score += score;
    }
    avg_score /= scores.size();

    cout << "Score Statistics:" << endl;
    cout << "  Min:    " << min_score << endl;
    cout << "  Max:    " << max_score << endl;
    cout << "  Median: " << median_score << endl;
    cout << "  Average: " << fixed << setprecision(1) << avg_score << endl
         << endl;

    // Move count statistics
    vector<int> move_counts;
    for (const auto& stats : all_stats) {
        move_counts.push_back(stats.move_count);
    }
    sort(move_counts.begin(), move_counts.end());

    int min_moves = move_counts.front();
    int max_moves = move_counts.back();
    int median_moves = move_counts[move_counts.size() / 2];
    double avg_moves = 0;
    for (int moves : move_counts) {
        avg_moves += moves;
    }
    avg_moves /= move_counts.size();

    cout << "Move Count Statistics:" << endl;
    cout << "  Min:    " << min_moves << endl;
    cout << "  Max:    " << max_moves << endl;
    cout << "  Median: " << median_moves << endl;
    cout << "  Average: " << fixed << setprecision(1) << avg_moves << endl
         << endl;

    // Bingo statistics
    vector<int> bingo_counts;
    for (const auto& stats : all_stats) {
        bingo_counts.push_back(stats.bingo_count);
    }
    sort(bingo_counts.begin(), bingo_counts.end());

    int min_bingos = bingo_counts.front();
    int max_bingos = bingo_counts.back();
    int median_bingos = bingo_counts[bingo_counts.size() / 2];
    double avg_bingos = 0;
    for (int bingos : bingo_counts) {
        avg_bingos += bingos;
    }
    avg_bingos /= bingo_counts.size();

    cout << "Bingo Statistics:" << endl;
    cout << "  Min:    " << min_bingos << endl;
    cout << "  Max:    " << max_bingos << endl;
    cout << "  Median: " << median_bingos << endl;
    cout << "  Average: " << fixed << setprecision(2) << avg_bingos << endl
         << endl;

    // Top 5 games by score
    cout << "Top 5 Games by Score:" << endl;
    vector<GameStats> sorted_by_score = all_stats;
    sort(sorted_by_score.begin(), sorted_by_score.end(),
         [](const GameStats& a, const GameStats& b) { return a.total_score > b.total_score; });

    for (int i = 0; i < min(5, (int)sorted_by_score.size()); i++) {
        const auto& stats = sorted_by_score[i];
        cout << "  " << (i + 1) << ". Seed " << stats.seed << ": "
             << stats.total_score << " pts ("
             << stats.move_count << " moves, "
             << stats.bingo_count << " bingos)" << endl;
    }

    return 0;
}
