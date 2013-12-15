// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __OTHER_H_
#define __OTHER_H_


#include "model.h"
#include "objectlist.h"

// class for objects that move but can not fire
class other : public model  
{
private:
  objectlist* path;  // list of waypoints to be followed
  object* waypoint;  // active waypoint to be reached
public:
  other (char *filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms, int *sw);
  virtual ~other ();
  virtual int Typ ();
  virtual void draw ();
  virtual void control ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif

