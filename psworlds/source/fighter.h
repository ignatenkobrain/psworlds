// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __FIGHTER_H_
#define __FIGHTER_H_


#include "model.h"

// abstract class for underwater vessels with weapons (player, computer)
class fighter : public model  
{
protected:
  weapon_t *weapons;      // weapons of fighter
  int weaponcount;      // number of weapons
  int active_weapon;      // active_weapon
  float lastshot;        // time since last shot
  void shoot ();        // fires missiles for all active wepaons
  void switch_weapon ();    // switches to the next weapon
  listobj* target;      // the current target or waypoint to reach
  listobj* waypt;        // the current waypoint to reach
  objectlist* targetlist;    // enemy targets to be destroyed
  objectlist* pathlist;    // path to be followed when no target near

public:
  fighter (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
        float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw);
  virtual ~fighter ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  listobj* getTarget ();
  listobj* getWP ();
  virtual void add_object (int target,object* obj);
  objectlist* get_pathlist ();
};

#endif

