// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "building.h"

building::building (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
           float Rotspd, int Mass, int Energy, GLfloat *norms,int *sw)
            :model (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,0,Mass,Energy,0,norms,sw)
{

}

building::~building ()
{

}

int building::Typ ()
{
  return BUILDING;
}

// draws the building
void building::draw ()
{
  model::draw ();
}

// controls the building
void building::control ()
{
  collision_handling ();
  if (energy<=0&&destroyable)
  {
    //explode the object
    explode ();
  }
  update_lifetime ();
}

void building::collision_handling ()
{
  // check for collisions and reduce energy
  if (collisionlist->get_first ()!=NULL)
  {
    listobj* tmpobj = collisionlist->get_first ();
    model* obj;
    GLfloat m=0;
    while (tmpobj!=NULL)
    {
      obj = (model*)tmpobj->obj;
      if (obj->Typ ()!=HOMING&&obj->Typ ()!=DIRECT&&obj->Typ ()!=TORPEDO)
      {
        m+=obj->getMass ();
      }
      else
      {
        // reduce energy for objects which are destroyable
        if (destroyable)
        {
          // you can't be shot by missiles you just fired
          if (obj->getmvstate ()!=MV_START)
          {
            energy-=obj->getEnergy ();
          }
        }
      }
      tmpobj=(listobj*)tmpobj->next;
    }
    if (m>0)
    {
      // reduce energy for objects which are destroyable
      if (destroyable)
      {
        energy-=m/5;
      }
    }
  }
}

void building::move_normal ()
{
  // nothing to do here, buildings don't move!
}

void building::move_collide () 
{
  // nothing to do here, buildings don't move!
}

// just for compatility reasons
void building::set_weaponcount (int c) {}
void building::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void building::add_object (int target,object* obj) {}

