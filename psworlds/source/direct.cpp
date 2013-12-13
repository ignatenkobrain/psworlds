// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "direct.h"
#include "bitmap.h"
#include "mission.h"
#include "sound.h"
#include "trail.h"

direct::direct (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy,GLfloat *norms,int *sw):
          missile (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,norms,sw)
{
  mvspd=maxspd;

  // generate the trail object
  mytrail = new trail (x,y,z,rotX,rotY,rotZ,this,TRAIL_FOLLOW);
  ((mission*)getmission ())->add_trail (mytrail);

}

direct::~direct ()
{

}

int direct::Typ () 
{
  return DIRECT;
}

// draws the object
void direct::draw () 
{
  model::draw ();
}

void direct::control ()
{
  // nothing else to do here because we just go in one direction
  move ();
  update_lifetime ();
}

void direct::ground_collision ()
{
  // checks for collision with the seafloor
  // we usually have the model coordinate origin at the center of gravity, so minimum
  // altitude should be 5 or so
  Camera* cam;
  bitmap* bmp = NULL;  

  if (alt<2)
  {
    cam = (Camera*) getcam ();
    // play impact sample
    playsam (7,12000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z)*0.8,99);
    
    // kill the damn thing
    destroy=1;

    // signal the trail object that we´re gone
    mytrail->divorce ();

    // spawn a lovely dust cloud
    bmp = new bitmap (prevx,prevy,prevz,0,0,0,BMP_SMALLDUST,0.0);
    ((mission*)getmission ())->add_bmp (bmp);
    ((mission*) getmission ())->get_bmplist ()->setsort (1);
  }
  prevx = x;
  prevy = y;
  prevz = z;
}


void direct::move_collide ()
{
  // destroy the shot if it hits an object
  destroy = 1;

  // signal the trail object that we´re gone
  mytrail->divorce ();
  // and spawn gunhit-bitmap
  bitmap* bmp = new bitmap (prevx, prevy, prevz, 0,0,0, BMP_GUNHIT, 0.0);
  ((mission*) getmission ())->add_bmp (bmp);
  ((mission*) getmission ())->get_bmplist ()->setsort (1);
  // play impact sample
  Camera* cam = (Camera*) getcam ();
  playsam (20,8400,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),198);
}

