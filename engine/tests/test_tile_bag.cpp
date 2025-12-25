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
    assert_equal(47, vowels, "Should have 47 vowels initially");
    assert_equal(57, consonants, "Should have 46 consonants initially");

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

void test_tile_bag_draw_specific_letter() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Draw Specific Letter ===" << color::RESET << endl;

    TileBag bag(123);

    // Draw a specific letter that exists
    char drawn = bag.drawTile('E');
    assert_equal('E', drawn, "Should draw an E");
    assert_equal(101, bag.remainingCount(), "Should have 101 tiles left");

    // Draw another E
    char drawn2 = bag.drawTile('E');
    assert_equal('E', drawn2, "Should draw another E");
    assert_equal(100, bag.remainingCount(), "Should have 100 tiles left");
}

void test_tile_bag_draw_specific_letter_with_joker_fallback() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: TileBag Draw Specific Letter with Joker Fallback ===" << color::RESET << endl;

    TileBag bag(456);

    // Draw all Z's (there's only 1)
    char z1 = bag.drawTile('Z');
    assert_equal('Z', z1, "Should draw the Z");
    assert_equal(101, bag.remainingCount(), "Should have 101 tiles left");

    // Try to draw another Z - should get a joker instead
    char z2 = bag.drawTile('Z');
    assert_equal('?', z2, "Should draw a joker when Z is not available");
    assert_equal(100, bag.remainingCount(), "Should have 100 tiles left");

    // Try to draw another Z - should get the second joker
    char z3 = bag.drawTile('Z');
    assert_equal('?', z3, "Should draw the second joker when Z is still not available");
    assert_equal(99, bag.remainingCount(), "Should have 99 tiles left");

    // Try to draw another Z - should fail now
    char z4 = bag.drawTile('Z');
    assert_equal('\0', z4, "Should return null when neither Z nor jokers are available");
    assert_equal(99, bag.remainingCount(), "Should still have 99 tiles left");
}

void test_can_draw_tiles_basic() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: canDrawTiles Basic ===" << color::RESET << endl;

    TileBag bag(789);

    // Test with available letters
    assert_true(bag.canDrawTiles("HELLO"), "Should be able to draw HELLO");
    assert_true(bag.canDrawTiles("E"), "Should be able to draw single E");
    assert_true(bag.canDrawTiles(""), "Should be able to draw empty string");

    // Test with letters that exist in the distribution
    assert_true(bag.canDrawTiles("AEIOU"), "Should be able to draw common vowels");
    assert_true(bag.canDrawTiles("SCRABBLE"), "Should be able to draw SCRABBLE");
}

void test_can_draw_tiles_with_joker_fallback() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: canDrawTiles with Joker Fallback ===" << color::RESET << endl;

    TileBag bag(321);

    // Draw all Z's (there's only 1)
    bag.drawTile('Z');

    // Should still be able to "draw" Z by using a joker
    assert_true(bag.canDrawTiles("Z"), "Should be able to draw Z using a joker");

    // Should be able to draw 2 Z's using the 2 jokers
    assert_true(bag.canDrawTiles("ZZ"), "Should be able to draw ZZ using both jokers");

    // Cannot draw 3 Z's (1 gone, only 2 jokers available)
    assert_false(bag.canDrawTiles("ZZZ"), "Should not be able to draw ZZZ (need 3, have 0 Z + 2 jokers)");
}

void test_can_draw_tiles_insufficient_letters() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: canDrawTiles Insufficient Letters ===" << color::RESET << endl;

    TileBag bag(654);

    // Draw many E's (there are 15 total)
    for (int i = 0; i < 15; ++i) {
        bag.drawTile('E');
    }

    // Now E's are exhausted, but we have 2 jokers
    assert_true(bag.canDrawTiles("E"), "Should be able to draw 1 E using joker");
    assert_true(bag.canDrawTiles("EE"), "Should be able to draw 2 E's using both jokers");
    assert_false(bag.canDrawTiles("EEE"), "Should not be able to draw 3 E's (need 3, have 0 E + 2 jokers)");
}

void test_can_draw_tiles_multiple_of_same_letter() {
    cout << "\n"
         << color::BLUE << color::BOLD << "=== Test: canDrawTiles Multiple of Same Letter ===" << color::RESET << endl;

    TileBag bag(987);

    // Test drawing multiple of the same letter
    assert_true(bag.canDrawTiles("EEEE"), "Should be able to draw 4 E's (have 15)");
    assert_true(bag.canDrawTiles("AAAAAAAAA"), "Should be able to draw 9 A's (have exactly 9)");

    // This should fail - there are only 9 A's and 2 jokers
    assert_false(bag.canDrawTiles("AAAAAAAAAAAA"), "Should not be able to draw 12 A's (have 9 A + 2 jokers)");

    // But 11 A's should work (9 real + 2 jokers)
    assert_true(bag.canDrawTiles("AAAAAAAAAAA"), "Should be able to draw 11 A's (9 A + 2 jokers)");
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
