<pre>
Peg Solitaire
=============

<a href="https://en.m.wikipedia.org/wiki/Peg_solitaire">link to wikipedia on peg solitaire</a>

Created a working prototype of Peg Solitaire board game. A command driven
program written in C that runs in the Terminal.

The goal is to get 1 bead in the centre of the board. To eliminate a bead use an 
adjacent bead to jump over the intended bead to be eliminated. A bead may only 
jump horizontally or vertically over 1 or more beads provided that there is an 
empty space in front of the bead that is intended to be eliminated.

+-------------------+----------------------------------------------------------+
| List of Commands: | Description:                                             |
|                   |                                                          |
| up                | Moves cursor up in the Main & Menu windows               |
| down              | Moves cursor down in the Main & Menu windows             |
| right             | Moves cursor right in the Main window only               |
| left              | Moves cusor left in the Main window only                 |
| select            | Selects a bead as well as deselects a bead in the Main   |
|                   | window. Executes the current option in the Main window   |
| tab               | Switches window from Main to Menu to Info & back to Main |
| help              | Lists the commands                                       |
| quit              | Exits the program                                        |
| main              | Shortcut to Main window                                  |
| menu              | Shortcut to Menu window                                  |
| 1 2 3 4 5 6       | Shortcuts that only works in the menu window             |
| info              | Shortcut to Info window                                  |
| new               | Starts a new game                                        |
| load              | Loads a game                                             |
| save              | Saves a game                                             |
| settings          | Displays settings                                        |
+-------------------+----------------------------------------------------------+

An image of how the game looks:
</pre>

<img src="https://github.com/AshrafAllie/Chinese_Checkers/blob/master/peg_solitaire.png" />
