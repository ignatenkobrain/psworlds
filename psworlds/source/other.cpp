// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "other.h"

other::other (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
       float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw)
        :model (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,0,norms,sw)
{

}

other::~other ()
{

}

int other::Typ ()
{
  return OTHER;
}

// draws the object
void other::draw ()
{
  model::draw ();
}

// controls the object
void other::control ()
{
  // we don't need any AI here anymore because we can use the computer class
  move ();
}

// just for compatility reasons
void other::set_weaponcount (int c)
{

}

// just for compatility reasons
void other::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{

}

// adds an object to the pathlist
void other::add_object (int target,object* obj)
{

}

