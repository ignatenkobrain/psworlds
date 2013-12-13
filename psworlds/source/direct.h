// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __DIRECT_H_
#define __DIRECT_H_


#include "missile.h"

// missiles that just go in one direction until they hit
class direct : public missile  
{
public:
  direct (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ, 
       float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw);
  virtual ~direct ();
  virtual int Typ ();
  virtual void draw ();
  virtual void control ();
  virtual void ground_collision ();
  virtual void move_collide ();
};

#endif
