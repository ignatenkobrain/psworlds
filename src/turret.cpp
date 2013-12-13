// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "turret.h"
#include "direct.h"
#include "homing.h"
#include "torpedo.h"
#include "mission.h"
#include "sound.h"


turret::turret (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, int Mass, int Energy,GLfloat *norms,int *sw):building (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Mass,Energy,norms,sw)
{
  targetlist = new objectlist ();
  target = NULL;
  lastshot = 0.0;
  weapons = NULL;
  weaponcount = 0;
  active_weapon = 0;
}

turret::~turret ()
{
  if (weaponcount)
  {
    free (weapons);
  }
  targetlist->clear_all_entrys ();
  delete targetlist;
}

int turret::Typ ()
{
  return TURRET;
}

// draws the object
void turret::draw ()
{
  model::draw ();
}

// delete all entrys in targetlist from destroyed objects
void turret::cleanup_targetlist ()
{
  objectlist* objlist = ((mission*)getmission ())->get_objlist ();
  listobj* tmp = targetlist->get_first ();
  listobj* tmp1 = NULL;
  while (tmp!=NULL)
  {
    if (objlist->in_list (tmp->obj)==NULL)
    {
      tmp1 =  tmp->next;
      targetlist->delete_entry (tmp);
      tmp = tmp1;
    }
    else if (tmp->obj->getmvstate ()==MV_DESTROY)
    {
      tmp1 =  tmp->next;
      targetlist->delete_entry (tmp);
      tmp = tmp1;
    }
    else
    {
      tmp = tmp->next;
    }
  }
  if (target!=NULL)
  {
    if (targetlist->in_list (target->obj)==NULL)
      target = NULL;
  }
}

// controls the object
void turret::control ()
{
  building::control ();

  cleanup_targetlist ();
  // first step, establish target
  
  //  nearobject = search nearest object in targetlist
  listobj* nearest = targetlist->nearest (this);

  // if target still exists
  if (target!=NULL)
  {
    if (targetlist->in_list (target->obj)!=NULL)
    {
      // if target is far away
      if (distance (this,target->obj)>1000)
      {
        if (nearest!=NULL)
        {
          // and nearest target is near
          if (distance (this,nearest->obj)<1000)
          {
            // choose new target
            target = nearest;
          }
          else
          {
            // no target near
            target = NULL;
          }
        }
        else
        {
          // no target near
          target = NULL;
        }
      }
    }
  }
  else
  {
    if (nearest!=NULL)
    {
      // if nearest target is not to far away
      if (distance (nearest->obj,this)<1000)
      {
        // choose target
        target = nearest;
      }
      else
      {
        // no target near
        target = NULL;
      }
    }
    else
    {
      // no target near
      target = NULL;
    }
  }

  // if there is a target
  if (target!=NULL)
  {
    // second step, follow the object
    float destx=target->obj->getx ();
    float desty=target->obj->gety ();
    float destz=target->obj->getz ();

    destx-=x;
    desty-=y;
    destz-=z;

    float rx=destx*invmat[0]+desty*invmat[4]+destz*invmat[8];
    float ry=destx*invmat[1]+desty*invmat[5]+destz*invmat[9];
    float rz=destx*invmat[2]+desty*invmat[6]+destz*invmat[10];
    float d=fastdist3 (rx,ry,rz);
    rx/=d;
    ry/=d;
    rz/=d;
    if (rx<-0.005)
    {
      left ();
    }
    else if (rx>0.005)
    {
      right ();
    }
    if (ry<-0.005)
    {
      up ();
    }
    else if (ry>0.005)
    {
      down ();
    }
    dx=-mat[8];
    dy=-mat[9];
    dz=-mat[10];

    // third step, if object is near choose weapon and fire
    if (collision_soon_object!=NULL)
    {
      if (collision_soon_object==target->obj||(((model*)collision_soon_object)->get_enemystatus ()!=enemystatus))
      {
        active_weapon=0;
        shoot ();
      }
    }
  }

  // update time since last shot
  float secs=gettime ();
  lastshot+=secs;
}

// sets the weapon count and initializes the weapon array
void turret::set_weaponcount (int c)
{
  weaponcount = c;
  active_weapon = 0;
  if (c>0)
  {
    weapons = (weapon_t*)malloc (sizeof (weapon_t)*c);
  }
}

// adds a weapon to the weapon array
void turret::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{
  if (active_weapon<weaponcount)
  {
    weapons[active_weapon].x=sx;
    weapons[active_weapon].y=sy;
    weapons[active_weapon].z=sz;
    weapons[active_weapon].x2=sx2;
    weapons[active_weapon].y2=sy2;
    weapons[active_weapon].z2=sz2;
    weapons[active_weapon].time_to_live=time;
    weapons[active_weapon].power=power;
    weapons[active_weapon].amount=amount;
    weapons[active_weapon].filename=(char*)malloc (strlen (filename)+1);
    strcpy (weapons[active_weapon].filename,filename);
    weapons[active_weapon].name=(char*)malloc (strlen (name)+1);
        strcpy (weapons[active_weapon].name,name);
    weapons[active_weapon].mvspd=mvspd;
    weapons[active_weapon].rotspd=rotspd;
    weapons[active_weapon].type=type;
    weapons[active_weapon].doubleshot=dbl;
    weapons[active_weapon].rate=rate;
    active_weapon++;
  }
  if (active_weapon==weaponcount)
  {
    active_weapon=0;
  }
}

// adds an object to the targetlist
void turret::add_object (int target,object* obj)
{
  if (target)
  {
    targetlist->add_object (obj);
  }
}

// fires missiles for all active wepaons
void turret::shoot ()
{
  GLfloat no[] = { 1.0, 1.0, 1.0};
  int sw[] = { 0,1,2};
  model* shot=NULL;
  if (active_weapon<weaponcount)
  {
    if (lastshot>1/weapons[active_weapon].rate)
    {
      if (weapons[active_weapon].amount>0||weapons[active_weapon].amount==-1)  // weapon available
      {
        float x,y,z;
        x = weapons[active_weapon].x;
        y = weapons[active_weapon].y;
        z = weapons[active_weapon].z;
        float sx=mat[0]*x+mat[4]*y+mat[8]*z+getx ();
        float sy=mat[1]*x+mat[5]*y+mat[9]*z+gety ();
        float sz=mat[2]*x+mat[6]*y+mat[10]*z+getz ();
        if (weapons[active_weapon].type==DIRECT)
        {
          shot=new direct (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
        }
        else if (weapons[active_weapon].type==TORPEDO)
        {
          shot = new torpedo (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
          ((torpedo*)shot)->setspeed (mvspd);
        }
        else
        {
          shot=new homing (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
          if (target!=NULL)
          {
            ((homing*)shot)->set_target (target->obj);
          }
          else
          {
            ((homing*)shot)->set_target (NULL);
          }
          ((homing*)shot)->setspeed (mvspd);
        }
        shot->set_lifetime (weapons[active_weapon].time_to_live);
        shot->setmat (mat);
        shot->setinvmat (invmat);
        shot->add_collision_object (this);
        ((mission*)getmission ())->add_object (shot);
        if (weapons[active_weapon].doubleshot)
        {
          x = weapons[active_weapon].x2;
          y = weapons[active_weapon].y2;
          z = weapons[active_weapon].z2;
          float sx=mat[0]*x+mat[4]*y+mat[8]*z+getx ();
          float sy=mat[1]*x+mat[5]*y+mat[9]*z+gety ();
          float sz=mat[2]*x+mat[6]*y+mat[10]*z+getz ();
          if (weapons[active_weapon].type==DIRECT)
          {
               shot=new direct (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
          }
          else if (weapons[active_weapon].type==TORPEDO)
          {
            shot = new torpedo (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
            ((torpedo*)shot)->setspeed (mvspd);
          }
          else
          {
            shot=new homing (weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
            if (target!=NULL)
            {
              ((homing*)shot)->set_target (target->obj);
            }
            else
            {
              ((homing*)shot)->set_target (NULL);
            }
            ((homing*)shot)->setspeed (mvspd);
          }
          shot->set_lifetime (weapons[active_weapon].time_to_live);
          shot->setmat (mat);
          shot->setinvmat (invmat);
          shot->add_collision_object (this);
          ((mission*)getmission ())->add_object (shot);
        }
        // reduce weaponcount for
        if (weapons[active_weapon].amount>0)
        {
          weapons[active_weapon].amount--;
        }
      
        // play some sound
        Camera* cam = (Camera*) getcam ();
        playsam (3,15000,distantvolume (cam->getx (), cam->gety (), cam->getz (), sx, sy, sz)/3,100);
        lastshot=0;
      }
    }
  }
}


// (de)activates weapon i
void turret::switch_weapon ()
{
  if (active_weapon<weaponcount-1)
  {
    active_weapon++;
  }
  else
  {
    active_weapon=0;
  }
}






