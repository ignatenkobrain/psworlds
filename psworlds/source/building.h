// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __BUILDING_H_
#define __BUILDING_H_

#include "model.h"

// this class represents stationary objects which cannot be moved
// they can possibly be destroyed
class building : public model  
{
protected:
  virtual void collision_handling ();  // checks all objects in collision list and reacts corresponding
  virtual void move_normal ();      // movement in MV_NORMAL
  virtual void move_collide ();    // movement in MV_COLLIDE
public:
  building (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, int Mass, int Energy, GLfloat* norms,int *sw);
  virtual ~building ();
  virtual int Typ ();
  virtual void draw ();
  virtual void control ();
  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif
