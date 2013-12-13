// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __PARTICLE_H_
#define __PARTICLE_H_

#include "object.h"

#define PARTICLE_LIFETIME 1.5    // lifetime in seconds

// particles are places at fixed positions in the scene with a specific color
// they exist only for a small amount of time
// they are generated randomely around the camera
class particle : public object  
{
private:
  float r;  // red amount of color
  float g;  // green amount of color
  float b;  // blue amount of color
  float a;  // alpha amount of color

public:
  particle (float x, float y, float z, float r, float g, float b, float a);
  virtual ~particle ();

  virtual int Typ ();
  virtual void draw ();  // draws a single point
  virtual void control ();  // decreases lifetime of particle

  // just for compatility reasons
  virtual void move ();
  virtual void clear_collision_info ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif

