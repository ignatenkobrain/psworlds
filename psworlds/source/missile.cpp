// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#include "missile.h"

missile::missile (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy,GLfloat *norms,int *sw)
          :model (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,0,norms,sw)
{
  movement_state=MV_START;
}


missile::~missile ()
{

}


void missile::collision_handling ()
{
  if (collisionlist->get_first ()!=NULL)
  {
    if (movement_state!=MV_START)
    {
      movement_state=MV_COLLIDE;
    }
  }
  else
  {
    movement_state=MV_DESTROY;    
  }
}


void missile::move_collide ()
{
  // missiles explode when they collide
  destroy=1;
  mytrail->divorce ();
}



// decreases time_to_live and sets destroy if necessary
void missile::update_lifetime ()
{
  if (movement_state==MV_DESTROY)
  {
    GLfloat ticks = gettime ();
    time_to_live-=ticks;
    if (time_to_live<=0)
    {
      destroy=1;
      mytrail->divorce ();
    }
  }
}


// moves the object, no drifting for missiles
void missile::move_normal ()
{
  dx=-mat[8];
  dy=-mat[9];
  dz=-mat[10];
  GLfloat ticks = gettime ();
  x+=dx*mvspd*ticks;
  y+=dy*mvspd*ticks;
  z+=dz*mvspd*ticks;
}

// just for compatility reasons
void missile::set_weaponcount (int c)
{

}

// just for compatility reasons
void missile::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{

}

// just for compatility reasons
void missile::add_object (int target,object* obj)
{

}

