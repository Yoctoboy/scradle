Design an implementation plan for adding Duplicate Scrabble game generation to the Scradle engine.

## Context from exploration:
The codebase currently has:
- Excellent solver: Board (15x15), DAWG (411k French words), MoveGenerator, Scorer
- Rack class (holds 7 tiles)
- Comprehensive test infrastructure
- NO tile bag, NO random generation, NO game state management, NO game loop

The engine is a stateless solver - given board + rack → find best move.

## Requirements:
1. **Optimal single-player Duplicate Scrabble**: Always play the highest-scoring move
2. **Seeded randomness**: Use random seeds to reproduce games deterministically
3. **Statistics tracking**:
   - Final score
   - Number of moves
   - Complete move history (word, position, direction, score)
   - Number of bingos (7-tile words, 50pt bonus)
4. **Game generation**: Create complete games from start to finish

## Duplicate Scrabble Rules:
- Start with empty board, draw 7 tiles randomly
- Find best move, play it on board
- Refill rack to 7 tiles (or as many as available)
- Repeat until tile bag empty (or until no more vowels, or no more consonants) OR no valid moves possible

## Design Constraints:
- French Scrabble tile distribution (100 tiles total):
  - 15 A's, 2 B's, 2 C's, 3 D's, 15 E's, 2 F's, 2 G's, 2 H's
  - 8 I's, 1 J, 1 K, 5 L's, 3 M's, 6 N's, 6 O's, 2 P's
  - 1 Q, 6 R's, 6 S's, 6 T's, 6 U's, 2 V's, 1 W, 1 X, 1 Y, 1 Z
  - 2 blanks ('?')
- Use C++17, match existing code style
- Keep solver (Board, MoveGenerator, Scorer) unchanged
- Add new classes: TileBag, GameState, DuplicateGame
- Use std::mt19937 for seeded randomness

Please provide:
1. New class designs (TileBag, GameState, DuplicateGame) with key methods
2. File structure (where to add new .h/.cpp files)
3. Game loop algorithm pseudocode
4. Statistics collection approach
5. Testing strategy
6. Critical implementation details and edge cases

Focus on a clean, testable design that integrates well with the existing solver architecture.