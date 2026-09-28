# Yahtzee in C

A command-line Yahtzee game for a human player versus the computer. The game runs for 13 rounds and tracks scores across the Yahtzee categories.

The computer can reroll dice up to twice. It compares the expected best available score for each possible set of dice to reroll, then scores the final roll in the highest-scoring unused category.

## Build

With GCC installed:

```sh
gcc -std=c11 -Wall -Wextra yahtzee.c -o yahtzee
```

On Windows, run `yahtzee.exe`. On macOS or Linux, run `./yahtzee`.

## Play

Run the program in a terminal and follow the prompts to enter a player name, choose dice to reroll, and select a scoring category.
