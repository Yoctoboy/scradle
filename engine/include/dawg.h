#ifndef SCRADLE_DAWG_H
#define SCRADLE_DAWG_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace scradle {

// DAWG (Directed Acyclic Word Graph) for efficient word storage and lookup
// A trie-like structure optimized for space by sharing common suffixes
class DAWG {
public:
    DAWG();
    ~DAWG();

    // Build DAWG from a list of words
    void build(const std::vector<std::string>& words);

    // Load DAWG from a text file (one word per line)
    bool loadFromFile(const std::string& filename);

    // Query operations
    bool contains(const std::string& word) const;
    bool hasPrefix(const std::string& prefix) const;

    // Get all words with a given prefix
    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) const;

    // Statistics
    int getWordCount() const { return word_count_; }
    int getNodeCount() const { return node_count_; }

    // Clear the DAWG
    void clear();

    // Node structure (public for move generator)
    struct Node {
        std::unordered_map<char, std::shared_ptr<Node>> children;
        bool is_end_of_word;

        Node() : is_end_of_word(false) {}
    };

    // Get root node for traversal (needed by move generator)
    std::shared_ptr<Node> getRoot() const { return root_; }

    // Navigate to node at prefix (returns nullptr if prefix not found)
    std::shared_ptr<Node> getNodeAt(const std::string& prefix) const;

private:

    std::shared_ptr<Node> root_;
    int word_count_;
    int node_count_;

    // Helper functions
    void insertWord(const std::string& word);
    void collectWords(const std::shared_ptr<Node>& node,
                     const std::string& prefix,
                     std::vector<std::string>& results) const;
};

} // namespace scradle

#endif // SCRADLE_DAWG_H
