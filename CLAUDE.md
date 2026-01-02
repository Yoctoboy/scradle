# Scradle

A scrabble like application

## Best practices

Don't forget to test your own code
Add a command in Makefile whenever it is relevant

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
    - Find 3 mutually compatible high-scoring 15-letter words (without joker), i.e. that can theoretically be placed simultaneously on the grid (can use the help of scripts/expensive_game_finder/score_15_letter_words.cpp implementation)
    - Is able to manage a gamestate, add and remove the last made move from it (that may have to be implemented in the GameState class)

The next step to implement for this algorithm is: if, for one of the main words, we can actually place a contiguous substring (or simply "substring") of it, then we do it. This will need:
- to have a method that returns a vector of scrabble-valid substrings of another word given as parameter (as well as the position in the main word at which is starts)
- to test if any such word can be placed right now (i.e. needing 7 tiles or less, and connected to some other tile of the grid, i.e. a valid move) without filling any "triple word" tile
- then try a bunch of racks consisting of those letters, find the best moves for this rack and if one of these moves is actually the substring we are processing, then we place it on the board and that's our move for the loop