Chinese_Checkers
================

I created a working prototype of a variant of the Chinese Checkers board game. A
command driven program written in C that runs in the Terminal.

The goal is to get 1 bead in the centre of the board. To eliminate a bead use an 
adjacent bead to jump over the intended bead to be eliminated. A bead may only 
jump horizontally or vertically over 1 or more beads provided that there is an 
empty space in front of the bead that is intended to be eliminated.

+-------------------+----------------------------------------------------------+<br />
| List of Commands: | Description:                                             |<br />
|                   |                                                          |<br />
| up                | Moves cursor up in the Main & Menu windows               |<br />
| down              | Moves cursor down in the Main & Menu windows             |<br />
| right             | Moves cursor right in the Main window only               |<br />
| left              | Moves cusor left in the Main window only                 |<br />
| select            | Selects a bead as well as deselects a bead in the Main   |<br />
|                   | window. Executes the current option in the Main windown  |<br />
| tab               | Switches window from Main to Menu to Info & back to Main |<br />
| help              | Lists the commands                                       |<br />
| quit              | Exits the program                                        |<br />
| main              | Shortcut to Main window                                  |<br />
| menu              | Shortcut to Menu window                                  |<br />
| 1 2 3 4 5 6       | Shortcuts that only works in the menu window             |<br />
| info              | Shortcut to Info window                                  |<br />
| new               | Starts a new game                                        |<br />
| load              | Loads a game                                             |<br />
| save              | Saves a game                                             |<br />
| settings          | Displays settings                                        |<br />
+-------------------+----------------------------------------------------------+<br />
