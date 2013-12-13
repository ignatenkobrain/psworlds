// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "homing.h"
#include "sound.h"
#include "mission.h"
#include "bitmap.h"


homing::homing (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw):
        missile (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,norms,sw)
{

  // generate the trail object
  mytrail = new trail (x,y,z,rotX,rotY,rotZ,this,TRAIL_HOMING);
  ((mission*)getmission ())->add_trail (mytrail);


}

homing::~homing ()
{

}

int homing::Typ () {
  return HOMING;
}

void homing::draw () {
  model::draw ();
}

void homing::control ()
{
  if (target!=NULL)
  {
    // check if target still exists
    objectlist* objlist=((mission*)getmission ())->get_objlist ();
    if (objlist->in_list (target)==NULL)
    {
      target=NULL;
    }
    else if (target->getmvstate ()==MV_DESTROY)
    {
      target=NULL;
    }
  }

  // change directions to follow the object
  if (target!=NULL)
  {
    float destx=target->getx ();
    float desty=target->gety ();
    float destz=target->getz ();

    destx-=x;
    desty-=y;
    destz-=z;
    float rx=destx*invmat[0]+desty*invmat[4]+destz*invmat[8];
    float ry=destx*invmat[1]+desty*invmat[5]+destz*invmat[9];
    float rz=destx*invmat[2]+desty*invmat[6]+destz*invmat[10];
    float d=fastdist3(rx,ry,rz);
    rx/=d;
    ry/=d;
    rz/=d;
    if (rx<-0.1)
    {
      left ();
    }
    else if (rx>0.1)
    {
      right ();
    }
    if (ry<-0.1)
    {
      up ();
    }
    else if (ry>0.1)
    {
      down ();
    }
    if (rz<-0.1)
    {
      if (mvspd<maxspd) inc_speed ();
    }
    else if (rz>0.1)
    {  
      if (mvspd>maxspd/4) dec_speed ();
    }
  }
  else
  {
    // accelerate until maxspeed
    if (mvspd<maxspd) inc_speed ();
  }
  move ();
  update_lifetime ();
}

void homing::set_target (object* obj)
{
  target=obj;
}

void homing::setspeed (int value)
{
  mvspd=value;
}


void homing::ground_collision ()
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

void homing::move_collide ()
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

