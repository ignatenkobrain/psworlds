// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "torpedo.h"
#include "sound.h"
#include "mission.h"
#include "bitmap.h"


torpedo::torpedo (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw):
        missile (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,norms,sw)
{

  // generate the trail object
  mytrail = new trail (x,y,z,rotX,rotY,rotZ,this,TRAIL_MISSILE);
  ((mission*)getmission ())->add_trail (mytrail);
}

torpedo::~torpedo ()
{

}

int torpedo::Typ () {
  return TORPEDO;
}

void torpedo::draw () {
  model::draw ();
}

void torpedo::control ()
{
  // accelerate until maxspeed
  if (mvspd<maxspd) inc_speed ();
  
  move ();
  update_lifetime ();
}


void torpedo::setspeed (int value)
{
  mvspd=value;
}


void torpedo::ground_collision ()
{
  // checks for collision with the seafloor
  // we usually have the model coordinate origin at the center of gravity, so minimum
  // altitude should be 5 or so
  Camera* cam;


  if (alt<2)
  {
    cam = (Camera*) getcam ();
    playsam (5,12000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),201);
    
    // kill the damn thing
    destroy=1;
    mytrail->divorce ();

    bitmap* bmp = new bitmap (prevx,prevy,prevz,0,0,0,BMP_GUNEXPLO,0.0);
    ((mission*)getmission ())->add_bmp (bmp);
    ((mission*) getmission ())->get_bmplist ()->setsort (1);
  }
  prevx = x;
  prevy = y;
  prevz = z;
}

void torpedo::move_collide ()
{
  Camera* cam = (Camera*) getcam ();
  // destroy the shot if it hits an object
  destroy = 1;

  // signal the trail object that we´re gone
  mytrail->divorce ();
  // and spawn gunhit-bitmap
  bitmap* bmp = new bitmap (prevx, prevy, prevz, 0,0,0, BMP_GUNEXPLO, 0.0);
  ((mission*) getmission ())->add_bmp (bmp);
  ((mission*) getmission ())->get_bmplist ()->setsort (1);

  // play sounds
  playsam (5,12000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),198);
  playsam (20,8400,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),201);  
}

