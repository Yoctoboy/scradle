#include "dawg.h"
#include <fstream>
#include <algorithm>
#include <cctype>

using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::ifstream;

namespace scradle {

DAWG::DAWG() : root_(make_shared<Node>()), word_count_(0), node_count_(1) {}

DAWG::~DAWG() {
    clear();
}

void DAWG::clear() {
    root_ = make_shared<Node>();
    word_count_ = 0;
    node_count_ = 1;
}

void DAWG::build(const vector<string>& words) {
    clear();
    for (const auto& word : words) {
        insertWord(word);
    }
}

void DAWG::insertWord(const string& word) {
    if (word.empty()) {
        return;
    }

    // Convert to uppercase
    string upper_word = word;
    for (char& c : upper_word) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    // Traverse/create nodes for each character
    shared_ptr<Node> current = root_;
    for (char c : upper_word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = make_shared<Node>();
            node_count_++;
        }
        current = current->children[c];
    }

    // Mark end of word
    if (!current->is_end_of_word) {
        current->is_end_of_word = true;
        word_count_++;
    }
}

bool DAWG::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    clear();

    string word;
    while (std::getline(file, word)) {
        // Trim whitespace
        word.erase(0, word.find_first_not_of(" \t\r\n"));
        word.erase(word.find_last_not_of(" \t\r\n") + 1);

        if (!word.empty() && word[0] != '#') {  // Skip comments
            insertWord(word);
        }
    }

    file.close();
    return true;
}

bool DAWG::contains(const string& word) const {
    if (word.empty()) {
        return false;
    }

    shared_ptr<Node> node = getNodeAt(word);
    return node != nullptr && node->is_end_of_word;
}

bool DAWG::hasPrefix(const string& prefix) const {
    if (prefix.empty()) {
        return true;
    }

    // Convert to uppercase
    string upper_prefix = prefix;
    for (char& c : upper_prefix) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    // Traverse the DAWG
    shared_ptr<Node> current = root_;
    for (char c : upper_prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return false;
        }
        current = it->second;
    }

    return true;
}

vector<string> DAWG::getWordsWithPrefix(const string& prefix) const {
    vector<string> results;

    // Convert to uppercase
    string upper_prefix = prefix;
    for (char& c : upper_prefix) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    // Find the node corresponding to the prefix
    shared_ptr<Node> current = root_;
    for (char c : upper_prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return results;  // Prefix not found
        }
        current = it->second;
    }

    // Collect all words from this node
    collectWords(current, upper_prefix, results);

    return results;
}

void DAWG::collectWords(const shared_ptr<Node>& node,
                        const string& prefix,
                        vector<string>& results) const {
    if (node->is_end_of_word) {
        results.push_back(prefix);
    }

    for (const auto& pair : node->children) {
        collectWords(pair.second, prefix + pair.first, results);
    }
}

shared_ptr<DAWG::Node> DAWG::getNodeAt(const string& prefix) const {
    if (prefix.empty()) {
        return root_;
    }

    // Convert to uppercase
    string upper_prefix = prefix;
    for (char& c : upper_prefix) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    // Traverse the DAWG
    shared_ptr<Node> current = root_;
    for (char c : upper_prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return nullptr;
        }
        current = it->second;
    }

    return current;
}

} // namespace scradle
