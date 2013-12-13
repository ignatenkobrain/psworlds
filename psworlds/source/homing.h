// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//



#ifndef __HOMING_H_
#define __HOMING_H_


#include "missile.h"



// missiles that follow a specific object as good as possible
class homing : public missile  
{
private:
  object* target;    // object to hit
  float acceleration;  
public:
  homing (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
       float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw);
  virtual ~homing ();
  virtual int Typ ();
  virtual void draw ();
  virtual void control ();
  void set_target (object* obj);
  void setspeed (int value);
  virtual void ground_collision ();
  virtual void move_collide ();
};

#endif

