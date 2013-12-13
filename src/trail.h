// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __TRAIL_H_
#define __TRAIL_H_


#include "psworlds.h"
#include "object.h"


#define TRAIL_FOLLOW 0
#define TRAIL_FREE 1
#define TRAIL_MISSILE 2
#define TRAIL_HOMING 3


#define TRAILFADE 1.0
#define TRAILLIFT 2.2
#define TRAIL_MISSILE_LENGTH 250.0
#define TRAIL_HOMIE_LENGTH 40.0



class trail : public object  
{
private:
  GLfloat startalpha; // alpha values for start and end point
  GLfloat endalpha;
  GLfloat start[3];  // start coordinates
  GLfloat startheight;  // the height of the trail at starting coordinates
  GLuint state;    // defines the current state of the trail
  GLuint initialstate;  // defines what kind of shot this used to trail
  object* leader;    // the object (shot) the trail is trailing
  GLfloat dist;    // the length of the trail
  double *matrix;    // the rotation matrix of the shot
  double fixmatrix[16];  // a constant matrix, used for dumb torpedoes and standard shots
              // necessary for divorcing trails
  int gotmatrix;      // TODO: this is a hack! the rotation matrix should be handed
              // to the shot at construction time, not grabbed when the
              // moment seems to be appropriate

public:
  trail (float X,float Y,float Z,float rotX, float rotY, float rotZ, object* lead, GLuint state);
  virtual ~trail ();

  virtual int Typ ();
  virtual void draw ();
  virtual void control ();

  void divorce ();  // release the trail from its leader because the leader probably died
  void grabmatrix ();  // copy the current matrix to fixmatrix

  // just for compatility reasons
  virtual void move ();
  virtual void clear_collision_info ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);


};



#endif

