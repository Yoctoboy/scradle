# Scradle

A scrabble like application

## Best practices

Don't forget to test your own code

## Engine

This folder will contain a C++ engine that, given the state of a scrabble grid and a set of letters, finds the word that gives the most points.
The engine will be able to:
- solve for the best possible word
- have object representations of a scrabble grid and of the french dictionnary (direct acyclic word graph - dawg). The complete list of accepted words is in engine/dawgs
- be callable from a Node.JS environment, in particulier in AWS Lambda for a future website based on the engine
- be callable from other C++ functions

## Implementation Phases

### Phase 1: Board & Basic Data Structures ✅ COMPLETE
- ✅ Implement 15x15 board representation
- ✅ Premium square types (DL, TL, DW, TW)
- ✅ Cell structure for letters and premium squares
- ✅ Basic board operations (get/set tile, display)
- ✅ Rack representation for 7 tiles
- ✅ Test framework with colored output
- ✅ Passing tests

### Phase 2: DAWG Loader and Validator ✅ COMPLETE
- ✅ DAWG node structure with child pointers
- ✅ Load from text files (test_words.txt with 352 French words)
- ✅ Word validation function (contains, hasPrefix)
- ✅ Prefix traversal for move generation (getWordsWithPrefix)
- ✅ Case-insensitive word matching
- ✅ Passing tests

### Phase 3: Move Generator with Cross-Checks ✅ COMPLETE
- ✅ Anchor identification algorithm
- ✅ Cross-check set computation
- ✅ Move generation using DAWG traversal
- ✅ Left-part and right-part extension logic
- ✅ Move and TilePlacement structures
- ✅ Direction-agnostic traversal (horizontal/vertical)
- ✅ Integration with DAWG for word validation
- ✅ Word validation ensuring only valid DAWG words are generated
- ✅ 54 passing tests (all moves verified against DAWG)

### Phase 4: Scoring System ✅ COMPLETE
- ✅ Letter values for French Scrabble (all standard values implemented)
- ✅ Premium square scoring logic (DL, TL, DW, TW)
- ✅ Premium squares only apply to newly placed tiles
- ✅ Cross-word scoring (perpendicular words formed by new tiles)
- ✅ 50-point bonus for 7-tile words (bingo)
- ✅ Main word scoring with letter and word multipliers
- ✅ Find best move given a game state (grid and rack contents)
- ✅ 37 passing tests covering all scoring scenarios

### Phase 5: Integration & Optimization
- Complete solver that finds best move
- Performance optimizations (caching, pruning)
- Bitboards for fast lookups
- Move ordering heuristics

### Phase 6: Node.js Bindings
- N-API integration
- JavaScript API design
- Board state serialization/deserialization
- Error handling and validation

### Phase 7: AWS Lambda Compatibility
- Optimize binary size
- Cold start optimization
- Memory usage profiling
- Lambda deployment configuration