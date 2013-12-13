// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __COCKPIT_H_
#define __COCKPIT_H_

#include "psworlds.h"
#include "tgaloader.h"
#include "player.h"
#include "compass.h"

// the various display capabilities
#define CP_TARGET 1
#define CP_MAP 2
#define CP_INFO 3
#define CP_OBJECTIVES 4
#define CP_EQUIP 5
#define CP_RADAR 6

#define ALPHA_PER_SECOND 0.3
#define RADARZOOMER 400.0

#define RADARMINZOOM 1500.0
#define RADARMAXZOOM 250.0
#define SONARPINGDELAY 8.5

#define MAPMINZOOM 1.0
#define MAPMAXZOOM 5.0
#define MAPZOOMER 1.3

#define DAMAGEFADE 0.8


typedef struct
{
  int numverts;
  GLfloat* x;
  GLfloat* y;
  GLfloat* alpha;
  GLfloat xpos;
  GLfloat ypos;
  GLfloat angle;
} crack;

typedef struct
{
  GLfloat bx, by;
  GLfloat y[2];
  GLfloat bxd, byd;
} pong;



class Cockpit  
{
private:
  textureObj *left;  // left texture
  textureObj *mid;  // middle texture
  textureObj *rite;  // right texture
  textureObj *strip;  // remaining strip to the right
  textureObj *dleft;  // the left display itself
  textureObj *dmid;  // middle display
  textureObj *drite;  // right display
  textureObj *dradar;  // radar display
  textureObj *minimap;  // a 256x256 texture map of the mission map file
  textureObj *env;  // environment map

  GLuint panels;    // display list for drawing the cockpit
  GLuint leftdis;    // display list for left display
  GLuint middis;    // middle display
  GLuint ritedis;    // right display
  GLuint radardis;  // radar display
  GLuint display[3];  // states of the three displays
  GLuint xhair;    // crosshair display list
  GLfloat cpcol[3];  // panel modulate color

  GLfloat radarxzoom, radaryzoom;  // the radar zoom parameters
  GLfloat radarxpos, radarypos;  // the location of the radar origin in viewport coords
  GLfloat radarxextent, radaryextent, radaraltextent;  // the multiplier parameters
  GLfloat sonarpingtime;  // time until next sonar ping

  int pongmode;    // EASTEREGG!
  pong pg;

  crack* cracks[3];  // cracks in the windshield

  GLfloat mapzoom;  // minimap zoom factor

  GLfloat alpha;    // transparency of the displays

  player* pl;      // the player who owns the cockpit

  compass* horizon;  // the compass / artificial horizon

  void gauges ();    // draw the info in the displays

  int points1,points2;  // points for pong
  
public:
  Cockpit (int type, player* plr, void* mis);
  virtual ~Cockpit ();

  void draw ();            // draw the panels
  void setPlayer (player* pl);      // bind the cockpit to the player
  void setDisplay (int nr, int value);  // sets displayfunctions
  void displaykeys ();          // handle the display control keys
  void hit ();              // set cockpit color if hit
  void genCrack (int num);        // generate a crack in the windshield
  void recover ();            // restore panelo colors
  int  getpongable ();          // determinde easteregg-state
  void setPongmode (int p);      // set easteregg-state
  int  getPongmode ();          // get easteregg-state
  void playpong ();
};


#endif
