// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//



#ifndef __TURRET_H_
#define __TURRET_H_


#include "building.h"
#include "objectlist.h"

// class for rotating turrets with great fire power
class turret : public building  
{
private:
  weapon_t *weapons;      // weapons of fighter
  int weaponcount;      // number of weapons
  int active_weapon;      // active_weapon
  float lastshot;        // time since last shot
  void shoot ();        // fires missiles for all active wepaons
  void switch_weapon ();    // switches to the next weapon
  objectlist* targetlist;    // targets to be shot
  listobj* target;      // current target

  void cleanup_targetlist ();  // delete all entrys in targetlist from destroyed objects
public:
  turret (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
       float Rotspd, int Mass, int Energy, GLfloat *norms,int *sw);
  virtual ~turret ();
  virtual int Typ ();
  virtual void draw ();
  virtual void control ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif

