# Scradle

A scrabble like application

## Best practices

Don't forget to test your own code

## Engine

This folder contains a C++ engine that, given the state of a scrabble grid and a set of letters, finds the word that gives the most points.
The engine is able to:
- solve for the best possible word
- have object representations of a scrabble grid and of the french dictionnary (direct acyclic word graph - dawg). The complete list of accepted words is in engine/dawgs
- be callable from other C++ functions

## Expensive game finder

Using the rules of Duplicate Scrabble, the scripts/expensive_game_finder folder will contain an implementation that tries to find very expensive games, i.e. games that have a high total final score.

In duplicate scrabble, given a grid state and 7 (or less) tiles, only the very best move is placed on the grid each turn. To get a high scoring game, we have influence over two decisive factors:
- Which tiles are drawn at what moment (with respect to whih tiles are remaining in the bag)
- If there are several moves that score the same best score, we can choose whichever we want

All other Scrabble rules are kept the same.

First implementation steps:
- ExpensiveGameFinder class that handles a GameState, uses MoveGenerator to generate moves and has an entrypoint method
- That entrypoint method does:
    - Find 2 mutually compatible high-scoring 15-letter words (without joker), i.e. that can theoretically be placed simultaneously on the grid (can use the help of scripts/expensive_game_finder/score_15_letter_words.cpp implementation)
    - Is able to manage a gamestate and remove the last made move from it (that may have to be implemented in the GameState object)