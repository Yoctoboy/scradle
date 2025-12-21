#include "move_generator.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <set>

#include "scorer.h"

using std::min;
using std::string;
using std::vector;

namespace scradle {

// ============================================================================
// Utility functions (defined early for use throughout)
// ============================================================================

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

// ============================================================================
// Constructor and main entry points
// ============================================================================

MoveGenerator::MoveGenerator(const Board& board, const Rack& rack, const DAWG& dawg)
    : board_(board), rack_(rack), dawg_(dawg) {}

vector<Move> MoveGenerator::generateMoves() {
    // Step 1: Find all start positions
    vector<StartPosition> positions = findStartPositions();

    // Step 2: Generate all raw moves
    vector<RawMove> raw_moves = generateAllRawMoves(positions);

    // Step 3: Filter and validate moves
    vector<Move> valid_moves = filterValidMoves(raw_moves);

    return valid_moves;
}

vector<Move> MoveGenerator::getBestMove() {
    vector<Move> valid_moves = generateMoves();

    if (valid_moves.empty()) {
        return valid_moves;
    }

    // Step 4: Score all moves
    Scorer scorer;
    for (auto& move : valid_moves) {
        int score = scorer.scoreMove(board_, move);
        move.setScore(score);
    }

    // Step 5: Find the best score
    int best_score = valid_moves[0].getScore();
    for (const auto& move : valid_moves) {
        if (move.getScore() > best_score) {
            best_score = move.getScore();
        }
    }

    // Step 6: Return all moves with the best score
    vector<Move> best_moves;
    for (const auto& move : valid_moves) {
        if (move.getScore() == best_score) {
            best_moves.push_back(move);
        }
    }

    return best_moves;
}

vector<Move> MoveGenerator::getTopMoves(int count) {
    vector<Move> valid_moves = generateMoves();

    if (valid_moves.empty()) {
        return valid_moves;
    }

    // Score all moves
    Scorer scorer;
    for (auto& move : valid_moves) {
        int score = scorer.scoreMove(board_, move);
        move.setScore(score);
    }

    // Sort moves by score in descending order
    std::sort(valid_moves.begin(), valid_moves.end(),
              [](const Move& a, const Move& b) {
                  return a.getScore() > b.getScore();
              });

    // Return top 'count' moves (or all if fewer than count)
    if (count >= (int)valid_moves.size()) {
        return valid_moves;
    }

    return vector<Move>(valid_moves.begin(), valid_moves.begin() + count);
}

vector<StartPosition> MoveGenerator::findStartPositions() const {
    vector<StartPosition> positions;

    if (board_.isBoardEmpty()) {
        for (int row = 1; row <= 7; row++) {
            positions.emplace_back(row, 7, Direction::VERTICAL, 7 - row + 1, 7);
        }
        for (int col = 1; col <= 7; col++) {
            positions.emplace_back(7, col, Direction::HORIZONTAL, 7 - col + 1, 7);
        }
        return positions;
    }

    int cur_row, cur_col, min_ext, max_ext;
    for (int row = 0; row <= 14; row++) {
        for (int col = 0; col <= 14; col++) {
            if (!board_.isEmpty(row, col)) {
                // cell is not empty, so cannot be a start position
                continue;
            }

            // try to extend vertically
            min_ext = 0;
            for (int cur_ext = 1; cur_ext <= 7; cur_ext++) {
                cur_row = row + cur_ext - 1;
                if (board_.isAnchor(cur_row, col)) {
                    min_ext = cur_ext;
                    break;
                }
            }
            if (min_ext == 0) {
                // no anchor found extending vertically from (row, col), continuing;
                continue;
            }
            // here find max_ext vertically: how many letters can we add vertically from (row, col)
            // including the letter we place at (row, col). Needs to take into account already placed tiles and board edge
            max_ext = 0;
            cur_row = row;
            while (cur_row <= 14) {
                if (board_.isEmpty(cur_row, col)) {
                    max_ext++;
                }
                cur_row++;
            }

            if (max_ext >= min_ext) {
                positions.emplace_back(row, col, Direction::VERTICAL, min_ext, min(max_ext, 7));
            }

            // try to extend horizontally
            min_ext = 0;
            for (int cur_ext = 1; cur_ext <= 7; cur_ext++) {
                cur_col = col + cur_ext - 1;
                if (board_.isAnchor(row, cur_col)) {
                    min_ext = cur_ext;
                    break;
                }
            }
            if (min_ext == 0) {
                // no anchor found extending horizontally from (row, col), continuing;
                continue;
            }
            // find max_ext horizontally: how many letters can we add horizontally from (row, col)
            max_ext = 0;
            cur_col = col;
            while (cur_col <= 14) {
                if (board_.isEmpty(row, cur_col)) {
                    max_ext++;
                }
                cur_col++;
            }

            if (max_ext >= min_ext) {
                positions.emplace_back(row, col, Direction::HORIZONTAL, min_ext, min(max_ext, 7));
            }
        }
    }

    return positions;
}

// ============================================================================
// STEP 2: Generate all raw moves (stub for now)
// ============================================================================

vector<RawMove> MoveGenerator::generateAllRawMoves(const vector<StartPosition>& positions) const {
    vector<RawMove> raw_moves;

    // Get rack tiles as a string
    string rack_tiles = rack_.toString();
    if (rack_tiles == "(empty)") {
        return raw_moves;  // No moves possible with empty rack
    }

    // Generate all permutations of all possible lengths (1 to rack size)
    // Keep blanks as '?' - they will be expanded lazily
    vector<string> all_permutations;
    generatePermutations(rack_tiles, 1, rack_tiles.size(), all_permutations);

    // For each start position, generate moves with lazy blank expansion
    for (const auto& pos : positions) {
        for (const auto& perm : all_permutations) {
            int perm_len = perm.size();
            // Only use permutations within the min/max extension range
            if (perm_len >= pos.min_extension && perm_len <= pos.max_extension) {
                // Generate all possible moves by expanding blanks lazily
                vector<RawMove> moves_from_perm = createRawMovesWithBlanks(perm, pos);
                for (auto& move : moves_from_perm) {
                    if (!move.placements.empty()) {
                        raw_moves.push_back(move);
                    }
                }
            }
        }
    }

    return raw_moves;
}

void MoveGenerator::generatePermutations(
    const string& tiles,
    int min_length,
    int max_length,
    vector<string>& result) const {
    // Use a set to automatically deduplicate permutations (handles duplicate tiles)
    std::set<string> unique_permutations;

    // Generate all permutations of subsets from min_length to max_length
    for (int len = min_length; len <= max_length && len <= (int)tiles.size(); len++) {
        // Generate all subsets of size len, then permute each subset
        string current;
        vector<bool> used(tiles.size(), false);
        vector<string> temp_result;
        generatePermutationsHelper(tiles, used, len, current, temp_result);

        // Insert into set to remove duplicates
        for (const auto& perm : temp_result) {
            unique_permutations.insert(perm);
        }
    }

    // Convert set back to vector
    result.clear();
    result.assign(unique_permutations.begin(), unique_permutations.end());
}

void MoveGenerator::generatePermutationsHelper(
    const string& tiles,
    vector<bool>& used,
    int remaining,
    string& current,
    vector<string>& result) const {
    if (remaining == 0) {
        result.push_back(current);
        return;
    }

    for (size_t i = 0; i < tiles.size(); i++) {
        if (!used[i]) {
            used[i] = true;
            current.push_back(tiles[i]);
            generatePermutationsHelper(tiles, used, remaining - 1, current, result);
            current.pop_back();
            used[i] = false;
        }
    }
}

void MoveGenerator::expandBlanks(
    const string& permutation,
    size_t index,
    string current,
    vector<string>& result) const {
    // Base case: reached the end of the permutation
    if (index == permutation.size()) {
        result.push_back(current);
        return;
    }

    char c = permutation[index];
    if (c == '?') {
        // Blank tile - try all 26 letters (lowercase to mark as blank)
        for (char letter = 'a'; letter <= 'z'; letter++) {
            expandBlanks(permutation, index + 1, current + letter, result);
        }
    } else {
        // Regular tile - keep it as is (uppercase)
        expandBlanks(permutation, index + 1, current + c, result);
    }
}

RawMove MoveGenerator::createRawMove(
    const string& tile_sequence,
    const StartPosition& pos) const {
    RawMove move;
    move.direction = pos.direction;
    move.start_row = pos.row;
    move.start_col = pos.col;

    int row = pos.row;
    int col = pos.col;
    size_t tile_idx = 0;

    // Place tiles in the specified direction, skipping occupied squares
    while (tile_idx < tile_sequence.size() && row <= 14 && col <= 14) {
        if (board_.isEmpty(row, col)) {
            // Place the next tile from the sequence
            char c = tile_sequence[tile_idx];
            bool is_blank = (c >= 'a' && c <= 'z');   // lowercase = blank
            char letter = is_blank ? toupper(c) : c;  // convert to uppercase for display

            TilePlacement placement(row, col, letter, true, is_blank);
            move.placements.push_back(placement);
            tile_idx++;
        }
        // Move to next position
        getNext(row, col, pos.direction);
    }

    return move;
}

vector<RawMove> MoveGenerator::createRawMovesWithBlanks(
    const string& tile_sequence,
    const StartPosition& pos) const {
    vector<RawMove> result;

    // Use recursive expansion to handle blanks lazily
    expandBlanksLazy(tile_sequence, 0, "", pos, result);

    return result;
}

string MoveGenerator::getWordPrefixForPartialSequence(
    const string& partial_sequence,
    const StartPosition& pos) const {
    // This simulates placing the partial_sequence and returns the word prefix
    // including any existing tiles on the board

    Direction dir = pos.direction;
    int row = pos.row;
    int col = pos.col;

    // First, walk backwards to find the true start of the word (in case there are tiles before start position)
    int word_start_row = row;
    int word_start_col = col;

    int prev_row = row;
    int prev_col = col;
    getPrev(prev_row, prev_col, dir);

    while (prev_row >= 0 && prev_col >= 0 && prev_row <= 14 && prev_col <= 14 &&
           !board_.isEmpty(prev_row, prev_col)) {
        word_start_row = prev_row;
        word_start_col = prev_col;
        getPrev(prev_row, prev_col, dir);
    }

    // Now build the prefix by walking forward from word_start
    string prefix = "";
    int curr_row = word_start_row;
    int curr_col = word_start_col;
    size_t tile_idx = 0;  // Index into partial_sequence

    // We need to figure out when we start using partial_sequence tiles
    // That's when we reach pos.row, pos.col
    bool reached_start_pos = (curr_row == pos.row && curr_col == pos.col);

    while (curr_row >= 0 && curr_col >= 0 && curr_row <= 14 && curr_col <= 14) {
        if (!board_.isEmpty(curr_row, curr_col)) {
            // Existing tile on board
            char letter = board_.getLetter(curr_row, curr_col);
            prefix += toupper(letter);
        } else if (reached_start_pos && tile_idx < partial_sequence.size()) {
            // Place a tile from partial_sequence
            char c = partial_sequence[tile_idx];
            bool is_blank = (c >= 'a' && c <= 'z');
            char letter = is_blank ? toupper(c) : c;
            prefix += letter;
            tile_idx++;
        } else {
            // Empty square and no more tiles to place
            break;
        }

        // Check if we've reached the start position
        if (curr_row == pos.row && curr_col == pos.col) {
            reached_start_pos = true;
        }

        getNext(curr_row, curr_col, dir);
    }

    return prefix;
}

void MoveGenerator::expandBlanksLazy(
    const string& tile_sequence,
    size_t index,
    string current,
    const StartPosition& pos,
    vector<RawMove>& result) const {
    // Base case: processed all tiles in the sequence
    if (index == tile_sequence.size()) {
        RawMove move = createRawMove(current, pos);
        if (!move.placements.empty()) {
            result.push_back(move);
        }
        return;
    }

    char c = tile_sequence[index];
    if (c == '?') {
        // Blank tile - need to try different letters
        // OPTIMIZATION: Check which letters would form valid DAWG prefixes

        // Build the word prefix up to this point (including board tiles)
        string word_prefix = getWordPrefixForPartialSequence(current, pos);

        // Try each letter, but only if it keeps us in valid DAWG space
        for (char letter = 'a'; letter <= 'z'; letter++) {
            char upper_letter = toupper(letter);

            // Check if adding this letter maintains a valid DAWG prefix
            // This prunes invalid branches early!
            // Only prune if we have a non-empty prefix (otherwise we can't make valid inferences)
            if (word_prefix.empty() || dawg_.hasPrefix(word_prefix + upper_letter)) {
                expandBlanksLazy(tile_sequence, index + 1, current + letter, pos, result);
            }
            // If hasPrefix returns false, we skip this letter entirely
            // This is the key optimization: we don't explore impossible branches
        }
    } else {
        // Regular tile - keep it as is (uppercase)
        // For regular tiles, don't prune - we trust the permutations are valid
        // The final validation will catch invalid words
        expandBlanksLazy(tile_sequence, index + 1, current + c, pos, result);
    }
}

// ============================================================================
// STEP 3: Validate moves (stub for now)
// ============================================================================

vector<Move> MoveGenerator::filterValidMoves(const vector<RawMove>& raw_moves) const {
    vector<Move> valid_moves;

    for (const auto& raw_move : raw_moves) {
        if (isValidMove(raw_move)) {
            // Get the main word for the move
            string main_word = getMainWord(raw_move);

            // Convert to Move and add to valid moves
            Move move = rawMoveToMove(raw_move, main_word);

            // Debug: Log QUARRE moves
            if (main_word == "QUARRE") {
                std::cerr << "Valid QUARRE move created: StartPos(" << raw_move.start_row << ","
                          << raw_move.start_col << " "
                          << (raw_move.direction == Direction::HORIZONTAL ? "H" : "V")
                          << ") -> Final move: " << move.toString() << std::endl;
            }

            valid_moves.push_back(move);
        }
    }

    return valid_moves;
}

bool MoveGenerator::isValidMove(const RawMove& raw_move) const {
    if (raw_move.placements.empty()) {
        return false;
    }

    // Get the main word
    string main_word = getMainWord(raw_move);
    if (main_word.length() < 2) {
        return false;  // Words must be at least 2 letters
    }

    // Validate main word in DAWG
    if (!dawg_.contains(main_word)) {
        return false;
    }

    // Get all cross-words
    vector<string> cross_words = getCrossWords(raw_move);

    // Validate all cross-words
    for (const auto& cross_word : cross_words) {
        if (!dawg_.contains(cross_word)) {
            return false;
        }
    }

    return true;
}

string MoveGenerator::getMainWord(const RawMove& raw_move) const {
    if (raw_move.placements.empty()) {
        return "";
    }

    Direction dir = raw_move.direction;

    // Find the actual start of the word by walking backward from start position
    int word_start_row = raw_move.start_row;
    int word_start_col = raw_move.start_col;

    int prev_row = word_start_row;
    int prev_col = word_start_col;
    getPrev(prev_row, prev_col, dir);

    while (prev_row >= 0 && prev_col >= 0 && prev_row <= 14 && prev_col <= 14 &&
           !board_.isEmpty(prev_row, prev_col)) {
        word_start_row = prev_row;
        word_start_col = prev_col;
        getPrev(prev_row, prev_col, dir);
    }

    // Now walk forward from the actual start, collecting all letters
    string word = "";
    int row = word_start_row;
    int col = word_start_col;

    while (row >= 0 && col >= 0 && row <= 14 && col <= 14) {
        if (!board_.isEmpty(row, col)) {
            // Existing tile on board (convert to uppercase, as blanks are stored as lowercase)
            char letter = board_.getLetter(row, col);
            word += toupper(letter);
        } else {
            // Check if we placed a tile here
            bool found = false;
            for (const auto& placement : raw_move.placements) {
                if (placement.row == row && placement.col == col) {
                    word += placement.letter;
                    found = true;
                    break;
                }
            }
            if (!found) {
                break;  // No tile here, end of word
            }
        }
        getNext(row, col, dir);
    }

    return word;
}

vector<string> MoveGenerator::getCrossWords(const RawMove& raw_move) const {
    vector<string> cross_words;

    // For each newly placed tile, check for cross-words in the perpendicular direction
    Direction perp_dir = (raw_move.direction == Direction::HORIZONTAL) ? Direction::VERTICAL : Direction::HORIZONTAL;

    for (const auto& placement : raw_move.placements) {
        int row = placement.row;
        int col = placement.col;

        // Check if there are tiles adjacent in the perpendicular direction
        int prev_row = row;
        int prev_col = col;
        getPrev(prev_row, prev_col, perp_dir);

        int next_row = row;
        int next_col = col;
        getNext(next_row, next_col, perp_dir);

        bool has_prev = (prev_row >= 0 && prev_col >= 0 && prev_row <= 14 && prev_col <= 14 &&
                         !board_.isEmpty(prev_row, prev_col));
        bool has_next = (next_row >= 0 && next_col >= 0 && next_row <= 14 && next_col <= 14 &&
                         !board_.isEmpty(next_row, next_col));

        if (!has_prev && !has_next) {
            // No cross-word formed by this tile
            continue;
        }

        // Find the start of the cross-word
        int word_start_row = row;
        int word_start_col = col;

        prev_row = row;
        prev_col = col;
        getPrev(prev_row, prev_col, perp_dir);

        while (prev_row >= 0 && prev_col >= 0 && prev_row <= 14 && prev_col <= 14 &&
               !board_.isEmpty(prev_row, prev_col)) {
            word_start_row = prev_row;
            word_start_col = prev_col;
            getPrev(prev_row, prev_col, perp_dir);
        }

        // Collect the cross-word
        string cross_word = "";
        int curr_row = word_start_row;
        int curr_col = word_start_col;

        while (curr_row >= 0 && curr_col >= 0 && curr_row <= 14 && curr_col <= 14) {
            if (!board_.isEmpty(curr_row, curr_col)) {
                // Existing tile on board (convert to uppercase, as blanks are stored as lowercase)
                char letter = board_.getLetter(curr_row, curr_col);
                cross_word += toupper(letter);
            } else if (curr_row == row && curr_col == col) {
                // This is the newly placed tile
                cross_word += placement.letter;
            } else {
                break;
            }
            getNext(curr_row, curr_col, perp_dir);
        }

        if (cross_word.length() > 1) {
            cross_words.push_back(cross_word);
        }
    }

    return cross_words;
}

Move MoveGenerator::rawMoveToMove(const RawMove& raw_move, const string& word) const {
    // Find the actual start of the word (may be before start_row/start_col if there are existing tiles)
    Direction dir = raw_move.direction;
    int word_start_row = raw_move.start_row;
    int word_start_col = raw_move.start_col;

    int prev_row = word_start_row;
    int prev_col = word_start_col;
    getPrev(prev_row, prev_col, dir);

    while (prev_row >= 0 && prev_col >= 0 && prev_row <= 14 && prev_col <= 14 &&
           !board_.isEmpty(prev_row, prev_col)) {
        word_start_row = prev_row;
        word_start_col = prev_col;
        getPrev(prev_row, prev_col, dir);
    }

    // Create the move
    Move move(word_start_row, word_start_col, dir, word);

    // Add all placements from the raw move
    for (const auto& placement : raw_move.placements) {
        move.addPlacement(placement);
    }

    return move;
}

}  // namespace scradle
