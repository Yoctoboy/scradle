#include "dawg.h"
#include "test_framework.h"
#include <iostream>

using namespace scradle;
using namespace test;
using std::cout;
using std::endl;

void test_dawg_creation() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Creation ===" << color::RESET << endl;

    DAWG dawg;

    assert_equal(0, dawg.getWordCount(), "Empty DAWG should have 0 words");
    assert_equal(1, dawg.getNodeCount(), "Empty DAWG should have 1 node (root)");
}

void test_dawg_insert() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Insert ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"CAT", "CAR", "CARD", "CARE", "CAREFUL", "DOG", "DODGE"};

    dawg.build(words);

    assert_equal(7, dawg.getWordCount(), "DAWG should have 7 words");
    assert_true(dawg.getNodeCount() > 7, "DAWG should have more than 7 nodes");
}

void test_dawg_contains() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Contains ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"CHAT", "CHIEN", "MAISON", "PORTE"};

    dawg.build(words);

    assert_true(dawg.contains("CHAT"), "DAWG should contain 'CHAT'");
    assert_true(dawg.contains("chat"), "DAWG should contain 'chat' (case insensitive)");
    assert_true(dawg.contains("CHIEN"), "DAWG should contain 'CHIEN'");
    assert_true(dawg.contains("MAISON"), "DAWG should contain 'MAISON'");
    assert_true(dawg.contains("PORTE"), "DAWG should contain 'PORTE'");

    assert_true(!dawg.contains("CHA"), "DAWG should not contain 'CHA' (prefix only)");
    assert_true(!dawg.contains("CHATS"), "DAWG should not contain 'CHATS' (not added)");
    assert_true(!dawg.contains("VOITURE"), "DAWG should not contain 'VOITURE' (not added)");
}

void test_dawg_prefix() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Prefix ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"CHAT", "CHIEN", "CHEVAL", "MAISON", "PORTE"};

    dawg.build(words);

    assert_true(dawg.hasPrefix("CH"), "DAWG should have prefix 'CH'");
    assert_true(dawg.hasPrefix("CHA"), "DAWG should have prefix 'CHA'");
    assert_true(dawg.hasPrefix("CHAT"), "DAWG should have prefix 'CHAT' (complete word)");
    assert_true(dawg.hasPrefix("CHE"), "DAWG should have prefix 'CHE'");
    assert_true(dawg.hasPrefix("M"), "DAWG should have prefix 'M'");
    assert_true(dawg.hasPrefix("MA"), "DAWG should have prefix 'MA'");

    assert_true(!dawg.hasPrefix("CHO"), "DAWG should not have prefix 'CHO'");
    assert_true(!dawg.hasPrefix("VOITURE"), "DAWG should not have prefix 'VOITURE'");
    assert_true(!dawg.hasPrefix("Z"), "DAWG should not have prefix 'Z'");
}

void test_dawg_get_words_with_prefix() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Get Words With Prefix ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"CHAT", "CHIEN", "CHEVAL", "CHER", "MAISON", "PORTE"};

    dawg.build(words);

    auto ch_words = dawg.getWordsWithPrefix("CH");
    assert_equal(4, (int)ch_words.size(), "Should find 4 words starting with 'CH'");

    auto che_words = dawg.getWordsWithPrefix("CHE");
    assert_equal(2, (int)che_words.size(), "Should find 2 words starting with 'CHE'");

    auto chat_words = dawg.getWordsWithPrefix("CHAT");
    assert_equal(1, (int)chat_words.size(), "Should find 1 word starting with 'CHAT'");

    auto no_words = dawg.getWordsWithPrefix("Z");
    assert_equal(0, (int)no_words.size(), "Should find 0 words starting with 'Z'");
}

void test_dawg_load_from_file() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Load ODS8 Dictionary ===" << color::RESET << endl;

    DAWG dawg;
    bool loaded = dawg.loadFromFile("engine/dictionnaries/ods8_complete.txt");

    assert_true(loaded, "DAWG should load successfully from ODS8 file");
    assert_equal(411430, dawg.getWordCount(), "ODS8 should contain 411,430 words");

    // Test common French words
    assert_true(dawg.contains("CHAT"), "DAWG should contain 'CHAT'");
    assert_true(dawg.contains("MAISON"), "DAWG should contain 'MAISON'");
    assert_true(dawg.contains("BONJOUR"), "DAWG should contain 'BONJOUR'");
    assert_true(dawg.contains("SCRABBLE"), "DAWG should contain 'SCRABBLE'");

    // Test 2-letter words
    assert_true(dawg.contains("AA"), "DAWG should contain 'AA'");
    assert_true(dawg.contains("DE"), "DAWG should contain 'DE'");

    // Test long words
    assert_false(dawg.contains("VOITUR"), "DAWG should not contain 'VOITUR'");
    assert_false(dawg.contains("DESHYPOTHEQUIE"), "DAWG should not contain 'DESHYPOTHEQUIE'");
    assert_true(dawg.contains("DESHYPOTHEQUIEZ"), "DAWG should contain 'DESHYPOTHEQUIEZ'");
}

void test_dawg_case_insensitive() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Case Insensitive ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"Chat", "CHIEN", "MaIsOn"};

    dawg.build(words);

    assert_true(dawg.contains("CHAT"), "Should find 'CHAT' (uppercase)");
    assert_true(dawg.contains("chat"), "Should find 'chat' (lowercase)");
    assert_true(dawg.contains("Chat"), "Should find 'Chat' (mixed case)");
    assert_true(dawg.contains("CHIEN"), "Should find 'CHIEN'");
    assert_true(dawg.contains("chien"), "Should find 'chien'");
    assert_true(dawg.contains("MAISON"), "Should find 'MAISON'");
    assert_true(dawg.contains("maison"), "Should find 'maison'");
}

void test_dawg_clear() {
    cout << "\n" << color::BLUE << color::BOLD << "=== Test: DAWG Clear ===" << color::RESET << endl;

    DAWG dawg;
    std::vector<std::string> words = {"CHAT", "CHIEN", "MAISON"};

    dawg.build(words);
    assert_equal(3, dawg.getWordCount(), "DAWG should have 3 words before clear");

    dawg.clear();
    assert_equal(0, dawg.getWordCount(), "DAWG should have 0 words after clear");
    assert_equal(1, dawg.getNodeCount(), "DAWG should have 1 node after clear (root)");
    assert_true(!dawg.contains("CHAT"), "DAWG should not contain 'CHAT' after clear");
}

int main() {
    cout << color::MAGENTA << color::BOLD << "=== Scradle Engine - DAWG Tests ===" << color::RESET << endl;

    test_dawg_creation();
    test_dawg_insert();
    test_dawg_contains();
    test_dawg_prefix();
    test_dawg_get_words_with_prefix();
    test_dawg_load_from_file();
    test_dawg_case_insensitive();
    test_dawg_clear();

    print_summary();

    return exit_code();
}
