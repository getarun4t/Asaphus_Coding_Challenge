# Asaphus_Coding_Challenge
Two player coding challenge by Asaphus

The goal is to implement the mechanics of a simple game and write test cases for them.

The rules of the game are:
  - There are two types of boxes, green and blue. 
  - Both can absorb tokens of a given weight, which they add to their own total weight. 
  - Both are initialized with a given initial weight.
  - After a box absorbs a token weight, it outputs a score. 
  - Green and blue boxes calculate the score in different ways:
  - A green box calculates the score as the square of the mean of the 3 weights that it most recently absorbed (square of mean of all absorbed weights if there are fewer than 3).
  - A blue box calculates the score as Cantor's pairing function of the smallest and largest weight that it has absorbed so far, i.e. pairing(smallest, largest), where pairing(0, 1) = 2
  - The game is played with two green boxes with initial weights 0.0 and 0.1, and two blue boxes with initial weights 0.2 and 0.3.
  - There is a list of input token weights. Each gets used in one turn.
  - There are two players, A and B. Both start with a score of 0. 
  - The players take turns alternatingly. Player A starts.
  - In each turn, the current player selects one of the boxes with the currently smallest weight, and lets it absorb the next input token weight. Each input weight gets only used once.
  - The result of the absorption gets added to the current player's score.
  - When all input token weights have been used up, the game ends, and the player with highest score wins.
 
  Task:
  - Create a git repo for the implementation. Use the git repository to have a commit history.
  - Implement all missing parts, marked with "TODO", including the test cases.
  - Split the work in reasonable commits (not just one commit).
  - Make sure the test cases succeed.
  - Produce clean, readable code.
 
  Notes:
  - Building and running the executable: g++ --std=c++14 asaphus_coding_challenge.cpp -o challenge && ./challenge
  - Feel free to add a build system like CMake, meson, etc.
  - Feel free to add more test cases, if you would like to test more.
  - This file includes the header-only test framework Catch v2.13.9.
  - A main function is not required, as it is provided by the test framework.
