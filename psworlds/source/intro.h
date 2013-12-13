// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __INTRO_H_
#define __INTRO_H_

#include "tgaloader.h"


#define INTRO 1
#define OUTRO 2

#define MOD 1
#define MP3 2


class intro  
{
private:
  textureObj* img[20];

  GLfloat al1;    // alpha values for slide 1
  GLfloat al2;    // and slide 2

  GLfloat chl[3];    // character length for the two fonts

  int mode;      // intro or outro
  int step;      // index of current slide
  int killme;      // quitting flag
  int quit;      // second quitting flag
  GLfloat vol;    // module volume
  int musi;      // kind of music (mod or mp3)
  GLfloat beginvol;  // initial music volume
  GLfloat musicfadespeed;

  int numsteps;    // number of slideshow screens
  char* txt[20][10];  // all subtitles for slideshow. up to 5 lines of text per slide
  int fo[20][10];    // used font for text
  GLfloat tm[20];    // slide times 
  GLfloat timeleft;  // remaining time of current slide
  GLfloat prevtime;  // time in previous frame

  void control ();    // control fades and swaps
  void display ();    // display gfx
  void initsound ();  // reserve channels for LARGE mods

public:
  intro (int mode);
  virtual ~intro ();

  void run ();

};

#endif 
