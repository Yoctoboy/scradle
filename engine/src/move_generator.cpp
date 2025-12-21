#include "move_generator.h"

#include <algorithm>
#include <cctype>

using std::min;
using std::string;
using std::vector;

namespace scradle {

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
    vector<string> all_permutations;
    generatePermutations(rack_tiles, 1, rack_tiles.size(), all_permutations);

    // Expand blank tiles in permutations
    vector<string> expanded_permutations;
    for (const auto& perm : all_permutations) {
        expandBlanks(perm, 0, "", expanded_permutations);
    }

    // For each start position, use only permutations with appropriate lengths
    for (const auto& pos : positions) {
        for (const auto& perm : expanded_permutations) {
            int perm_len = perm.size();
            // Only use permutations within the min/max extension range
            if (perm_len >= pos.min_extension && perm_len <= pos.max_extension) {
                RawMove raw_move = createRawMove(perm, pos);
                // Only add moves that actually placed tiles
                if (!raw_move.placements.empty()) {
                    raw_moves.push_back(raw_move);
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
    // Generate all permutations of subsets from min_length to max_length
    for (int len = min_length; len <= max_length && len <= (int)tiles.size(); len++) {
        // Generate all subsets of size len, then permute each subset
        string current;
        vector<bool> used(tiles.size(), false);
        generatePermutationsHelper(tiles, used, len, current, result);
    }
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
            TilePlacement placement;
            placement.row = row;
            placement.col = col;
            placement.letter = tile_sequence[tile_idx];
            placement.isBlank = false;  // TODO: Handle blanks in Step 2 or later
            move.placements.push_back(placement);
            tile_idx++;
        }
        // Move to next position
        getNext(row, col, pos.direction);
    }

    return move;
}

// ============================================================================
// STEP 3: Validate moves (stub for now)
// ============================================================================

vector<Move> MoveGenerator::filterValidMoves(const vector<RawMove>& raw_moves) const {
    vector<Move> valid_moves;
    // TODO: Implement in Step 3
    return valid_moves;
}

bool MoveGenerator::isValidMove(const RawMove& raw_move, string& main_word) const {
    // TODO: Implement in Step 3
    return false;
}

string MoveGenerator::getMainWord(const RawMove& raw_move) const {
    // TODO: Implement in Step 3
    return "";
}

vector<string> MoveGenerator::getCrossWords(const RawMove& raw_move) const {
    // TODO: Implement in Step 3
    return vector<string>();
}

Move MoveGenerator::rawMoveToMove(const RawMove& raw_move, const string& word) const {
    // TODO: Implement in Step 3
    return Move();
}

// ============================================================================
// Utility functions
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

}  // namespace scradle
