#include "move_generator.h"
#include <algorithm>
#include <cctype>

using std::vector;
using std::set;
using std::string;
using std::shared_ptr;

namespace scradle {

MoveGenerator::MoveGenerator(const Board& board, const Rack& rack, const DAWG& dawg)
    : board_(board), rack_(rack), dawg_(dawg) {}

vector<Move> MoveGenerator::generateMoves() {
    vector<Move> all_moves;

    // Generate moves in both directions
    vector<Move> h_moves = generateMovesHorizontal();
    vector<Move> v_moves = generateMovesVertical();

    all_moves.insert(all_moves.end(), h_moves.begin(), h_moves.end());
    all_moves.insert(all_moves.end(), v_moves.begin(), v_moves.end());

    return all_moves;
}

vector<Move> MoveGenerator::generateMovesHorizontal() {
    vector<Move> moves;
    vector<Anchor> anchors = findAnchors(Direction::HORIZONTAL);

    for (const auto& anchor : anchors) {
        extendLeft(anchor, Direction::HORIZONTAL, moves);
    }

    return moves;
}

vector<Move> MoveGenerator::generateMovesVertical() {
    vector<Move> moves;
    vector<Anchor> anchors = findAnchors(Direction::VERTICAL);

    for (const auto& anchor : anchors) {
        extendLeft(anchor, Direction::VERTICAL, moves);
    }

    return moves;
}

vector<Anchor> MoveGenerator::findAnchors(Direction dir) {
    vector<Anchor> anchors;

    // On an empty board, only the center is an anchor
    if (board_.isBoardEmpty()) {
        Anchor center(Board::CENTER, Board::CENTER);
        center.max_left_extension = Board::CENTER;
        // On empty board, all letters are valid
        for (char c = 'A'; c <= 'Z'; ++c) {
            center.cross_checks.insert(c);
        }
        anchors.push_back(center);
        return anchors;
    }

    // Find all anchors on the board
    for (int row = 0; row < Board::SIZE; ++row) {
        for (int col = 0; col < Board::SIZE; ++col) {
            if (isAnchor(row, col)) {
                Anchor anchor(row, col);

                // Compute cross-checks
                anchor.cross_checks = computeCrossChecks(row, col, dir);

                // Compute max left extension
                int ext_row = row, ext_col = col;
                getPrev(ext_row, ext_col, dir);

                anchor.max_left_extension = 0;
                while (board_.isValidPosition(ext_row, ext_col) &&
                       board_.isEmpty(ext_row, ext_col)) {
                    anchor.max_left_extension++;

                    // Stop if we hit an adjacent tile perpendicular to direction
                    int check_row = ext_row, check_col = ext_col;
                    if (dir == Direction::HORIZONTAL) {
                        if ((check_row > 0 && !board_.isEmpty(check_row - 1, check_col)) ||
                            (check_row < Board::SIZE - 1 && !board_.isEmpty(check_row + 1, check_col))) {
                            break;
                        }
                    } else {
                        if ((check_col > 0 && !board_.isEmpty(check_row, check_col - 1)) ||
                            (check_col < Board::SIZE - 1 && !board_.isEmpty(check_row, check_col + 1))) {
                            break;
                        }
                    }

                    getPrev(ext_row, ext_col, dir);
                }

                anchors.push_back(anchor);
            }
        }
    }

    return anchors;
}

bool MoveGenerator::isAnchor(int row, int col) const {
    // A square is an anchor if:
    // 1. It's empty
    if (!board_.isEmpty(row, col)) {
        return false;
    }

    // 2. It's adjacent to a filled square (orthogonally)
    // Check all four directions
    if (row > 0 && !board_.isEmpty(row - 1, col)) return true;
    if (row < Board::SIZE - 1 && !board_.isEmpty(row + 1, col)) return true;
    if (col > 0 && !board_.isEmpty(row, col - 1)) return true;
    if (col < Board::SIZE - 1 && !board_.isEmpty(row, col + 1)) return true;

    return false;
}

set<char> MoveGenerator::computeCrossChecks(int row, int col, Direction main_dir) {
    set<char> checks;

    // Get the cross-word perpendicular to the main direction
    string cross_word = getCrossWord(row, col, main_dir);

    // If no cross-word exists, all letters are valid
    if (cross_word.empty()) {
        for (char c = 'A'; c <= 'Z'; ++c) {
            checks.insert(c);
        }
        return checks;
    }

    // Find which letters form valid words when placed at the anchor
    for (char c = 'A'; c <= 'Z'; ++c) {
        // Replace the placeholder in cross_word with the test letter
        string test_word = cross_word;

        // Find the placeholder position (where the anchor is in the cross_word)
        size_t placeholder_pos = test_word.find('_');
        if (placeholder_pos == string::npos) {
            // This shouldn't happen, but handle it gracefully
            continue;
        }

        // Replace the placeholder with the test letter
        test_word[placeholder_pos] = c;

        if (dawg_.contains(test_word)) {
            checks.insert(c);
        }
    }

    return checks;
}

string MoveGenerator::getCrossWord(int row, int col, Direction main_dir) {
    string word;

    if (main_dir == Direction::HORIZONTAL) {
        // Build vertical word through this position
        int start_row = row;
        while (start_row > 0 && !board_.isEmpty(start_row - 1, col)) {
            start_row--;
        }

        int end_row = row;
        while (end_row < Board::SIZE - 1 && !board_.isEmpty(end_row + 1, col)) {
            end_row++;
        }

        // If there are no adjacent letters, no cross-word
        if (start_row == row && end_row == row) {
            return "";
        }

        for (int r = start_row; r <= end_row; ++r) {
            if (r == row) {
                word += '_';  // Placeholder for the letter we'll insert
            } else {
                word += board_.getLetter(r, col);
            }
        }
    } else {
        // Build horizontal word through this position
        int start_col = col;
        while (start_col > 0 && !board_.isEmpty(row, start_col - 1)) {
            start_col--;
        }

        int end_col = col;
        while (end_col < Board::SIZE - 1 && !board_.isEmpty(row, end_col + 1)) {
            end_col++;
        }

        // If there are no adjacent letters, no cross-word
        if (start_col == col && end_col == col) {
            return "";
        }

        for (int c = start_col; c <= end_col; ++c) {
            if (c == col) {
                word += '_';  // Placeholder for the letter we'll insert
            } else {
                word += board_.getLetter(row, c);
            }
        }
    }

    return word;
}

void MoveGenerator::extendLeft(const Anchor& anchor, Direction dir, vector<Move>& moves) {
    // If there's a tile to the left/above of anchor, we must start from there
    int prev_row = anchor.row, prev_col = anchor.col;
    getPrev(prev_row, prev_col, dir);

    if (board_.isValidPosition(prev_row, prev_col) && !board_.isEmpty(prev_row, prev_col)) {
        // There are existing tiles before the anchor - we must include them
        // Find the actual start of the word
        int start_row = anchor.row, start_col = anchor.col;
        while (true) {
            int test_row = start_row, test_col = start_col;
            getPrev(test_row, test_col, dir);
            if (!board_.isValidPosition(test_row, test_col) || board_.isEmpty(test_row, test_col)) {
                break;
            }
            start_row = test_row;
            start_col = test_col;
        }

        // Build the prefix from existing tiles and traverse the DAWG
        string prefix;
        shared_ptr<DAWG::Node> node = dawg_.getRoot();
        int curr_row = start_row, curr_col = start_col;

        while ((curr_row != anchor.row || curr_col != anchor.col)) {
            char letter = board_.getLetter(curr_row, curr_col);
            char upper_letter = std::toupper(static_cast<unsigned char>(letter));
            prefix += upper_letter;

            auto it = node->children.find(upper_letter);
            if (it == node->children.end()) {
                // Existing tiles don't form a valid prefix - no moves possible
                return;
            }
            node = it->second;

            getNext(curr_row, curr_col, dir);
        }

        // Now continue from the anchor position with the DAWG node reached so far
        extendRight(node, prefix, anchor.row, anchor.col, dir, rack_, moves, false);
    } else {
        // No existing tiles before anchor
        // Try starting at the anchor
        extendRight(dawg_.getRoot(), "", anchor.row, anchor.col, dir, rack_, moves, false);

        // Also try starting before the anchor (left extension)
        // This allows moves like QI where Q is placed before the anchor
        for (int ext = 1; ext <= anchor.max_left_extension; ++ext) {
            int start_row = anchor.row, start_col = anchor.col;
            for (int i = 0; i < ext; ++i) {
                getPrev(start_row, start_col, dir);
            }
            if (!board_.isValidPosition(start_row, start_col)) {
                break;
            }
            extendRight(dawg_.getRoot(), "", start_row, start_col, dir, rack_, moves, false);
        }
    }
}

void MoveGenerator::extendRight(
    const shared_ptr<DAWG::Node>& node,
    const string& partial_word,
    int row, int col,
    Direction dir,
    Rack temp_rack,
    vector<Move>& moves,
    bool anchor_placed,
    vector<int> blank_positions
) {
    // Base case: position out of bounds
    if (!board_.isValidPosition(row, col)) {
        return;
    }

    // If this position has a tile, we must use it
    if (!board_.isEmpty(row, col)) {
        char letter = board_.getLetter(row, col);

        // Get uppercase version for DAWG traversal (in case it's a blank on board)
        char upper_letter = std::toupper(static_cast<unsigned char>(letter));

        auto it = node->children.find(upper_letter);
        if (it == node->children.end()) {
            return;  // This letter doesn't continue any valid word
        }

        string new_word = partial_word + upper_letter;
        int next_row = row, next_col = col;
        getNext(next_row, next_col, dir);

        extendRight(it->second, new_word, next_row, next_col, dir, temp_rack, moves, true, blank_positions);
        return;
    }

    // If we've placed the anchor and the current word is valid, record the move
    if (anchor_placed && !partial_word.empty() && node->is_end_of_word) {
        int start_row = row, start_col = col;
        for (size_t i = 0; i < partial_word.length(); ++i) {
            getPrev(start_row, start_col, dir);
        }

        Move move(start_row, start_col, dir, partial_word);

        // Add TilePlacement objects for each tile in the word
        int tile_row = start_row, tile_col = start_col;
        for (size_t i = 0; i < partial_word.length(); ++i) {
            char letter = partial_word[i];
            bool is_from_rack = board_.isEmpty(tile_row, tile_col);

            // Check if this position is a blank (either newly placed or already on board)
            bool is_blank = false;
            if (is_from_rack) {
                // New tile - check blank_positions
                is_blank = std::find(blank_positions.begin(), blank_positions.end(), static_cast<int>(i)) != blank_positions.end();
            } else {
                // Existing tile on board - check if it's lowercase (indicates blank)
                char board_letter = board_.getLetter(tile_row, tile_col);
                is_blank = (board_letter >= 'a' && board_letter <= 'z');
            }

            move.addPlacement(TilePlacement(tile_row, tile_col, letter, is_from_rack, is_blank));
            getNext(tile_row, tile_col, dir);
        }

        moves.push_back(move);
    }

    // Compute cross-checks for this empty position
    set<char> cross_checks = computeCrossChecks(row, col, dir);

    // Try placing each available tile from the rack
    for (int i = 0; i < temp_rack.size(); ++i) {
        char tile = temp_rack.getTile(i);

        // If this is a blank tile, try all possible letters
        if (tile == '?') {
            for (char letter = 'A'; letter <= 'Z'; ++letter) {
                auto it = node->children.find(letter);
                if (it == node->children.end()) {
                    continue;
                }

                // Check if this letter passes cross-check validation
                if (cross_checks.find(letter) == cross_checks.end()) {
                    continue;
                }

                // Create new rack without the blank
                Rack new_rack = temp_rack;
                new_rack.removeTile('?');

                string new_word = partial_word + letter;

                // Track this position as a blank
                vector<int> new_blank_positions = blank_positions;
                new_blank_positions.push_back(static_cast<int>(partial_word.length()));

                int next_row = row, next_col = col;
                getNext(next_row, next_col, dir);

                extendRight(it->second, new_word, next_row, next_col, dir, new_rack, moves, true, new_blank_positions);
            }
        } else {
            // Regular tile
            auto it = node->children.find(tile);
            if (it == node->children.end()) {
                continue;
            }

            // Check if this letter passes cross-check validation
            if (cross_checks.find(tile) == cross_checks.end()) {
                continue;
            }

            // Create new rack without this tile
            Rack new_rack = temp_rack;
            new_rack.removeTile(tile);

            string new_word = partial_word + tile;
            int next_row = row, next_col = col;
            getNext(next_row, next_col, dir);

            extendRight(it->second, new_word, next_row, next_col, dir, new_rack, moves, true, blank_positions);
        }
    }
}

void MoveGenerator::getNext(int& row, int& col, Direction dir) const {
    if (dir == Direction::HORIZONTAL) {
        col++;
    } else {
        row++;
    }
}

void MoveGenerator::getPrev(int& row, int& col, Direction dir) const {
    if (dir == Direction::HORIZONTAL) {
        col--;
    } else {
        row--;
    }
}

} // namespace scradle
