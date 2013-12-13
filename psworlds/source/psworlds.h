// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __PSWORLDS_H_
#define __PSWORLDS_H_

#include <GL/glut.h>
#include <GL/gl.h>
#include "SDL/SDL.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"


#define WINDOWS 1
#define LINUX 2

#ifdef WIN32
  #define SYSTEM WINDOWS
#else
  #define SYSTEM LINUX
#endif


// game states
#define GS_MENU 1
#define GS_INGAME 2
#define GS_NEWGAME 3
#define GS_QUIT 4
#define GS_QUITGAME 5
#define GS_ABORT 6
#define GS_LOADGAME 7
#define GS_MISSIONEND 8
#define GS_MISSIONEND1 9


#define TITLEVOL 255


// functions
void    reshape (int w, int h, GLfloat zoom);
int      getGamestate ();
double    gettime ();
char*    getkeys ();
void*    getcam ();
void    timing ();
void    kbd ();
void*    getworld ();
void*    getmission ();
void*    getdisplaylists ();
void*    gettexturelist ();
int      getxres ();
int      getyres ();
int      getMissionNum ();
void    setMission (int ms);
void    loadmission ();
int      getMissionAdvance ();
int      getNumMission ();
void    setMissionAdvance (int ma);
void    setCheated (int c);
int      getCheated ();
int      getWire ();

#endif
