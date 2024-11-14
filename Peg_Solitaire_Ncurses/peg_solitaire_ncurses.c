/*Date:    29May2013Wed 16:50
//Updated: 14Nov2024Thu 15:19:52
//File:    peg_solitaire_ncurses.c
//Author:  Ashraf
//Email:   ashraf.allie01@gmail.com
//Desc:    The goal is to get 1 bead in the centre of the board. To eliminate a
           bead use an adjacent bead to jump over the intended bead to be
           eliminated. A bead may only jump horizontally or vertically over 1 or
           more beads provided that there is an empty space in front of the bead
           that is intended to be eliminated.


           This program is free software: you can redistribute it and/or modify
           it. This program is distributed in the hope that it will be useful,
           but WITHOUT ANY WARRANTY; without even the implied warranty of
           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


           To compile with gcc command:
            gcc -Wall -Werror -Wextra -Wunused -pedantic `ncursesw6-config --cflags` peg_solitaire_ncurses.c `ncursesw6-config --libs` -o peg_solitaire_ncurses
           To run: ./peg_solitaire_ncurses
*/


/*----------------
//Macro Defintions
----------------*/
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define ESCAPE_KEY 27
#define ENTER_KEY  13
#define TAB_KEY     9


/*-------------------------
//Data Structure Defintions (struct, enum union)
-------------------------*/
enum SubWindowType {Main, Menu, Info};


/*----------------------------
//Global Variable Declarations
----------------------------*/
WINDOW *MainSubWin, *MenuSubWin, *InfoSubWin, *StatusSubWin;

int board[9][9],
    beads,
    Current_Board_Row,
    Current_Board_Col,
    Selected_Bead_Row,
    Selected_Bead_Col,
    Screen_Rows,
    Screen_Cols,
    HighlightedMenuOption = 1,
    Valid_Bead_Hop = 0;


enum SubWindowType CurrentSubWindow = Main;


/*---------------------
//Function Declarations (prototypes)
---------------------*/
void Ncurses_Initialization(void);
void Splash_Screen(void);
void Term_Screen_Size_Detection(void);
void Board_Initialization(void);
void ScreenBorderSetup(void);
void ScreenSubWindowSetup(void);
void MainMenu(unsigned char HighlightedChoice);
void Peg_Solitaire_Board(void);
void Status_Window(void);
void Info_Window(unsigned char Msg);
void ChoiceSelection(void);
void Board_Cursor_Clear(void);
void Board_Cursor(void);
void Bead_Manager(void);
void Stale_Mate_Checker(void);
void Save_Game(void);
void Load_Game(void);
void DestroySubWindows(void);
void Ncurses_End(void);


/*------------
//Main Program
------------*/
int main(void)
{
 Ncurses_Initialization();
 Splash_Screen();
 Term_Screen_Size_Detection();
 Board_Initialization();
 ScreenBorderSetup();
 ScreenSubWindowSetup();
 Status_Window();
 MainMenu(HighlightedMenuOption);
 Peg_Solitaire_Board();
 Board_Cursor();
 Info_Window(0);
 refresh();
 ChoiceSelection();
 Ncurses_End();
 return 0;
}


/*--------------------
//Function Definitions (Implementation)
--------------------*/


/*FUNCTION:*/
void Ncurses_Initialization(void)
{
 initscr();
 if (has_colors() && start_color() == OK)
 {
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(3, COLOR_CYAN,    COLOR_BLACK);
 }
 cbreak();
 noecho();
 nonl();
 intrflush(stdscr, TRUE);
 keypad(stdscr, TRUE);
}


/*FUNCTION:*/
void Splash_Screen(void)
{
 printw(
 "Peg Solitaire\n\n"
 "Created by Ashraf\n"
 "ashraf.allie01@gmail.com\n\n"
 "Ncurses version + mouse & colour support\n"
 "Copyleft 2013 - 2024\n\n"
 "Press enter key to continue\n\n");

 getch();
 clear();
}


/*FUNCTION:*/
void Term_Screen_Size_Detection(void)
{
 if ((LINES <= 23) || (COLS <= 65))
 {
  printw("Window size too small\nPlease resize\n"
         "Your screen size is:\n"
         "Screen_Rows = %d\n"
         "Screen_Cols = %d\n\n"
         "Screen rows must be greater than 23\n"
         "Screen columns must be greater than 65\n\n",
         LINES, COLS);

  getch();
  endwin();
  exit(EXIT_FAILURE);
 }

 Screen_Rows = 22; /* 23 rows 0 to 22*/
 Screen_Cols = 64; /* 65 columns 0 to 64*/
}


/*FUNCTION:*/
void Board_Initialization(void)
{
 unsigned char board_row, board_col;


 beads = 44;
 Current_Board_Row = 4;
 Current_Board_Col = 4;
 Selected_Bead_Row = 'N',
 Selected_Bead_Col = 'N';


 /*Initializing the 4 non used 3x3 blocks to NULL (N)*/
 for (board_row = 0; board_row <= 2; board_row++)
    for (board_col = 0; board_col <= 2; board_col++)
    {
     board[board_row][board_col] = 'N';
     board[8 - board_row][board_col] = 'N';
     board[board_row][8 - board_col] = 'N';
     board[8 - board_row][8 - board_col] = 'N';
    }

 /*Initialize all remaining blocks to beads represented by (X)*/
 for (board_row = 0; board_row <= 2; board_row++)
    for (board_col = 3; board_col <= 5; board_col++)
    {
     board[board_row][board_col] = 'X';     /*Top 3x3 block*/
     board[board_col][board_row] = 'X';     /*Left 3x3 block*/
     board[8 - board_row][board_col] = 'X'; /*Bottom 3x3 block*/
     board[board_col][8 - board_row] = 'X'; /*Right 3x3 block*/
     board[board_row + 3][board_col] = 'X'; /*Central 3x3 block*/
                                      /*Note the central element block is set!*/
    }

 /*Uninitialize the central block represented by a space*/
 board[Current_Board_Row][Current_Board_Col] = ' ';
}


/*FUNCTION:*/
void ScreenBorderSetup(void)
{
 unsigned char Row, Col;


 attrset(COLOR_PAIR(1));
 mvaddch(0,           0,           ACS_ULCORNER); /*top left corner*/
 mvaddch(0,           Screen_Cols, ACS_URCORNER); /*top right corner*/
 mvaddch(Screen_Rows, 0,           ACS_LLCORNER); /*bottom left corner*/
 mvaddch(Screen_Rows, Screen_Cols, ACS_LRCORNER); /*bottom right corner*/


 for (Col = 1; Col < Screen_Cols; Col++)        /*top & bottom borders*/
 {
  mvaddch(0,           Col, ACS_HLINE);
  mvaddch(Screen_Rows, Col, ACS_HLINE);
 }

 for (Row = 1; Row < Screen_Rows; Row++)      /*left, right & internal borders*/
 {
  mvaddch(Row, 0,                ACS_VLINE);
  mvaddch(Row, Screen_Cols,      ACS_VLINE);
  mvaddch(Row, Screen_Cols - 18, ACS_VLINE);
 }

 for (Col = Screen_Cols - 17; Col < Screen_Cols; Col++)
 {
  mvaddch(9,               Col, ACS_HLINE);  /*border between Menu & Info*/
  mvaddch(Screen_Rows - 4, Col, ACS_HLINE);  /*border between Info & Status*/
 }

                                        /*Part of internal border conjunctions*/
 mvaddch(0, Screen_Cols - 18, ACS_TTEE);
 mvaddch(9, Screen_Cols - 18, ACS_LTEE);
 mvaddch(9, Screen_Cols,      ACS_RTEE);

 mvaddch(Screen_Rows - 4, Screen_Cols - 18, ACS_LTEE);
 mvaddch(Screen_Rows - 4, Screen_Cols,      ACS_RTEE);
 mvaddch(Screen_Rows,     Screen_Cols - 18, ACS_BTEE);

 attrset(COLOR_PAIR(0));
}


/*FUNCTION:*/
void ScreenSubWindowSetup(void)
{
 MainSubWin   = subwin(stdscr, 21, 45,  1,  1);
 MenuSubWin   = subwin(stdscr,  8, 17,  1, 47);
 InfoSubWin   = subwin(stdscr,  8, 17, 10, 47);
 StatusSubWin = subwin(stdscr,  3, 17, 19, 47);
}


/*FUNCTION:*/
void MainMenu(unsigned char HighlightedChoice)
{
 unsigned char Menu_Row, Menu_Col;
 char          *MainMenuArray[] =
 {
  "Menu:",
  "1. Intro / Help",
  "2. New Game",
  "3. Load Game",
  "4. Save Game",
  "5. Settings",
  "6. Quit"
 };


 wclear(MenuSubWin);
 wprintw(MenuSubWin, " %s", MainMenuArray[0]);
 for (Menu_Row = 1; Menu_Row <= 6; Menu_Row++)
 {
  if (HighlightedChoice == Menu_Row)
    wattron(MenuSubWin, A_REVERSE);

  for (Menu_Col = 0; MainMenuArray[Menu_Row][Menu_Col] != '\0'; Menu_Col++)
  {
   if (Menu_Col == 2 && HighlightedChoice == Menu_Row)
     waddch(MenuSubWin, '[');
   else mvwaddch(MenuSubWin, Menu_Row+1, Menu_Col + 1,
                 MainMenuArray[Menu_Row][Menu_Col]);
  }

  if (HighlightedChoice == Menu_Row)
  {
   waddch(MenuSubWin, ']');
   wattroff(MenuSubWin, A_REVERSE);
  }
 }

 wrefresh(MenuSubWin);
}


/*FUNCTION:*/
void Peg_Solitaire_Board(void)
{
 unsigned char Game_Board_Row, Game_Board_Col;
 char          *Game_Board[] =
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


 /*Writes the Game_Board & the board beads to Screen Array*/
 for (Game_Board_Row = 0; Game_Board_Row < 21; Game_Board_Row++)
    for (Game_Board_Col = 0; Game_Board[Game_Board_Row][Game_Board_Col] != '\0';
         Game_Board_Col++)
    {
     mvwaddch(MainSubWin, Game_Board_Row, Game_Board_Col + 2,
              Game_Board[Game_Board_Row][Game_Board_Col]);

     if ((Game_Board_Row % 2) == 0 && (Game_Board_Row / 2) != 0)
       if ((Game_Board_Col % 4) == 0 && (Game_Board_Col / 4) != 0)
         if ((Game_Board_Row / 2) - 1 <= 8 && (Game_Board_Col / 4) - 1 <= 8)
         {
          if (board[(Game_Board_Row / 2) - 1][(Game_Board_Col / 4) - 1] == 'N')
            mvwaddch(MainSubWin, Game_Board_Row, Game_Board_Col+2, ' ');
          else
          {
           mvwaddch(MainSubWin, Game_Board_Row, Game_Board_Col + 2,
                    board[(Game_Board_Row / 2) - 1][(Game_Board_Col / 4) - 1]);
          }
         }
    }
}


/*FUNCTION:*/
void Status_Window(void)
{
 wclear(StatusSubWin);

 switch (CurrentSubWindow)
 {
  case Main:
       CurrentSubWindow = Menu;
       mvwprintw(StatusSubWin, 2, 1, "Window: Menu");
       break;


  case Menu:
       CurrentSubWindow = Main;
       mvwprintw(StatusSubWin, 0, 1, "Row: %c", Current_Board_Row + 48);
       mvwprintw(StatusSubWin, 1, 1, "Col: %c", Current_Board_Col + 48);
       mvwprintw(StatusSubWin, 2, 1, "Window: Main");
       break;


  default:
  break;
 }

 wrefresh(StatusSubWin);
}


/*FUNCTION:*/
void Info_Window(unsigned char Msg)
{
 char *Info[] =
 {
  "Info:",
  "Options 1 to 6   only work in     the menu window",
  "Keys: up down    left right tab   enter. Shortcut  keys for menu    are numbers 1    to 6 ",
  "Settings         feature not      implemented",
  "Bead selected",
  "Bead deselected",
  "Empty block",
  "Beads left:",
  "Cannot select    another bead",
  "Congratulations  you have solved  the puzzle",
  "New Game         initialized",
  "Stale mate no    moves possible",
  "Game saved",
  "Game loaded",
  "Error game not   loaded"
 };


 wclear(InfoSubWin);
 mvwprintw(InfoSubWin, 0, 1, "%s", Info[0]);

 if (Msg != 0)
   mvwprintw(InfoSubWin, 2, 1, "%s", Info[Msg]);

 wrefresh(InfoSubWin);
}


/*FUNCTION:*/
void ChoiceSelection(void)
{
 int    Key;
 int    EnteredChoice;
#ifdef NCURSES_MOUSE_VERSION
 char   total_values;
 char   Row_Value;
 char   Col_Value;
 MEVENT Mouse_Event;


 mousemask(ALL_MOUSE_EVENTS, NULL);
#endif


 do
 {
  Key = getch();


  switch (Key)
  {
   case KEY_DOWN:
        if (CurrentSubWindow == Menu)
        {
         if (HighlightedMenuOption == 6) HighlightedMenuOption = 0;
         MainMenu(++HighlightedMenuOption);
        }

        if (CurrentSubWindow == Main)
        {
         Board_Cursor_Clear();

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

         else Current_Board_Row++;

         Board_Cursor();
        }
   break;


   case KEY_UP:
        if (CurrentSubWindow == Menu)
        {
         if (HighlightedMenuOption == 1) HighlightedMenuOption = 7;
         MainMenu(--HighlightedMenuOption);
        }

        if (CurrentSubWindow == Main)
        {
         Board_Cursor_Clear();

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
         else Current_Board_Row--;

         Board_Cursor();
        }
   break;


   case KEY_LEFT:
        if (CurrentSubWindow == Main)
        {
         Board_Cursor_Clear();

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

         else Current_Board_Col--;

         Board_Cursor();
        }
   break;


   case KEY_RIGHT:
        if (CurrentSubWindow == Main)
        {
         Board_Cursor_Clear();

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

         else Current_Board_Col++;

         Board_Cursor();
        }
   break;


#ifdef NCURSES_MOUSE_VERSION
   case KEY_MOUSE:
        getmouse(&Mouse_Event);

        /*Main Window*/
        if ((/*board columns 3, 4 or 5*/
             Mouse_Event.y >=  3 && Mouse_Event.y <= 19 &&
             Mouse_Event.x >= 19 && Mouse_Event.x <= 27
            ) ||
            (/*board rows 3, 4 or 5*/
             Mouse_Event.y >=  9 && Mouse_Event.y <= 13 &&
             Mouse_Event.x >=  7 && Mouse_Event.x <= 39
            )
           )
        {
         if (CurrentSubWindow == Menu) Status_Window();

         /*Checking for valid row*/
         for (Row_Value = 3, total_values = 1; total_values <= 9;
              Row_Value += 2, total_values++)
         {
          if (Mouse_Event.y == Row_Value) break; /*Valid row*/
          else if (Row_Value > Mouse_Event.y)
          {
           Row_Value = 0;
           break;
          }
         }

         /*Checking for valid col*/
         for (Col_Value = 7, total_values = 1; total_values <= 9;
              Col_Value += 4, total_values++)
         {
          if (Mouse_Event.x == Col_Value) break; /*Valid col*/
          else if (Col_Value > Mouse_Event.x)
          {
           Col_Value = 0;
           break;
          }
         }

         /*Get Board co-ordinates*/
         if ((Row_Value != 0) && (Col_Value != 0))
         {
          Board_Cursor_Clear();
          Current_Board_Row = (Row_Value - 3) / 2;
          Current_Board_Col = (Col_Value - 7) / 4;
          Board_Cursor();
          Key = ENTER_KEY;
         }
         else break;
        }

        /*Menu Window*/
        else if (Mouse_Event.y >=  3 && Mouse_Event.y <=  8 &&
                 Mouse_Event.x >= 48 && Mouse_Event.x <= 62)
        {
         if (CurrentSubWindow == Main) Status_Window();
         HighlightedMenuOption = Mouse_Event.y - 2;
         MainMenu(HighlightedMenuOption);
         Key = ENTER_KEY;
        }
   break;
#endif

   case '1': /*ascii value 49:*/
   case '2': /*50:*/
   case '3': /*51:*/
   case '4': /*52:*/
   case '5': /*53:*/
   case '6': /*54:*/
        if (CurrentSubWindow == Menu)
        {
         HighlightedMenuOption = Key - 48;
         MainMenu(HighlightedMenuOption);
         Key = ENTER_KEY;
        }

#ifdef NCURSES_MOUSE_VERSION
        else if (CurrentSubWindow == Main && Key != KEY_MOUSE)
        {
         Info_Window(1);
        }
#else
        else if (CurrentSubWindow == Main)
        {
         Info_Window(1);
        }
#endif
   break;

/*
   case ENTER_KEY:
   break;
*/

   case TAB_KEY:
        Status_Window();
   break;


   case ESCAPE_KEY:
        EnteredChoice = 6;
   break;
  }


  /*Mouse clicks tranlate enter key*/
  if (Key == ENTER_KEY)
  {
   if (CurrentSubWindow == Menu)
   {
    EnteredChoice = HighlightedMenuOption;

    switch (EnteredChoice)
    {
     /*Intro / Help*/
     case 1:
          Info_Window(2);
     break;


     /*New Game*/
     case 2:
          Board_Initialization();
          Peg_Solitaire_Board();
          Board_Cursor();
          wrefresh(MainSubWin);
          Info_Window(10);
          Status_Window();
     break;


     /*Load & Save Game*/
     case 3:
          Load_Game();
     break;


     case 4:
          Save_Game();
     break;

     /*Settings*/
     case 5:
          Info_Window(3);
     break;


     /*Quit*/
     case 6:
          Key = ESCAPE_KEY;
     break;
    }
   }
   else if (CurrentSubWindow == Main)
   {
    if (board[Current_Board_Row][Current_Board_Col] == ' ' &&
        Selected_Bead_Row == 'N' && Selected_Bead_Col == 'N')
      /*Empty block*/
      Info_Window(6);
    else if (board[Current_Board_Row][Current_Board_Col] == ' ' &&
             Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N'
            )
           /*Do calculations for valid hop*/
           Bead_Manager();
    else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
             Selected_Bead_Row == 'N' && Selected_Bead_Col == 'N'
            )
    {
     /*Selects a bead*/
     Selected_Bead_Row = Current_Board_Row;
     Selected_Bead_Col = Current_Board_Col;
     Info_Window(4);
    }
    else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
             Selected_Bead_Row == Current_Board_Row &&
             Selected_Bead_Col == Current_Board_Col
            )
    {
     /*Deselects a bead & clears bead cursor*/
     Board_Cursor();
     Selected_Bead_Row = 'N';
     Selected_Bead_Col = 'N';
     Board_Cursor();
     Info_Window(5);
    }
    else if (board[Current_Board_Row][Current_Board_Col] == 'X' &&
             Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N')
    {
     /*Cannot select another bead*/
     Info_Window(8);
    }
   }
  }
 }
 while ((Key != ESCAPE_KEY) || (EnteredChoice != 6));
}


/*FUNCTION:*/
void Board_Cursor_Clear(void)
{
 mvwaddch(MainSubWin, Current_Board_Row * 2 + 2, Current_Board_Col * 4 + 5, ' ');
 mvwaddch(MainSubWin, Current_Board_Row * 2 + 2, Current_Board_Col * 4 + 7, ' ');
}


/*FUNCTION:*/
void Board_Cursor(void)
{
 /*Writes new cursor position*/
 wattrset(MainSubWin, COLOR_PAIR(2) | A_BOLD);
 mvwaddch(MainSubWin, Current_Board_Row * 2 + 2, Current_Board_Col * 4 + 5, '[');
 mvwaddch(MainSubWin, Current_Board_Row * 2 + 2, Current_Board_Col * 4 + 7, ']');
 wattrset(MainSubWin, COLOR_PAIR(0));

 /*Writes selected bead's cursor position*/
 if (Selected_Bead_Row != 'N' && Selected_Bead_Col != 'N')
 {
  wattrset(MainSubWin, COLOR_PAIR(3));
  mvwaddch(MainSubWin, Selected_Bead_Row * 2 + 2,
           Selected_Bead_Col * 4 + 5, '[');
  mvwaddch(MainSubWin, Selected_Bead_Row * 2 + 2,
           Selected_Bead_Col * 4 + 7, ']');
  wattrset(MainSubWin, COLOR_PAIR(0));
 }

 if (Valid_Bead_Hop)
 {
  /*Clears the selected bead cursor after a valid hop*/
  mvwaddch(MainSubWin, Selected_Bead_Row * 2 + 2,
           Selected_Bead_Col * 4 + 5, ' ');
  mvwaddch(MainSubWin, Selected_Bead_Row * 2 + 2,
           Selected_Bead_Col * 4 + 7, ' ');
  Valid_Bead_Hop = 0;
 }

 wrefresh(MainSubWin);
 Status_Window();
 Status_Window();
}


/*FUNCTION:*/
void Bead_Manager(void)
{
 /*The Bead_Manager will validate correct & incorrect bead moves, keeps track
   of total amount of beads left, writes to the board array */

 if (Selected_Bead_Row != 'N' &&
     board[Current_Board_Row][Current_Board_Col] == ' ')
 {
  if (Current_Board_Col == Selected_Bead_Col)
  {
   if (((Current_Board_Row > Selected_Bead_Row) &&
        (Current_Board_Row - Selected_Bead_Row == 2)) ||
       ((Selected_Bead_Row > Current_Board_Row) &&
        (Selected_Bead_Row - Current_Board_Row == 2))
      )
     if (board[(Current_Board_Row + Selected_Bead_Row) / 2][Current_Board_Col]
                                                                         == 'X')
     {
      /*Erase the 2 beads & write the 1 bead*/
      board[Selected_Bead_Row][Selected_Bead_Col] = ' ';
      board[(Current_Board_Row + Selected_Bead_Row)/2][Current_Board_Col] = ' ';
      board[Current_Board_Row][Current_Board_Col] = 'X';
      Peg_Solitaire_Board();
      Valid_Bead_Hop = 1;
      Board_Cursor();
      Selected_Bead_Row = 'N';
      Selected_Bead_Col = 'N';
      Board_Cursor();
      if (beads > 1) beads--;
      Info_Window(7);
      mvwaddch(InfoSubWin, 2, 13, beads / 10 + 48);
      mvwaddch(InfoSubWin, 2, 14, beads % 10 + 48);
      wrefresh(InfoSubWin);
     }
  }
  else if (Current_Board_Row == Selected_Bead_Row)
  {
   if (((Current_Board_Col > Selected_Bead_Col) &&
        (Current_Board_Col - Selected_Bead_Col == 2)) ||
       ((Selected_Bead_Col > Current_Board_Col) &&
        (Selected_Bead_Col - Current_Board_Col == 2))
      )
     if (board[Current_Board_Row][(Current_Board_Col + Selected_Bead_Col) / 2]
                                                                         == 'X')
     {
      board[Selected_Bead_Row][Selected_Bead_Col] = ' ';
      board[Current_Board_Row][(Current_Board_Col + Selected_Bead_Col)/2] = ' ';
      board[Current_Board_Row][Current_Board_Col] = 'X';
      Peg_Solitaire_Board();
      Valid_Bead_Hop = 1;
      Board_Cursor();
      Selected_Bead_Row = 'N';
      Selected_Bead_Col = 'N';
      Board_Cursor();
      if (beads > 1) beads--;
      Info_Window(7);
      mvwaddch(InfoSubWin, 2, 13, beads / 10 + 48);
      mvwaddch(InfoSubWin, 2, 14, beads % 10 + 48);
      wrefresh(InfoSubWin);
     }
  }
 }

 Stale_Mate_Checker();
}


/*FUNCTION:*/
void Stale_Mate_Checker(void)
{
 /*If the total amount of beads is less than 25 then check each bead on the
   board to see whether there is an adjacent bead above, below, right or left
   of the bead

   Restraints:
    - Above row 0
    - 2 blocks above row 1
    - 2 blocks below row 7
    - Below row 8
    - Left of col 0
    - 2 blocks left of col 0
    - 2 blocks right of col 7
    - Right of col 8

   stale_mate variable = 0 or 1 which is false or true
   if 1 bead is left & is not in the centre then it is stale mate
   if a complete row or column is filled with beads
 */

 unsigned char board_row, board_col, stale_mate = 1;


 if (beads <= 24)
 {
  for (board_row = 0; board_row <= 8; board_row++)
  {
   for (board_col = 0; board_col <= 8; board_col++)
   {
    if ((board[board_row][board_col] == 'N') ||
        (board[board_row][board_col] == ' '))
      continue;

    if (board[board_row][board_col] == 'X')
    {
     if (board_row == 0)
     {
      if ((board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_row == 1)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row + 1][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_row == 7)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row - 1][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }

     }
     else if (board_row == 8)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_col == 0)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_col == 1)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col + 1] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_col == 7)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col - 1] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else if (board_col == 8)
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
     else
     {
      if ((board[board_row - 1][board_col    ] == 'X'  &&
           board[board_row - 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col + 1] == 'X'  &&
           board[board_row    ][board_col + 2] == ' ') ||
          (board[board_row + 1][board_col    ] == 'X'  &&
           board[board_row + 2][board_col    ] == ' ') ||
          (board[board_row    ][board_col - 1] == 'X'  &&
           board[board_row    ][board_col - 2] == ' ')
         )
      {
       stale_mate = 0;
       break;
      }
     }
    }
   }
  }

  if (beads == 1 && board[4][4] == 'X')
  {
   stale_mate = 0;
   Info_Window(9);
  }
  else if (beads == 1 && board[4][4] != 'X')
         stale_mate = 1;



  if (stale_mate == 1)
    Info_Window(11);
 }

 wrefresh(InfoSubWin);
}


/*FUNCTION:*/
void Save_Game(void)
{
 FILE *fp;
 unsigned char i,j;
 struct Saved_Data_Struct
 {
  int board[9][9],
      beads,
      Current_Board_Row,
      Current_Board_Col,
      Selected_Bead_Row,
      Selected_Bead_Col;
 } Saved_Data;


 for (i = 0; i <= 8; i++)
    for (j = 0; j <= 8; j++)
       Saved_Data.board[i][j] = board[i][j];

 Saved_Data.beads = beads;
 Saved_Data.Current_Board_Row = Current_Board_Row;
 Saved_Data.Current_Board_Col = Current_Board_Col;
 Saved_Data.Selected_Bead_Row = Selected_Bead_Row;
 Saved_Data.Selected_Bead_Col = Selected_Bead_Col;

 fp = fopen("peg_solitaire.save", "wb");
 if (fp)
 {
  fwrite(&Saved_Data, sizeof(Saved_Data), 1, fp);
  fclose(fp);
 }

 Info_Window(12);
}


/*FUNCTION:*/
void Load_Game(void)
{
 FILE *fp;
 unsigned char i,j;
 struct Saved_Data_Struct
 {
  int board[9][9],
      beads,
      Current_Board_Row,
      Current_Board_Col,
      Selected_Bead_Row,
      Selected_Bead_Col;
 } Saved_Data;


 fp = fopen("peg_solitaire.save", "rb");
 if (fp)
 {
  fread(&Saved_Data, sizeof(Saved_Data), 1, fp);
  fclose(fp);


  for (i = 0; i <= 8; i++)
     for (j = 0; j <= 8; j++)
        board[i][j] = Saved_Data.board[i][j];

  beads = Saved_Data.beads;
  Current_Board_Row = Saved_Data.Current_Board_Row;
  Current_Board_Col = Saved_Data.Current_Board_Col;
  Selected_Bead_Row = Saved_Data.Selected_Bead_Row;
  Selected_Bead_Col = Saved_Data.Selected_Bead_Col;

  Info_Window(13);
  Peg_Solitaire_Board();
  Board_Cursor();
  wrefresh(MainSubWin);
  Status_Window();
 }
 else Info_Window(14);
}


/*FUNCTION:*/
void DestroySubWindows(void)
{
 delwin(MainSubWin);
 delwin(MenuSubWin);
 delwin(InfoSubWin);
 delwin(StatusSubWin);
}


/*FUNCTION:*/
void Ncurses_End(void)
{
 clear();
 refresh();
 DestroySubWindows();
 endwin();
}
