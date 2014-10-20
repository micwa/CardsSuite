CardsSuite - Console card games for the GUI-less

## Overview

For those who like their games clean and minimalistic, CardsSuite is a
collection of card games playable from the terminal. Currently, the games
included are:

1. War
2. Solitaire

Suggestions for games to further include are welcome.

Note: Since the cards printed on the screen are literal UTF-8 card characters,
only terminals supporting UTF-8 by default are supported. Sorry Windows users!

##### Building from source

This project does not contain any executables, so you will have to build from
source. This requires [gcc] (https://gcc.gnu.org/) or another C compiler, and
[make] (http://www.gnu.org/software/make/) to utilize the included makefile
(which is optional, but greatly eases the process).

Run the following commands at the terminal:

```
git clone https://github.com/micwa6/CardsSuite.git
cd CardsSuite
make all
```

Then play the game with `./CardsSuite`, or `./CardsSuite [game_name]`.
Have fun!
