// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __MENU_H_
#define __MENU_H_


#include "psworlds.h"


#define MENU_YESNO  1
#define MENU_OK    2
#define MENU_SELBOX  3
#define MENU_INGAME  4
#define MENU_TITLE  5
#define MENU_SELMISSION 6
#define MENU_GAME 7
#define MENU_LOAD 8
#define MENU_SAVE 9

#define WAIT_NONE  0
#define WAIT_QUIT  1
#define WAIT_SELMISSION 2
#define WAIT_ABORT 3
#define WAIT_QUITGAME 4
#define WAIT_LOAD 5


#define CHARLEN    16.7


// class for a menu entry

class entry
{
private:
  char* txt;      // the text
  GLfloat x,y;    // the coordinates
  GLfloat col[4];    // text color
  void *host;      // the hosting menu
  int num;      // ordnance number

public:
  int selectable;    // determines if this entry can be hilighted

  entry (int nr, char* text, int x, int y, GLfloat* cols, int selectable, void* men);
  virtual ~entry ();
  GLfloat gety ();

  void draw ();
};


// the menu class

class menu  
{
private:
  menu* par;      // the parent menu
  menu* child;    // the next menu in the hierarchy

  entry *entries[9];
  int numentries;
  int type;      // menu type
  GLfloat blink;    // the selector blink value


public:
  GLfloat selcol[4];  // color of a selected entry
  int selected;    // the selected entry
  int killme;      // if set, destroy this menu
  int new_gs;      // the renewed game state
  int waitstate;    // set wait state if a child menu is active

  menu (int type, menu* parent, char* caption);
  virtual ~menu ();
  
  void handle ();    // control the menu
  void draw ();
  void returnvalue (int val);  // handle the return value returned by a child menu
};

#endif
