// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __BRIEFING_H_
#define __BRIEFING_H_

#include "psworlds.h"
#include "tgaloader.h"


class briefing  
{
private:
  char* txt[50];    // mission text
  GLfloat xl[50], yl[50];  // text location
  textureObj* bg;    // background image flying around
  GLfloat txtalpha;  // text blending
  GLfloat txtalphac;  // alpha change
  GLfloat tc;      // texture-coordinate
  GLfloat vol;    // music volume
  GLfloat voladjust;
  GLfloat xscaler;  // briefing scaler
  GLfloat zscaler;  // layer scaler
  GLfloat xsc, zsc;  // scale speed

  int mnr;      // mission number
  int killme;      // quitting flag

  void control ();
  void draw ();


public:
  briefing (int mnr);
  virtual ~briefing ();

  void run ();
};

#endif


