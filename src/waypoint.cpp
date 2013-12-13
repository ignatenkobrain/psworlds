// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "waypoint.h"

waypoint::waypoint (float x, float y, float z, int End, int num):
      object (x,y,z,0,0,0)
{
  end = End;
  reached = 0;
  serial = num;
}

waypoint::~waypoint ()
{

}

int waypoint::Typ ()
{
  return WAYPOINT;
}

int waypoint::get_end ()
{
  return end;
}

int waypoint::get_reached ()
{
  return reached;
}

void waypoint::set_reached ()
{
  reached=1;
}

void waypoint::reset_reached ()
{
  reached=0;
}

int waypoint::get_serial ()
{
  return serial;
}

// just for compatility reasons
void waypoint::draw () {}
void waypoint::move () {}
void waypoint::control () {}
void waypoint::clear_collision_info () {}
void waypoint::set_weaponcount (int c) {}
void waypoint::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void waypoint::add_object (int target,object* obj) {}

