Using the rules of Duplicate Scrabble, this folder will contain an implementation that tries to find very expensive games, i.e. games that have a high total final score.

In duplicate scrabble, given a grid state and 7 (or less) tiles, only the very best move is placed on the grid each turn. To get a high scoring game, we have influence over two decisive factors:
- Which tiles are drawn are what moment (with respect to whih tiles are remaining in the bag)
- If there are several moves that score the same, best score, we can choose whichever we like

Here are the main ideas of the algorithm (may change later upon further investigation of these ideas):
- We aim to make 15-letter words where we place tiles over 3 "triple word" tiles, as those moves can score more than 1000 points in a single word. One can only place two such words on the grid as triple-word tiles are scarce and only usable once (as are every other preium tiles)
- Since we only have 7 letters per move, some of these 15 letters will have to be placed beforehand. However we'd like to avoid placing such letters on "double letter" or "triple word" tiles because that would mean the final 15 letter word would not get to score them.
- Given a game state, we will have to make a function that evaluates its "potential", i.e. a score function that estimates if such 15 letter words may be placed further down the road, or if all such places are already blocked
- Other moves will mostly have to be "setup" moves to make sure the 15 letter triple-triple words can be placed. Such setup moves will have to be efficient

All other Scrabble rules are kept the same.

Around 97% of normal duplicate scrabble games (i.e. with random draw of tiles from the bag) end up with a score between 750 and 1200 points. Reaching 1500 and 2000 points will be nice checkpoints already. The current world record is 4017 which is obviously very high and quite improbable to stumble upon by playing randomly.

First implementation steps:
- Find out high scoring 15-letter words and store them in some object. Then display them
- Make a function that evalutes the potential of a given grid state
- Find a way to cleverly draw the right tiles at the right moment