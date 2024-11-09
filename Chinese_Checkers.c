//Date:    31Mar2013Sun 20:44
//Updated: 17Aug2020Mon 00:16:14
//File:    Chinese_Checkers.c
//Version: v1.5.14
//Author:  Ashraf
//Email:   ashraf.allie01@gmail.com
/*Desc:    A variant of the Chinese Checkers board game
           The goal is to get 1 bead in the centre of the board. To eliminate a
           bead use an adjacent bead to jump over the intended bead to be
           eliminated. A bead may only jump horizontally or vertically over 1 or
           more beads provided that there is an empty space in front of the bead
           that is intended to be eliminated




    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


//----------------
//Macro Defintions
//----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BUFFER 9


//-------------------------
//Data Structure Defintions (struct, enum union)
//-------------------------
enum WindowType {Main, Menu, Info};


//----------------------------
//Global Variable Declarations
//----------------------------
unsigned char board[9][9],
              beads,
              Current_Board_Row,
              Current_Board_Col,
              Selected_Bead_Row,
              Selected_Bead_Col,
              Screen_Rows,
              Screen_Cols,
              Screen[24][66],
              HighlightedMenuOption = 1,
              Valid_Bead_Hop = 0;

char Entered_Command[BUFFER];

enum WindowType CurrentWindow = Main;


//---------------------
//Function Declarations (prototypes)
//---------------------
void Splash_Screen(void);
void Term_Screen_Size_Detection(void);
void Board_Initialization(void);
void ScreenBorderSetup(void);
void MainMenu(unsigned char HighlightedChoice);
void Display_Screen(void);
void Chinese_Checkers_Board(void);
void Status_Window(void);
void Info_Window(unsigned char Msg);
void Command_Line(void);
void Board_Cursor(void);
void Bead_Manager(void);
void Save_Game(void);
void Load_Game(void);
void Input(char *string_ptr, size_t num_characters);


//------------
//Main Program
//------------
int main(void)
{
 Splash_Screen();
 Term_Screen_Size_Detection();
 Board_Initialization();
 ScreenBorderSetup();
 Status_Window();
 MainMenu(HighlightedMenuOption);
 Chinese_Checkers_Board();
 Board_Cursor();
 Info_Window(0);
 Display_Screen();

 do
 {
  Command_Line();
  Display_Screen();
 }
 while (strcmp(Entered_Command, "quit"));

 return 0;
}


//--------------------
//Function Definitions (Implementation)
//--------------------


//FUNCTION: Splash_Screen
void Splash_Screen(void)
{
 const char *version = "v1.5.14 standard C version";

 printf(
 "Chinese Checkers\n"
 "A variant of the Chinese Checkers board game\n\n"
 "Created by Ashraf\n"
 "ashraf.allie01@gmail.com\n\n"
 "%s\n"
 "Copyleft 2013\n\n"
 "Press enter key to continue\n\n",
 version);

 getchar();
}


//FUNCTION: Term_Screen_Size_Detection
void Term_Screen_Size_Detection(void)
{
 struct winsize w;
 ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);


 if (w.ws_row > 23) Screen_Rows = 24; else Screen_Rows = 0;
 if (w.ws_col > 65) Screen_Cols = 66; else Screen_Cols = 0;


 if ((Screen_Rows <= 23) || (Screen_Cols <= 65))
 {
  printf("Window size too small\nPlease resize\n"
         "Screen rows must be greater than 23\n"
         "Screen columns must be greater than 65\n\n");

  exit(EXIT_FAILURE);
 }
}


//FUNCTION: Board_Initialization
void Board_Initialization(void)
{
 unsigned char board_row, board_col;

 beads = 44;
 Current_Board_Row = 4;
 Current_Board_Col = 4;
 Selected_Bead_Row = 'N',
 Selected_Bead_Col = 'N';

 //Initializing the 4 non used 3x3 blocks to NULL (N)
 for (board_row = 0; board_row <= 2; board_row++)
    for (board_col = 0; board_col <= 2; board_col++)
    {
     board[board_row][board_col] = 'N';
     board[8 - board_row][board_col] = 'N';
     board[board_row][8 - board_col] = 'N';
     board[8 - board_row][8 - board_col] = 'N';
    }

 //Initialize all remaining blocks to beads represented by (X)
 for (board_row = 0; board_row <= 2; board_row++)
    for (board_col = 3; board_col <= 5; board_col++)
    {
     board[board_row][board_col] = 'X';     //Top 3x3 block
     board[board_col][board_row] = 'X';     //Left 3x3 block
     board[8 - board_row][board_col] = 'X'; //Bottom 3x3 block
     board[board_col][8 - board_row] = 'X'; //Right 3x3 block
     board[board_row + 3][board_col] = 'X'; //Central 3x3 block
                                            //Warning sets the central element block!
    }

 //Uninitialize the central block represented by a space
 board[Current_Board_Row][Current_Board_Col] = ' ';
}


//FUNCTION: ScreenBorderSetup
void ScreenBorderSetup(void)
{
 unsigned char Row, Col;

 for (Row = 0; Row < Screen_Rows-1; Row++)
    for (Col = 0; Col < Screen_Cols; Col++)
    {
     Screen[Row][Col] = ' ';
     Screen[Row][Screen_Cols - 1] = '\0';
    }

 Screen[0][0] = '+';                             //top left corner
 Screen[0][Screen_Cols - 2] = '+';               //top right corner
 Screen[Screen_Rows - 2][0]= '+';                //bottom left corner
 Screen[Screen_Rows - 2][Screen_Cols - 2] = '+'; //bottom right corner

 for (Col = 1; Col < Screen_Cols - 2; Col++)     //top & bottom borders
 {
  Screen[0][Col] = '-';
  Screen[Screen_Rows - 2][Col] = '-';
 }

 for (Row = 1; Row < Screen_Rows - 2; Row++)    //left, right & internal borders
 {
  Screen[Row][0] = '|';
  Screen[Row][Screen_Cols - 2] = '|';
  Screen[Row][Screen_Cols - 20] = '|';
 }

 for (Col = Screen_Cols - 19; Col < Screen_Cols - 2; Col++)
 {
  Screen[9][Col] = '-';                   //border between Menu & Info
  Screen[Screen_Rows - 6][Col] = '-';     //border between Info & Status
 }

                                          //Part of internal border conjunctions
 Screen[0][Screen_Cols - 20] = '+';
 Screen[9][Screen_Cols - 20] = '+';
 Screen[9][Screen_Cols - 2]  = '+';
 Screen[Screen_Rows - 6][Screen_Cols - 20] = '+';
 Screen[Screen_Rows - 6][Screen_Cols - 2]  = '+';
 Screen[Screen_Rows - 2][Screen_Cols - 20] = '+';
}


//FUNCTION: MainMenu
void MainMenu(unsigned char HighlightedChoice)
{
 unsigned char Menu_Row, Menu_Col, line_space_and_menu_shift_down = 0;
 char *MainMenuArray[] =
 {
  "Menu:",
  "1. Intro / Help",
  "2. New Game",
  "3. Load Game",
  "4. Save Game",
  "5. Settings",
  "6. Quit"
 };

 //Copies Main Menu into Screen Array
 for (Menu_Row = 0; Menu_Row <= 6; Menu_Row++)
 {
  for (Menu_Col = 0; MainMenuArray[Menu_Row][Menu_Col] != '\0'; Menu_Col++)
  {
   if (2 == Menu_Row + 1)
     line_space_and_menu_shift_down = 1;
   Screen[Menu_Row + 1 + line_space_and_menu_shift_down][Screen_Cols - 18 + Menu_Col] = MainMenuArray[Menu_Row][Menu_Col];
   if (HighlightedChoice == Menu_Row)
     Screen[Menu_Row + 1 + line_space_and_menu_shift_down][Screen_Cols - 18 + 2] = '[';
  }

  //Clears the closed square bracket
  Screen[Menu_Row + 1 + line_space_and_menu_shift_down][Screen_Cols - 18 + Menu_Col] = ' ';

  if (HighlightedChoice == Menu_Row)
    Screen[Menu_Row + 1 + line_space_and_menu_shift_down][Screen_Cols - 18 + Menu_Col] = ']';
 }

}


//FUNCTION: Display_Screen
void Display_Screen()
{
 unsigned char Row;

 for (Row = 0; Row < Screen_Rows - 1; Row++)
    printf("%s\n", Screen[Row]);
}


//FUNCTION: Chinese_Checkers_Board
void Chinese_Checkers_Board()
{
 unsigned char Game_Board_Row, Game_Board_Col;
 char *Game_Board[] =
{
"    0   1   2   3   4   5   6   7   8",
"              +---+---+---+",
"0             |   |   |   |             0",
"              +---+---+---+",
"1             |   |   |   |             1",
"              +---+---+---+",
"2             |   |   |   |             2",
"  +---+---+---+---+---+---+---+---+---+",
"3 |   |   |   |   |   |   |   |   |   | 3",
"  +---+---+---+---+---+---+---+---+---+",
"4 |   |   |   |   |   |   |   |   |   | 4",
"  +---+---+---+---+---+---+---+---+---+",
"5 |   |   |   |   |   |   |   |   |   | 5",
"  +---+---+---+---+---+---+---+---+---+",
"6             |   |   |   |             6",
"              +---+---+---+",
"7             |   |   |   |             7",
"              +---+---+---+",
"8             |   |   |   |             8",
"              +---+---+---+",
"    0   1   2   3   4   5   6   7   8"
};

//Writes the Game_Board & the board beads to Screen Array
for (Game_Board_Row = 0; Game_Board_Row < 21; Game_Board_Row++)
   for (Game_Board_Col = 0; Game_Board[Game_Board_Row][Game_Board_Col] != '\0'; Game_Board_Col++)
   {
    Screen[Game_Board_Row+1][Game_Board_Col+3] = Game_Board[Game_Board_Row][Game_Board_Col];

    if ((Game_Board_Row % 2) == 0 && (Game_Board_Row / 2) != 0)
      if ((Game_Board_Col % 4) == 0 && (Game_Board_Col / 4) != 0)
        if ((Game_Board_Row/2)-1 <= 8 && (Game_Board_Col/4)-1 <= 8)
        {
          if (board[(Game_Board_Row/2)-1][(Game_Board_Col/4)-1] == 'N')
            Screen[Game_Board_Row+1][Game_Board_Col+3] = ' ';
          else
          Screen[Game_Board_Row+1][Game_Board_Col+3] = board[(Game_Board_Row/2)-1][(Game_Board_Col/4)-1];
        }
   }
}


//FUNCTION: Status_Window
void Status_Window(void)
{
 unsigned char Col;
 char *Status[] =
 {
  "Window: Main",
  "Window: Menu",
  "Window: Info",
  "Row:",
  "Col:"
 };

 switch (CurrentWindow)
 {
  case Main:
       CurrentWindow = Menu;
       for (Col = Screen_Cols - 20; Col <= Screen_Cols - 2; Col++)
       {
        Screen[Screen_Rows - 4][Col] = Screen[9][Col];
        Screen[Screen_Rows - 5][Col] = Screen[2][Col];
        Screen[Screen_Rows - 6][Col] = Screen[2][Col];
       }
       break;

  case Menu:
       CurrentWindow = Main;

  case Info:
       CurrentWindow = Main;
       for (Col = Screen_Cols - 20; Col <= Screen_Cols - 2; Col++)
       {
        Screen[Screen_Rows - 4][Col] = Screen[2][Col];
        Screen[Screen_Rows - 5][Col] = Screen[2][Col];
        Screen[Screen_Rows - 6][Col] = Screen[9][Col];
       }

       for (Col = 0; Status[3][Col] != '\0'; Col++)
                                       //Note the condition for for loop is the
                                       //same length for index rows 3 & 4
       {
        Screen[Screen_Rows - 5][Screen_Cols - 18 + Col] = Status[3][Col];
        Screen[Screen_Rows - 4][Screen_Cols - 18 + Col] = Status[4][Col];
       }

       Screen[Screen_Rows - 5][Screen_Cols - 13] = Current_Board_Row + 48;
       Screen[Screen_Rows - 4][Screen_Cols - 13] = Current_Board_Col + 48;
       break;
 }

 for (Col = 0; Status[CurrentWindow][Col] != '\0' ; Col++)
    Screen[Screen_Rows - 3][Screen_Cols - 18 + Col] = Status[CurrentWindow][Col];
}


//FUNCTION: Info_Window
void Info_Window(unsigned char Msg)
{
 unsigned char Row = 11, Col;
 char *Info[] =
 {
  "Info:",
  "Error invalid  command. Type  \"help\" to list commands",
  "Options 1 to 6 only work in   the menu window",
  "Commands: up   down right leftselect tab quithelp main menu info new load  save settings",
  "New Game       initialized",
  "Settings       feature not    implemented",
  "For the menu   window use     commands up    down and the   numbers 1 to 6",
  "Bead selected",
  "Bead deselected",
  "Empty block",
  "Beads left:",
  "Cannot select  another bead",
  "Congratulationsyou have solvedthe puzzle",
  "Game saved",
  "Game loaded",
  "Error game not loaded"
 };

 //Clear Info area
 for (Row = 12; Row <= 17; Row++)
    for (Col = Screen_Cols - 18; Col <= Screen_Cols - 4; Col++)
       Screen[Row][Col] = ' ';

 Row = 11;

 switch (Msg)
 {
  case 0: for (Col = 0; Info[Msg][Col] != '\0'; Col++)
             Screen[10][Screen_Cols - 18 + Col] = Info[Msg][Col];

          break;

  case  1:
  case  2:
  case  3:
  case  4:
  case  5:
  case  6:
  case  7:
  case  8:
  case  9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
           for (Col = 0; Info[Msg][Col] != '\0'; Col++)
           {
            if (Col % 15 == 0)
              Row++;
            Screen[Row][Screen_Cols - 18 + (Col % 15)] = Info[Msg][Col];
           }

           break;
 }
}


//FUNCTION: Command_Line
void Command_Line(void)
{
 char *Command_List[] =
 {
/* 0*/ "menu",
/* 1*/ "1",
/* 2*/ "2",
/* 3*/ "3",
/* 4*/ "4",
/* 5*/ "5",
/* 6*/ "6",
/* 7*/ "up",
/* 8*/ "down",
/* 9*/ "right",
/*10*/ "left",
/*11*/ "select",
/*12*/ "tab",
/*13*/ "help",
/*14*/ "quit",
/*15*/ "main",
/*16*/ "info",
/*17*/ "new",
/*18*/ "load",
/*19*/ "save",
/*20*/ "settings"
 };

 unsigned char Command_Index;

 Info_Window(0);
 printf("Command: ");
 /*scanf("%s", Entered_Command);*/
 Input(Entered_Command, BUFFER);
 for (Command_Index = 0; Command_Index <= 20; Command_Index++)
 {
  if (strcmp(Command_List[Command_Index], Entered_Command) == 0)
  {
   break;
  }
 }

 switch (Command_Index)
 {
  //menu
  case  0: CurrentWindow = Main;
           Status_Window();
           break;

  //menu options 1 to 6
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6: if (CurrentWindow == Menu)
          {
           HighlightedMenuOption = Command_Index;
           MainMenu(HighlightedMenuOption);

           switch (Command_Index)
           {
            case 1: Info_Window(3);
                    break;

            case 2: Board_Initialization();   //Resets the boards beads
                    Chinese_Checkers_Board(); //Writes to Screen array
                    Board_Cursor();
                    CurrentWindow = Info;
                    Info_Window(4);
                    Status_Window();
                    break;

            case 3: Load_Game();
                    break;

            case 4: Save_Game();
                    break;

            case 5: Info_Window(5);
                    break;

            case 6: strcpy(Entered_Command, (char *) Command_List[14]);
                    break;
           }

          }
          else
              Info_Window(2);

          break;


  //up
  case  7: switch (CurrentWindow)
           {
            case Main:
                 if ( (Current_Board_Row == 0) &&
                     ((Current_Board_Col >= 3) && (Current_Board_Col <= 5))
                    )
                   Current_Board_Row = 8;
                 else if (  (Current_Board_Row == 3) &&
                          (((Current_Board_Col >= 0) && (Current_Board_Col <= 2)) ||
                           ((Current_Board_Col >= 6) && (Current_Board_Col <= 8))
                          )
                         )
                        Current_Board_Row = 5;
                 else
                     Current_Board_Row--;

                 Board_Cursor();
                 CurrentWindow = Info;
                 Status_Window();
                 break;

            case Menu:
                 if (HighlightedMenuOption == 1)
                   HighlightedMenuOption = 7;

                 MainMenu(--HighlightedMenuOption);
                 break;

            case Info:
                 break;
           }

           break;

  //down
  case  8: switch (CurrentWindow)
           {
            case Main:
                 if ( (Current_Board_Row == 8) &&
                     ((Current_Board_Col >= 3) && (Current_Board_Col <= 5))
                    )
                   Current_Board_Row = 0;
                 else if (  (Current_Board_Row == 5) &&
                          (((Current_Board_Col >= 0) && (Current_Board_Col <= 2)) ||
                           ((Current_Board_Col >= 6) && (Current_Board_Col <= 8))
                          )
                         )
                        Current_Board_Row = 3;
                 else
                     Current_Board_Row++;

                 Board_Cursor();
                 CurrentWindow = Info;
                 Status_Window();

                 break;

            case Menu:
                 if (HighlightedMenuOption == 6)
                   HighlightedMenuOption = 0;

                 MainMenu(++HighlightedMenuOption);
                 break;

            case Info:
                 break;
           }

           break;

  //right
  case  9: if (CurrentWindow == Main)
           {
            if ( (Current_Board_Col == 8) &&
                ((Current_Board_Row >= 3) && (Current_Board_Row <= 5)
                )
               )
              Current_Board_Col = 0;
            else if (  (Current_Board_Col == 5) &&
                     (((Current_Board_Row >= 0) && (Current_Board_Row <= 2)) ||
                      ((Current_Board_Row >= 6) && (Current_Board_Row <= 8))
                     )
                    )
                   Current_Board_Col = 3;

            else
                Current_Board_Col++;

            Board_Cursor();
            CurrentWindow = Info;
            Status_Window();
           }

           if (CurrentWindow == Menu)
             Info_Window(6);

           break;

  //left
  case 10: if (CurrentWindow == Main)
           {
            if ( (Current_Board_Col == 0) &&
                ((Current_Board_Row >= 3) && (Current_Board_Row <= 5)
                )
               )
              Current_Board_Col = 8;
            else if (  (Current_Board_Col == 3) &&
                     (((Current_Board_Row >= 0) && (Current_Board_Row <= 2)) ||
                      ((Current_Board_Row >= 6) && (Current_Board_Row <= 8))
                     )
                    )
                   Current_Board_Col = 5;
            else
                Current_Board_Col--;

            Board_Cursor();
            CurrentWindow = Info;
            Status_Window();

           }

           if (CurrentWindow == Menu)
             Info_Window(6);

           break;

  //select
  case 11: switch (CurrentWindow)
           {
            case Main:
                 if (board[Current_Board_Row][Current_Board_Col] == ' ' &&
                     Selected_Bead_Row == 'N' && Selected_Bead_Col == 'N'
                    )
                   //Empty block
                   Info_Window(9);
                 else if (board[Current_Board_Row][Current_Board_Col] == ' ' &&
                          Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N'
                         )
                        //Do calculations for valid hop
                        Bead_Manager();
                 else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
                          Selected_Bead_Row == 'N' && Selected_Bead_Col == 'N'
                         )
                 {
                  //Selects a bead
                  Selected_Bead_Row = Current_Board_Row;
                  Selected_Bead_Col = Current_Board_Col;
                  Info_Window(7);
                 }
                 else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
                          Selected_Bead_Row == Current_Board_Row &&
                          Selected_Bead_Col == Current_Board_Col
                         )
                 {
                  //Deselects a bead & clears bead cursor
                  Board_Cursor();
                  Selected_Bead_Row = 'N';
                  Selected_Bead_Col = 'N';
                  Board_Cursor();
                  Info_Window(8);
                 }
                 else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
                          Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N'
                         )
                 {
                  //Cannot select another bead
                  Info_Window(11);
                 }

                 break;

            case Menu:
                 switch (HighlightedMenuOption)
                 {
                  case 1: Info_Window(3);
                          break;

                  case 2: Board_Initialization();   //Resets the boards beads
                          Chinese_Checkers_Board(); //Writes to Screen array
                          Board_Cursor();
                          CurrentWindow = Info;
                          Info_Window(4);
                          Status_Window();
                          break;

                  case 3: Load_Game();
                          break;

                  case 4: Save_Game();
                          break;

                  case 5: Info_Window(5);
                          break;

                  case 6: strcpy(Entered_Command, (char *) Command_List[14]);
                          break;
                 }
                 break;

            case Info:
                 break;
           }
           break;

  //tab
  case 12: Status_Window();
           break;

  //help
  case 13: Info_Window(3);
           break;

  //quit
  case 14: MainMenu(6);
           break;

  //main
  case 15: CurrentWindow = Info;
           Status_Window();
           break;

  //info
  case 16: CurrentWindow = Menu;
           Status_Window();
           break;

  //new
  case 17: Board_Initialization();   //Resets the boards beads
           Chinese_Checkers_Board(); //Writes to Screen array
           Board_Cursor();
           CurrentWindow = Info;
           Status_Window();
           break;

  //load
  case 18: Load_Game();
           break;

  //save
  case 19: Save_Game();
           break;

  //settings
  case 20: Info_Window(5);
           break;

  //Error message display
  case 21: Info_Window(1);
           break;
 }
}


//FUNCTION: Board_Cursor
void Board_Cursor(void)
{
 //Clears previous cursor position
 //up command
 if (strcmp(Entered_Command, "up") == 0)
 {
  if (Current_Board_Row == 8)
  {
   //Clear board row 0
   Current_Board_Row = 0;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Row = 8;
  }
  else if (  (Current_Board_Row == 5) &&
           (
            ((Current_Board_Col >= 0) && (Current_Board_Col <= 2)) ||
            ((Current_Board_Col >= 6) && (Current_Board_Col <= 8))
           )
          )
  {
   //Clear board row 3
   Current_Board_Row = 3;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Row = 5;
  }
  else
  {
   //Clear the previous cursor position
   Screen[(Current_Board_Row + 1) * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[(Current_Board_Row + 1) * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
  }
 }

 //down command
 else if (strcmp(Entered_Command, "down") == 0)
 {
  if (Current_Board_Row == 0)
  {
   //Clear board row 8
   Current_Board_Row = 8;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Row = 0;
  }
  else if ( (Current_Board_Row == 3) &&
           (
            ((Current_Board_Col >= 0) && (Current_Board_Col <= 2)) ||
            ((Current_Board_Col >= 6) && (Current_Board_Col <= 8))
           )
          )
  {
   //Clear board row 5
   Current_Board_Row = 5;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Row = 3;
  }
  else
  {
   //Clear the previous cursor position
   Screen[(Current_Board_Row - 1) * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[(Current_Board_Row - 1) * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
  }
 }

 //right command
 else if (strcmp(Entered_Command, "right") == 0)
 {
  if (Current_Board_Col == 0)
  {
   //Clear board column 8
   Current_Board_Col = 8;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Col = 0;
  }
  else if (  (Current_Board_Col == 3) &&
           (
            ((Current_Board_Row >= 0) && (Current_Board_Row <= 2)) ||
            ((Current_Board_Row >= 6) && (Current_Board_Row <= 8))
           )
          )
  {
   //Clear board column 5
   Current_Board_Col = 5;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Col = 3;
  }
  else
  {
   //Clear the previous cursor position
   Screen[Current_Board_Row * 2 + 3][(Current_Board_Col - 1) * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][(Current_Board_Col - 1) * 4 + 8] = ' ';
  }
 }

 //left command
 else if (strcmp(Entered_Command, "left") == 0)
 {
  if (Current_Board_Col == 8)
  {
   //Clear board column 0
   Current_Board_Col = 0;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Col = 8;
  }
  else if (  (Current_Board_Col == 5) &&
           (
            ((Current_Board_Row >= 0) && (Current_Board_Row <= 2)) ||
            ((Current_Board_Row >= 6) && (Current_Board_Row <= 8))
           )
          )
  {
   //Clear board column 3
   Current_Board_Col = 3;
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ' ';
   Current_Board_Col = 5;
  }
  else
  {
   //Clear the previous cursor position
   Screen[Current_Board_Row * 2 + 3][(Current_Board_Col + 1) * 4 + 6] = ' ';
   Screen[Current_Board_Row * 2 + 3][(Current_Board_Col + 1) * 4 + 8] = ' ';
  }
 }


 //Writes new cursor position
 Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 6] = '[';
 Screen[Current_Board_Row * 2 + 3][Current_Board_Col * 4 + 8] = ']';

 //Writes selected bead's cursor position
 if (Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N')
 {
  Screen[Selected_Bead_Row * 2 + 3][Selected_Bead_Col * 4 + 6] = '[';
  Screen[Selected_Bead_Row * 2 + 3][Selected_Bead_Col * 4 + 8] = ']';
 }

 if (Valid_Bead_Hop)
 {
  //Clears the selected bead cursor after a valid hop
  Screen[Selected_Bead_Row * 2 + 3][Selected_Bead_Col * 4 + 6] = ' ';
  Screen[Selected_Bead_Row * 2 + 3][Selected_Bead_Col * 4 + 8] = ' ';
  Valid_Bead_Hop = 0;
 }
}


//FUNCTION: Bead_Manager
void Bead_Manager(void)
{
 /* The Bead_Manager will validate correct & incorrect bead moves, keeps track
    of total amount of beads left, writes to the board array */

 if (Selected_Bead_Row != 'N' && board[Current_Board_Row][Current_Board_Col] == ' ')
 {
  if (Current_Board_Col == Selected_Bead_Col)
  {
   if (((Current_Board_Row > Selected_Bead_Row) && (Current_Board_Row - Selected_Bead_Row == 2)) ||
       ((Selected_Bead_Row > Current_Board_Row) && (Selected_Bead_Row - Current_Board_Row == 2))
      )
     if (board[(Current_Board_Row + Selected_Bead_Row)/2][Current_Board_Col] == 'X')
     {
      //Erase the 2 beads & write the 1 bead
      board[Selected_Bead_Row][Selected_Bead_Col] = ' ';
      board[(Current_Board_Row + Selected_Bead_Row)/2][Current_Board_Col] = ' ';
      board[Current_Board_Row][Current_Board_Col] = 'X';
      Chinese_Checkers_Board();
      Valid_Bead_Hop = 1;
      Board_Cursor();
      Selected_Bead_Row = 'N';
      Selected_Bead_Col = 'N';
      Board_Cursor();
      if (beads > 1) beads--;
      Info_Window(10);
      Screen[12][Screen_Cols - 6] = beads / 10 + 48;
      Screen[12][Screen_Cols - 5] = beads % 10 + 48;

     }
  }
  else if (Current_Board_Row == Selected_Bead_Row)
  {
   if (((Current_Board_Col > Selected_Bead_Col) && (Current_Board_Col - Selected_Bead_Col == 2)) ||
       ((Selected_Bead_Col > Current_Board_Col) && (Selected_Bead_Col - Current_Board_Col == 2))
      )
     if (board[Current_Board_Row][(Current_Board_Col + Selected_Bead_Col)/2] == 'X')
     {
      board[Selected_Bead_Row][Selected_Bead_Col] = ' ';
      board[Current_Board_Row][(Current_Board_Col + Selected_Bead_Col)/2] = ' ';
      board[Current_Board_Row][Current_Board_Col] = 'X';
      Chinese_Checkers_Board();
      Valid_Bead_Hop = 1;
      Board_Cursor();
      Selected_Bead_Row = 'N';
      Selected_Bead_Col = 'N';
      Board_Cursor();
      if (beads > 1) beads--;
      Info_Window(10);
      Screen[12][Screen_Cols - 6] = beads / 10 + 48;
      Screen[12][Screen_Cols - 5] = beads % 10 + 48;
     }
  }
 }

 if (beads == 1 && board[4][4] == 'X')
   Info_Window(12);
}


//FUNCTION: Save_Game
void Save_Game(void)
{
 FILE *fp;
 unsigned char i,j;
 struct Saved_Data_Struct
 {
  unsigned char board[9][9],
                beads,
                Current_Board_Row,
                Current_Board_Col,
                Selected_Bead_Row,
                Selected_Bead_Col;
 } Saved_Data;

 for (i = 0; i<=8; i++)
    for (j = 0; j<=8; j++)
       Saved_Data.board[i][j] = board[i][j];

 Saved_Data.beads = beads;
 Saved_Data.Current_Board_Row = Current_Board_Row;
 Saved_Data.Current_Board_Col = Current_Board_Col;
 Saved_Data.Selected_Bead_Row = Selected_Bead_Row;
 Saved_Data.Selected_Bead_Col = Selected_Bead_Col;

 fp = fopen("Chinese_Checkers.save", "wb");
 if (fp)
 {
  fwrite(&Saved_Data, sizeof(Saved_Data), 1, fp);
  fclose(fp);
 }

 Info_Window(13);
}


//FUNCTION: Load_Game
void Load_Game(void)
{
 FILE *fp;
 unsigned char i,j;
 struct Saved_Data_Struct
 {
  unsigned char board[9][9],
                beads,
                Current_Board_Row,
                Current_Board_Col,
                Selected_Bead_Row,
                Selected_Bead_Col;
 } Saved_Data;

 fp = fopen("Chinese_Checkers.save", "rb");
 if (fp)
 {
  fread(&Saved_Data, sizeof(Saved_Data), 1, fp);
  fclose(fp);


  for (i = 0; i<=8; i++)
     for (j = 0; j<=8; j++)
        board[i][j] = Saved_Data.board[i][j];

  beads = Saved_Data.beads;
  Current_Board_Row = Saved_Data.Current_Board_Row;
  Current_Board_Col = Saved_Data.Current_Board_Col;
  Selected_Bead_Row = Saved_Data.Selected_Bead_Row;
  Selected_Bead_Col = Saved_Data.Selected_Bead_Col;

  Info_Window(14);
  Chinese_Checkers_Board();
  Board_Cursor();
  CurrentWindow=Info;
  Status_Window();
 }
 else Info_Window(15);
}


/*FUNCTION:*/
void Input(char *string_ptr, size_t num_characters)
{                                                                   
 if (fgets(string_ptr, num_characters, stdin) != NULL)              
 {                                                                  
  size_t last = strlen(string_ptr) - 1;                             
                                                                    
  if (string_ptr[last] == '\n')                                     
    string_ptr[last] = '\0';                                        
 }                                                                  
}
