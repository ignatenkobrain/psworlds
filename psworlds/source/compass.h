#ifndef __COMPASS_H_
#define __COMPASS_H_
// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#include "model.h"
#include "player.h"

class compass : public model
{
private:
  player* owner;  // the ship the compass belongs to


public:
  compass (char* fname, player* pl, GLfloat *norms,int *sw);
  virtual ~compass ();

  virtual void control ();

  virtual int Typ ();
  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif

