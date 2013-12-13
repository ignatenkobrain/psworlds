// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "object.h"
#include "psworlds.h"

object::object (float X,float Y,float Z,float rotX, float rotY, float rotZ)
{
  x=X;
  y=Y;
  z=Z;
  rotx=rotX;
  roty=rotY;
  rotz=rotZ;
  dx=0;
  dy=0;
  dz=0;
  destroy=0;
  movement_state=MV_NORMAL;
}

object::~object ()
{

}

// decreases time_to_live and sets destroy if necessary
void object::update_lifetime ()
{
  if (movement_state==MV_DESTROY)
  {
    GLfloat ticks = gettime ();
    time_to_live-=ticks;
    if (time_to_live<=0)
    {
      destroy=1;
    }
  }
}

// access functions to private variables
void object::setx (float value)
{
  x=value;
}

void object::sety (float value)
{
  y=value;
}

void object::setz (float value)
{
  z=value;
}

void object::setrotx (float value)
{
  while (value>=360) value-=360;
  while (value<0) value+=360;
  rotx=value;
}

void object::setroty (float value)
{
  while (value>=360) value-=360;
  while (value<0) value+=360;
  roty=value;
}

void object::setrotz (float value)
{
  while (value>=360) value-=360;
  while (value<0) value+=360;
  rotz=value;
}

GLfloat object::getx ()
{
  return x;
}

GLfloat object::gety ()
{
  return y;
}

GLfloat object::getz ()
{
  return z;
}

GLfloat object::getdx ()
{
  return dx;
}

GLfloat object::getdy ()
{
  return dy;
}

GLfloat object::getdz ()
{
  return dz;
}

GLfloat object::getrotx ()
{
  return rotx;
}

GLfloat object::getroty ()
{
  return roty;
}

GLfloat object::getrotz ()
{
  return rotz;
}

int object::getID ()
{
  return ID;
}

void object::setID (int value)
{
  ID=value;
}

int object::getdestroy ()
{
  return destroy;
}

void object::set_lifetime (float value)
{
  time_to_live=value;
}

int object::getmvstate ()
{
  return movement_state;
}

void object::set_enemystatus (int value)
{

}

void object::setName (char* value)
{
  name=(char*)malloc (strlen (value)+1);
  strcpy (name,value);
}

char* object::getName ()
{
  return name;
}
