#include <iostream>
#include <unordered_map>

#include "test_framework.h"
#include "tile_bag.h"

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_tile_bag_initial_distribution() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Initial Distribution ===" << color::RESET << endl;

    TileBag bag(12345);

    // Verify total count
    assert_equal(102, bag.remainingCount(), "Should have 100 tiles initially");

    // Draw all tiles and count distribution
    std::unordered_map<char, int> distribution;
    for (int i = 0; i < 102; ++i) {
        char tile = bag.drawTile();
        distribution[tile]++;
    }

    // Verify French Scrabble distribution
    assert_equal(9, distribution['A'], "Should have 15 A's");
    assert_equal(15, distribution['E'], "Should have 15 E's");
    assert_equal(8, distribution['I'], "Should have 8 I's");
    assert_equal(6, distribution['O'], "Should have 6 O's");
    assert_equal(6, distribution['U'], "Should have 6 U's");
    assert_equal(1, distribution['Y'], "Should have 1 Y");

    assert_equal(2, distribution['B'], "Should have 2 B's");
    assert_equal(2, distribution['C'], "Should have 2 C's");
    assert_equal(3, distribution['D'], "Should have 3 D's");
    assert_equal(2, distribution['F'], "Should have 2 F's");
    assert_equal(2, distribution['G'], "Should have 2 G's");
    assert_equal(2, distribution['H'], "Should have 2 H's");
    assert_equal(1, distribution['J'], "Should have 1 J");
    assert_equal(1, distribution['K'], "Should have 1 K");
    assert_equal(5, distribution['L'], "Should have 5 L's");
    assert_equal(3, distribution['M'], "Should have 3 M's");
    assert_equal(6, distribution['N'], "Should have 6 N's");
    assert_equal(2, distribution['P'], "Should have 2 P's");
    assert_equal(1, distribution['Q'], "Should have 1 Q");
    assert_equal(6, distribution['R'], "Should have 6 R's");
    assert_equal(6, distribution['S'], "Should have 6 S's");
    assert_equal(6, distribution['T'], "Should have 6 T's");
    assert_equal(2, distribution['V'], "Should have 2 V's");
    assert_equal(1, distribution['W'], "Should have 1 W");
    assert_equal(1, distribution['X'], "Should have 1 X");
    assert_equal(1, distribution['Z'], "Should have 1 Z");
    assert_equal(2, distribution['?'], "Should have 2 blanks");

    assert_true(bag.isEmpty(), "Bag should be empty after drawing all tiles");
}

void test_tile_bag_seeded_randomness() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Seeded Randomness ===" << color::RESET << endl;

    TileBag bag1(12345);
    TileBag bag2(12345);

    // Draw 20 tiles from each
    std::string tiles1 = bag1.drawTiles(20);
    std::string tiles2 = bag2.drawTiles(20);

    assert_equal(tiles1, tiles2, "Same seed should produce identical draws");
    assert_equal(20, (int)tiles1.length(), "Should draw 20 tiles");
}

void test_tile_bag_draw_and_return() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Draw and Return ===" << color::RESET << endl;

    TileBag bag(42);

    int initial_count = bag.remainingCount();
    assert_equal(102, initial_count, "Should start with 100 tiles");

    std::string drawn = bag.drawTiles(7);
    assert_equal(7, (int)drawn.length(), "Should draw 7 tiles");
    assert_equal(95, bag.remainingCount(), "Should have 93 tiles left");

    bag.returnTiles(drawn);
    assert_equal(102, bag.remainingCount(), "Should have 100 tiles after return");
}

void test_tile_bag_vowel_consonant_counting() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Vowel/Consonant Counting ===" << color::RESET << endl;

    TileBag bag(999);

    // Count vowels and consonants in full bag
    int vowels = bag.vowelCount();
    int consonants = bag.consonantCount();

    // French Scrabble: 51 vowels (15A + 15E + 8I + 6O + 6U + 1Y) + 2 blanks + 46 consonants = 100
    assert_equal(45, vowels, "Should have 51 vowels initially");
    assert_equal(55, consonants, "Should have 46 consonants initially");
    assert_equal(102, vowels + consonants + 2, "Vowels + consonants + blanks should equal 100");

    assert_true(bag.hasVowels(), "Should have vowels");
    assert_true(bag.hasConsonants(), "Should have consonants");
}

void test_tile_bag_draw_partial() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Draw Partial ===" << color::RESET << endl;

    TileBag bag(777);

    // Draw most tiles, leaving only a few
    bag.drawTiles(97);
    assert_equal(5, bag.remainingCount(), "Should have 3 tiles left");

    // Try to draw more than available
    std::string drawn = bag.drawTiles(10);
    assert_equal(5, (int)drawn.length(), "Should only draw 3 tiles (all remaining)");
    assert_true(bag.isEmpty(), "Bag should now be empty");
}

void test_tile_bag_reset() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Reset ===" << color::RESET << endl;

    TileBag bag(555);

    std::string first_draw = bag.drawTiles(10);
    assert_equal(92, bag.remainingCount(), "Should have 90 tiles after first draw");

    bag.reset();
    assert_equal(102, bag.remainingCount(), "Should have 100 tiles after reset");

    std::string second_draw = bag.drawTiles(10);
    assert_equal(first_draw, second_draw, "Reset should reproduce same sequence");
}

void test_tile_bag_different_seeds() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Different Seeds ===" << color::RESET << endl;

    TileBag bag1(111);
    TileBag bag2(222);

    std::string tiles1 = bag1.drawTiles(20);
    std::string tiles2 = bag2.drawTiles(20);

    assert_true(tiles1 != tiles2, "Different seeds should produce different draws");
}

int main() {
    cout << "=== Tile Bag Tests ===" << endl;

    test_tile_bag_initial_distribution();
    test_tile_bag_seeded_randomness();
    test_tile_bag_draw_and_return();
    test_tile_bag_vowel_consonant_counting();
    test_tile_bag_draw_partial();
    test_tile_bag_reset();
    test_tile_bag_different_seeds();

    print_summary();
    return exit_code();
}
