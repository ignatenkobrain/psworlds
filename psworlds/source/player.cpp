// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "player.h"
#include "psworlds.h"
#include "sound.h"
#include "camera.h"
#include "mission.h"
#include "cockpit.h"
#include "objectlist.h"
#include "waypoint.h"
#include "fonts.h"

#define TARGET_DIST 2250

player::player (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
         float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw)
        :fighter (filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,norms,sw)
{
  // no cracks at construction time
  bcrack[0] = 0;
  bcrack[1] = 0;
  bcrack[2] = 0;
  targetmode = 0;

  mvspd=0;
  write_log ("Player has been created\n");
}

player::~player ()
{

}

int player::Typ () {
  return PLAYER;
}

void player::draw () {
  if (((Camera*)getcam ())->getCamstate ()!=CAM_COCKPIT)
  {
    model::draw ();
  }
  if (target!=NULL)
  {
    //draw boundingbox of target in red
    model* obj=(model*)target->obj;
    glDisable (GL_LIGHTING);
    glEnable (GL_COLOR_MATERIAL);
    glColor4f (1.0,0.0,0.0,1.0);
    glDisable (GL_FOG);
    glBegin (GL_LINES);
      glVertex3f (obj->bbc (0,0),obj->bbc (0,1),obj->bbc (0,2));
      glVertex3f (obj->bbc (1,0),obj->bbc (1,1),obj->bbc (1,2));
      glVertex3f (obj->bbc (0,0),obj->bbc (0,1),obj->bbc (0,2));
      glVertex3f (obj->bbc (2,0),obj->bbc (2,1),obj->bbc (2,2));
      glVertex3f (obj->bbc (0,0),obj->bbc (0,1),obj->bbc (0,2));
      glVertex3f (obj->bbc (4,0),obj->bbc (4,1),obj->bbc (4,2));
      glVertex3f (obj->bbc (1,0),obj->bbc (1,1),obj->bbc (1,2));
      glVertex3f (obj->bbc (3,0),obj->bbc (3,1),obj->bbc (3,2));
      glVertex3f (obj->bbc (1,0),obj->bbc (1,1),obj->bbc (1,2));
      glVertex3f (obj->bbc (5,0),obj->bbc (5,1),obj->bbc (5,2));
      glVertex3f (obj->bbc (2,0),obj->bbc (2,1),obj->bbc (2,2));
      glVertex3f (obj->bbc (3,0),obj->bbc (3,1),obj->bbc (3,2));
      glVertex3f (obj->bbc (2,0),obj->bbc (2,1),obj->bbc (2,2));
      glVertex3f (obj->bbc (6,0),obj->bbc (6,1),obj->bbc (6,2));
      glVertex3f (obj->bbc (3,0),obj->bbc (3,1),obj->bbc (3,2));
      glVertex3f (obj->bbc (7,0),obj->bbc (7,1),obj->bbc (7,2));
      glVertex3f (obj->bbc (4,0),obj->bbc (4,1),obj->bbc (4,2));
      glVertex3f (obj->bbc (5,0),obj->bbc (5,1),obj->bbc (5,2));
      glVertex3f (obj->bbc (4,0),obj->bbc (4,1),obj->bbc (4,2));
      glVertex3f (obj->bbc (6,0),obj->bbc (6,1),obj->bbc (6,2));
      glVertex3f (obj->bbc (5,0),obj->bbc (5,1),obj->bbc (5,2));
      glVertex3f (obj->bbc (7,0),obj->bbc (7,1),obj->bbc (7,2));
      glVertex3f (obj->bbc (6,0),obj->bbc (6,1),obj->bbc (6,2));
      glVertex3f (obj->bbc (7,0),obj->bbc (7,1),obj->bbc (7,2));
    glEnd ();
    glEnable (GL_FOG);
    glDisable (GL_COLOR_MATERIAL);
    glEnable (GL_LIGHTING);
  }
  if (!destroyable)
  {
    print (300,80,"GOD MODE",1,1.0,1.0,1.0,1.0);
  }
}

void player::control ()
{
  // read keyboard commands to control the ship
  int gs;
  char *keys;
  headlight* lite = NULL;
  
  // check if we can fire again
  float secs=gettime ();
  lastshot+=secs;

  check_target ();

  // first of all, get the gamestate
  gs = getGamestate ();

  // now respond to player input
  if (gs == GS_INGAME||gs == GS_MISSIONEND)
  {
    // grab first headlite, if possible
    if (lites[0]!=NULL)
    {
      lite = (headlight*)lites[0];
    }

    keys = getkeys ();

    // cheats
    if (keys[SDLK_g]&&keys[SDLK_o]&&keys[SDLK_d])
    {
      // GOD MODE - YEAH
      destroyable = !destroyable;
      keys[SDLK_g]=0;
      keys[SDLK_o]=0;
      keys[SDLK_d]=0;
      setCheated (1);
    }
    if (keys[SDLK_f]&&keys[SDLK_i]&&keys[SDLK_n])
    {
      // Win a mission the fast way
      ((mission*)getmission ())->set_success ();
      keys[SDLK_f]=0;
      keys[SDLK_i]=0;
      keys[SDLK_n]=0;
      setCheated (1);
    }
    if (keys[SDLK_n]&&keys[SDLK_r]&&keys[SDLK_g])
    {
      // Full Energy
      energy = fullnrg;
      //remove cracks
      bcrack[0] = 0;
      bcrack[1] = 0;
      bcrack[2] = 0;
      keys[SDLK_n]=0;
      keys[SDLK_r]=0;
      keys[SDLK_g]=0;
      setCheated (1);
    }
    if (keys[SDLK_t]&&keys[SDLK_r]&&keys[SDLK_g])
    {
      // Enable targeting without range limit
      targetmode=!targetmode;
      keys[SDLK_t]=0;
      keys[SDLK_r]=0;
      keys[SDLK_g]=0;
      setCheated (1);
    }

    // normal keys
    // switch headlite on/off
    if (keys[SDLK_KP1] && lite!=NULL)
    {
      lite->toggle ();
      keys[SDLK_KP1]=0;
    }

    if (keys[SDLK_LEFT])
    {
      if ((keys[SDLK_KP0]) && (lite!=NULL))
        lite->left ();
      else
      left ();
    }
    if (keys[SDLK_RIGHT])
    { 
      if ((keys[SDLK_KP0]) && (lite!=NULL))
        lite->right ();
      else
      right ();
    }
    if (keys[SDLK_UP])
    {
      if ((keys[SDLK_KP0]) && (lite!=NULL))
        lite->up ();
      else
      up ();
    }
    if (keys[SDLK_DOWN])
    {
      if ((keys[SDLK_KP0]) && (lite!=NULL))
        lite->down ();
      else
      down ();
    }
    if (keys[SDLK_s])
    {
      rollleft ();
    }
    if (keys[SDLK_d])
    {
      rollright ();
    }
    if (keys[SDLK_a])
    {
      inc_speed ();
    }
    if (keys[SDLK_z])
    {
      dec_speed ();
    }
    if (keys[SDLK_x])
    {
      findnext_target ();
      (((mission*)getmission ())->getCockpit ())->setDisplay (0,CP_TARGET);
      keys[SDLK_x]=0;
    }
    if (keys[SDLK_c])
    {
      clear_target ();
      keys[SDLK_c]=0;
    }
    if (keys[SDLK_SPACE])
    {
      shoot ();
    }
    if (keys[SDLK_TAB])
    {
      // also play the weapon switch sample here
      Camera* cam = (Camera*) getcam ();
      playsam (15,16000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),210);
    
      switch_weapon ();
      keys[SDLK_TAB]=0;
    }
    if (keys[SDLK_BACKSPACE])
    {  
      // browse waypoints
      keys[SDLK_BACKSPACE]=0;
      playsam (8,14500,135,205);
      if (waypt!=NULL) waypt = waypt->next;
      if (waypt==NULL)
        waypt = pathlist->get_first ();
    }
    if (keys[SDLK_KP0])
    {
      if (keys[SDLK_KP4])
      {
        if (lite!=NULL)
        {
          lite->change_r (-secs);
        }
      }
      if (keys[SDLK_KP7])
      {
        if (lite!=NULL)
        {
          lite->change_r (secs);
        }
      }
      if (keys[SDLK_KP5])
      {
        if (lite!=NULL)
        {
          lite->change_g (-secs);
        }
      }
      if (keys[SDLK_KP8])
      {
        if (lite!=NULL)
        {
          lite->change_g (secs);
        }
      }
      if (keys[SDLK_KP6])
      {
        if (lite!=NULL)
        {
          lite->change_b (-secs);
        }
      }
      if (keys[SDLK_KP9])
      {
        if (lite!=NULL)
        {
          lite->change_b (secs);
        }

      }
    }
  }
  move ();


  update_lifetime ();

  int volume, freq;
  Camera* cam;

  // play the ship´s engine, volume depends on distance ship-camera
  cam = (Camera*) getcam ();
  volume = distantvolume (cam->getx (), cam->gety (), cam->getz (), x,y,z);
  freq = 12000+fabs (mvspd)*35;

  playengine (0,freq/2, volume);

  // find next waypoint or keep active one
  if (waypt==NULL) waypt = pathlist->get_first ();
  if (waypt!=NULL)
  {
    if (distance (waypt->obj,this)<WAYPT_PL_DIST && !((waypoint*)waypt->obj)->get_reached ())
    {
      ((waypoint*)waypt->obj)->set_reached ();
      waypt = waypt->next;
      playsam (24,22000,255,255);

      if (waypt==NULL)
        waypt = pathlist->get_first ();
    }
  }

  // reduce energy if pressure to high / low
  if (fabs (compression)/1800.0>0.4)
  {
    if (destroyable)
    {
      energy-=fullnrg/20*fabs (compression)/1800.0*secs;
      // signalise damage to cockpit
      ((mission*)getmission ())->getCockpit ()->hit ();
    }
  }

  // create cracks in windshield if we´ve got low energy
  float restnrg = (float)energy/fullnrg;
  if (restnrg<0.1)
  {
    if (bcrack[2]==0)
    {
      bcrack[2]=1;
      ((Cockpit*)((mission*)getmission ())->getCockpit ())->genCrack (2);
    }
  }
  else if (restnrg<0.2)
  {
    if (bcrack[1]==0)
    {
      bcrack[1]=1;
      ((Cockpit*)((mission*)getmission ())->getCockpit ())->genCrack (1);
    }
  }
  else if (restnrg<0.3)
  {
    if (bcrack[0]==0)
    {
      bcrack[0]=1;
      ((Cockpit*)((mission*)getmission ())->getCockpit ())->genCrack (0);
    }
  }
}


void player::explode ()
{
  stoplooping ();
  fighter::explode ();
}


void player::clear_target ()
{
  target=NULL;
}

void player::findnext_target ()
{
  object* obj=NULL;
  object* obj1=NULL;
  listobj* tmplobj=NULL;
  listobj* targetlobj=NULL;
  listobj* firstlobj=NULL;
  objectlist* objlist=((mission*)getmission ())->get_objlist ();
  float distmin=0,distmax=TARGET_DIST,distmax2=TARGET_DIST;
  if (targetmode)
  {
    distmax=0;
    distmax2=0;
  }
  float dist;
  if (target!=NULL)
  {
    obj=target->obj;
    if (objlist->in_list (obj)!=NULL)
    {
      distmin=distance (this,obj);
    }
  }
  tmplobj = (listobj*)(objlist->get_first ())->next;
  while (tmplobj!=NULL)
  {
    if (tmplobj->obj->Typ ()==HOMING||tmplobj->obj->Typ ()==DIRECT||tmplobj->obj->Typ ()==WAYPOINT)
    {
      tmplobj=NULL;
    }
    else
    {
      // only target objects which are destroyable
      if (((model*)(tmplobj->obj))->get_destroyable ()&&(tmplobj->obj)->getmvstate ()!=MV_DESTROY)
      {
        dist = distance (this,tmplobj->obj);
        if (dist>distmin)
        {
          if (distmax==0||dist<distmax)
          {
            targetlobj=tmplobj;
            distmax=dist;
          }
        }
        if (distmax2==0||dist<distmax2)
        {
          firstlobj=tmplobj;
          distmax2=dist;
        }
      }
      tmplobj=(listobj*)tmplobj->next;
    }
  }
  target=targetlobj;
  if (target==NULL)
  {
    target=firstlobj;
  }
}

void player::check_target ()
{
  if (target!=NULL)
  {
    object* obj=target->obj;
    objectlist* objlist=((mission*)getmission ())->get_objlist ();
    if (objlist->in_list (obj)==NULL)
    {
      clear_target ();
      findnext_target ();
    }
    else if (obj->getmvstate ()==MV_DESTROY)
    {
      clear_target ();
      findnext_target ();      
    }
    else if (distance (this,obj)>TARGET_DIST&&!targetmode)
    {
      clear_target ();
      findnext_target ();      
    }
  }
}


weapon_t* player::getweapons ()
{
  // provide access to the weapon array
  return weapons;
}


int player::getweapcount ()
{
  // return # of weapons
  return weaponcount;
}


int player::getactiveweapon ()
{
  return active_weapon;
}

void* player::getwaypoint ()
{
  if (waypt!=NULL) return waypt->obj;
  return NULL;
}

