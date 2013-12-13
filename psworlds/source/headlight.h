// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __HEADLIGHT_H_
#define __HEADLIGHT_H_

#include "psworlds.h"

#define HL_BEAM 0
#define HL_FLARE 1
#define HL_BEAMANDFLARE 2

#define LIGHT_ON 1
#define LIGHT_OFF 0

#define LITE_ROTSPD 40.0

#define LIGHT_FADE 1.4
#define FLAREORIGIN 4.0


// this class is a list of all flares that shall be printed in one frame
class flarelist
{
private:
  GLfloat x, y;    // predicted position on screen
  GLdouble wc[3];    // world coordinates
  GLfloat alpha;    // alpha value
  GLfloat size;    // size of the flare bitmap
  int visi;
public:
  flarelist *next;  // pointer to next
  
  flarelist (GLdouble fx, GLdouble fy, GLdouble fz, GLfloat dist, GLfloat strength);
  virtual ~flarelist ();

  void add (flarelist* nu);  // add a new flare as next pointer
  int visible ();        // check if the flare is visible at all
  void draw ();
  void prepare ();        // prepare flares by drawing yellow dots
  void finalcheck ();      // check if light isn´t obscured by reading framebuffer
};




class headlight : public object
{
private:

  GLfloat lx, ly, lz;    // actual lite position
  GLfloat lxr, lyr, lzr;  // actual lite rotation
  GLfloat cutoff, length; // cutoff and cone length parameters
  GLfloat intensity, cintensity;// the light intensity
  GLdouble  dir[16], 
        invdir[16],
        ldir[16];  // direction the light is facing, additionally to the host´s
              // orientation
  void* host;        // the hosting model
  int on;          // 0 if lite is off
  int bulb;        // defines if flare or beam or both need to be drawn
  GLfloat r,g,b;

public:
  
  headlight (float X,float Y,float Z,float rotX, float rotY, float rotZ, void* owner, float coff, float len, float intense, int spec);

  virtual ~headlight ();

  virtual int Typ ();
  virtual void draw ();
  virtual void control ();

  void lights_out ();  // destroy the light
  void toggle ();    // switch lite on/off
  void up ();      // bank up
  void down ();    // bank down
  void left ();    // turn left
  void right ();    // turn right
  int getstate ();    // return on/off state

  // just for compatility reasons
  virtual void move ();
  virtual void clear_collision_info ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);

  GLdouble* get_mat ();
  int getOn ();
  GLfloat getCutoff ();
  GLfloat getLength ();

  void change_r (GLfloat dr);
  void change_g (GLfloat dg);
  void change_b (GLfloat db);
  
  void set_r (GLfloat nr);
  void set_g (GLfloat ng);
  void set_b (GLfloat nb);

  GLfloat get_r ();
  GLfloat get_g ();
  GLfloat get_b ();
};

void loadflaregfx ();
void precalc_angles ();
void pregetProjections ();

void addflare (GLfloat fx, GLfloat fy, GLfloat fz, GLfloat strength);
void drawflares ();
void prepareflares ();
void finalizeflares ();


#endif


