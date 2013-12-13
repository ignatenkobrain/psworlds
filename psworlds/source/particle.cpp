// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "particle.h"
#include "psworlds.h"

particle::particle (float X, float Y, float Z, float R, float G, float B, float A):
      object (X,Y,Z,0,0,0)
{
  r=R;
  g=G;
  b=B;
  a=A;
  time_to_live=rand ()/(float)RAND_MAX*PARTICLE_LIFETIME+PARTICLE_LIFETIME;
  movement_state=MV_DESTROY;
}

particle::~particle ()
{

}

int particle::Typ ()
{
  return PARTICLE;
}

// draws the particle
void particle::draw ()
{
  glDisable (GL_LIGHTING);
  glEnable (GL_COLOR_MATERIAL);
  glColor4f (r,g,b,a);
  glBegin (GL_POINTS);
    glVertex3f (x,y,z);
  glEnd ();
  glDisable (GL_COLOR_MATERIAL);
  glEnable (GL_LIGHTING);
}

// just for compatility reasons
void particle::move ()
{
  // nothing to do here, particles don't move
}

// decreases lifetime of particle
void particle::control ()
{
  update_lifetime ();
}

// just for compatility reasons
void particle::clear_collision_info ()
{
  // nothing to do here, particles can't collide
}

// just for compatility reasons
void particle::set_weaponcount (int c)
{

}

// just for compatility reasons
void particle::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{

}

// just for compatility reasons
void particle::add_object (int target,object* obj)
{

}

