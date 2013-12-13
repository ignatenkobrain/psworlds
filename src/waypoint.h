// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __WAYPOINT_H_
#define __WAYPOINT_H_
#include "object.h"

#define WAYPT_DIST 100.0
#define WAYPT_PL_DIST 200.0


// waypoints are fixed positions in the scene, which can be points for objects to be reached
class waypoint : public object  
{
private:
  int end;   // is waypoint the end or should we go on the next ?
  int reached; // has waypoint already been reached ?
  int serial;   // serial number
public:
  waypoint (float x, float y, float z, int End, int num);
  virtual ~waypoint ();

  virtual int Typ ();
  int get_end ();
  int get_reached ();
  void set_reached ();
  void reset_reached ();
  int get_serial ();

  // just for compatility reasons
  virtual void draw ();
  virtual void control ();
  virtual void move ();
  virtual void clear_collision_info ();

  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};

#endif

