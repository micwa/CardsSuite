## CardsSuite

For those who like their games clean and minimalistic, CardsSuite is a
collection of card games playable from the terminal. Currently, the games
included are:

1. War
2. Solitaire

Suggestions for games to further include are welcome.

Note: Since the cards printed on the screen are literal UTF-8 card characters,
only terminals supporting UTF-8 by default are supported. Sorry Windows users!

### Included Games

#### War

This is the classic game of war. The deck is split in two so that you receive
half the deck and the opponent the other half. Every turn, the top card on each
player's pile is flipped over and compared. The player who has the higher
card wins that turn and keeps the opponent's card, and then puts both cards at
the bottom of his pile. This repeats until one player runs out of cards. That
player loses, and the player who possesses the whole deck wins.

Controls:

* Press `<Enter>` to play the next card
* Press `p` to pause

#### Solitaire

Klondike, or "Solitaire" as it is commonly known in the US, is a singe-player
game where the goal is to fill up the four foundation piles so that they each
contain cards of the same suit from Ace to King. An extensive set of rules for
Klondike/solitaire can be found [here]
(http://www.solitaire-cardgame.com/#rules).

Controls (type the character followed by `<Enter>`; `[x-y]` denotes a range of
possible values):

* `s` - flip the stock
* `t` - move a card from the waste to a column or foundation pile (will prompt)
    * `[1-7]` - move from waste to a column
    * `w`, `x`, `y`, or `z` - move from waste to the specified foundation pile
* `[1-7]` - move a card from a tableau pile to another column or a foundation
pile (will prompt)
    * `[1-7]` - move the LAST card from the first tableau pile to the second
    * `[a-s]` - move MULTIPLE cards starting at the selected row to another
    column (will prompt for another column)
    * `w`, `x`, `y`, or `z` - move the last card from the selected tableau pile
    to the specified foundation pile
* `u` - undo the previous move, to a maximum of 10 moves back (if you're a true
solitaire player, you won't use this)
* Press `P` to pause (note capital 'P')

### Building from source

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

### License

This software is released under the [MIT license]
(http://opensource.org/licenses/MIT).
