// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "mission.h"
#include "sound.h"
#include "fonts.h"
#include "headlight.h"
#include "alienlight.h"
#include "matrices.h"
#include "cockpit.h"


#define PPS 200    // particles per second to be added

// constructs a mission out of a missionfile
mission::mission (char* filename)
{
  xpath (filename);
  
  write_log ("Loading mission\n");
  cam = (Camera*)getcam ();

  f = fopen (filename, "r");

  objlist=new objectlist ();
  // create an extra list for the bitmaps. this is necessary because the bitmaps
  // will have to be resorted often
  bmplist = new objectlist ();

  // create the list for the particles
  partlist = new objectlist ();

  // create list for headlights
  litelist = new objectlist ();

  // finally, create a list for the weapon trails
  traillist = new objectlist ();

  alienlist = new objectlist ();

  demo = 0;
  num_goals = 0;

  read_file ();
  
  fclose (f);

  write_log ("Preparing Camera\n");

  // now set the player and bind the camera to him
  if (!demo)
  {
    plr = (player*)(objlist->get_first (PLAYER)->obj);
    cam->setActor (plr);
  }
  else
  {
    cam->setActor (objlist->get_first ()->obj);
  }
  
  if (!demo)
  {
    cockpit = new Cockpit (1, plr, this);  // generate Cockpit for fighter 1
  }
  else
  {
    cockpit = NULL;
  }

  mis_success = 0;
  mis_failure = 0;
  player_dead = 0;

}

// delete all allocated resources
mission::~mission ()
{
  if (sea!=NULL) delete sea;
  objlist->clear_all_objects ();
  bmplist->clear_all_objects ();
  partlist->clear_all_objects ();
  traillist->clear_all_objects ();
  litelist->clear_all_objects ();
  alienlist->clear_all_objects ();
  if (alienlist!=NULL) delete alienlist;
  if (objlist!=NULL) delete objlist;
  if (bmplist!=NULL) delete bmplist;
  if (partlist!=NULL) delete partlist;
  if (traillist!=NULL) delete traillist;
  if (litelist!=NULL) delete litelist;
  if (cockpit!=NULL) delete cockpit;
  for (int i=0;i<num_goals;i++)
  {
    if (goals[i].savelist!=NULL) delete goals[i].savelist;
    if (goals[i].reachlist!=NULL) delete goals[i].reachlist;
    if (goals[i].destroylist!=NULL) delete goals[i].destroylist;
  }
  //if (goals!=NULL) delete goals;
}

// reads the mission file and creates the object lists
void mission::read_file ()
{
  int num_al=1;
  char data[255];
  while (!feof (f)) 
  {
    fscanf (f, "%s", &data);
    if (!strcmp (data,"#pausemenu"))
    {
      fscanf (f,"%d\n",&pausemenu);
      if (pausemenu<=0) pausemenu=0;
      else pausemenu=1;
    }
    else if (!strcmp (data, "#map"))
    {
      read_map ();
    }
    else if (!strcmp (data,"#model"))
    {
      read_model ();
    }
    else if (!strcmp (data,"#targetlist"))
    {
      read_targetlist ();
    }
    else if (!strcmp (data,"#pathlist"))
    {
      read_pathlist ();
    }
    else if (!strcmp (data,"#num_goals"))
    {
      fscanf (f," %d",&num_goals);
      active_goal=0;
      goals = (goal_t*) malloc (sizeof (goal_t)*num_goals);
    }
    else if (!strcmp (data,"#goal"))
    {
      read_goal ();
    }
    else if (!strcmp (data,"#alienlight"))
    {
      read_alienlight (num_al);
      num_al++;
    }
    else if (!strcmp (data,"#demo"))
    {
      demo = 1;
    }
    else
    {
      fgets (data, sizeof (data), f);    // skip this line
    }
  }
}

// reads map information
void mission::read_map ()
{
  // load the map
  char file[255];
  _FBUF
  float scale, sealev, mindep, maxdep, waveheight, wavespeed;

  fscanf (f,"%s %f %f %f %f %f %f\n",file,&scale, &sealev, &maxdep, &mindep, &waveheight, &wavespeed);
  sprintf (buf, "missions/%s",file);
  sea = new world (buf, scale, sealev, maxdep, mindep, waveheight, wavespeed);
}

// reads alienlight information
void mission::read_alienlight (int gll)
{
  float ambr=0,ambg=0,ambb=0,amba=0;
  float difr=0,difg=0,difb=0,difa=0;
  float specr=0,specg=0,specb=0,speca=0;
  float x=0,y=0,z=0;
  float att=0;
  int ID;
  int bind=0;
  object* obj=NULL;
  alienlight* al=NULL;
  // indicates end of model description
  int end=0;
  // stringbuffer for reading file
  char data[255];

  while (!end)
  {
    fscanf (f, "%s", &data);
    if (!strcmp (data, "#object"))
    {
      fscanf (f, " %d", &ID);
      obj = objlist->in_list (ID)->obj;
    }
    else if (!strcmp (data, "#position"))
    {
      fscanf (f, " %f %f %f", &x,&y,&z);
          }
    else if (!strcmp (data, "#specular"))
    {
      fscanf (f, " %f %f %f %f", &specr, &specg, &specb, &speca);
    }
    else if (!strcmp (data,"#ambient"))
    {
      fscanf (f, " %f %f %f %f", &ambr, &ambg, &ambb, &amba);
    }
    else if (!strcmp (data,"#diffuse"))
    {
      fscanf (f, " %f %f %f %f", &difr, &difg, &difb, &difa);
    }
    else if (!strcmp (data,"#attenuation"))
    {
      write_log ("attentuation_factor\n");
      fscanf (f, " %f", &att);
    }
    else if (!strcmp (data,"#bind_to_headlight"))
    {
      bind=1;
    }
    else if (!strcmp (data,"#end_alienlight"))
    {
      end=1;
    }
    else
    {
      fgets (data, sizeof (data), f);    // skip this line
    }
  }
  al = new alienlight (gll,x,y,z,ambr,ambg,ambb,amba,difr,difg,difb,difa,specr,specg,specb,speca,att,obj);
  alienlist->add_object (al);
  if (bind) ((model*)obj)->bind_alienlight (al);
}

// reads model information
void mission::read_model ()
{
  // pointer to model
  object* obj=NULL;

  // values for model
  int id=0;
  int typ=0;
  int mass=0;
  int energy=0;
  int enemystatus=NEUTRAL;
  int wpend=0;
  char name[255]="\0";
  float x=0;
  float y=0;
  float z=0;
  float rotx=0;
  float roty=0;
  float rotz=0;
  float rotspd=0;
  float maxspd=0;

  // boolean variables to check if value is specified
  int bmass=0;
  int benergy=0;
  int bname=0;
  int bposition=0;
  int borientation=0;
  int bspeed=0;

  // indicates end of model description
  int end=0;
  
  // stringbuffer for reading file
  char data[255];

  fscanf (f," %d %d",&id,&typ);
  while (!end)
  {
    fscanf (f, "%s", &data);
    if (!strcmp (data, "#file"))
    {
      fscanf (f," %s",data);
      obj = read_ship (data,typ);
    }
    else if (!strcmp (data, "#name"))
    {
      fscanf (f," %s",name);
      remove_underscore (name);
      bname=1;
    }
    else if (!strcmp (data,"#position"))
    {
      fscanf (f," %f %f %f",&x,&y,&z);
      bposition=1;
    }
    else if (!strcmp (data,"#orientation"))
    {
      fscanf (f," %f %f %f",&rotx,&roty,&rotz);
      borientation=1;
    }
    else if (!strcmp (data,"#speed"))
    {
      fscanf (f," %f %f",&maxspd,&rotspd);
      bspeed=1;
    }
    else if (!strcmp (data,"#mass"))
    {
      fscanf (f," %d",&mass);
      bmass=1;
    }
    else if (!strcmp (data,"#energy"))
    {
      fscanf (f," %d",&energy);
      benergy=1;
    }
    else if (!strcmp (data,"#enemystatus"))
    {
      fscanf (f," %d",&enemystatus);
    }
    else if (!strcmp (data,"#end_point"))
    {
      wpend=1;
    }
    else if (!strcmp (data,"#end_model"))
    {
      end=1;
    }
    else
    {
      fgets (data, sizeof (data), f);    // skip this line
    }
  }
  if (typ==WAYPOINT)
  {
    obj = new waypoint (x,y,z,wpend,0);
  }
  
  obj->setID (id);
  obj->set_enemystatus (enemystatus);
  if (bmass)
  {
    if (typ!=WAYPOINT)
    {
      ((model*)obj)->setMass (mass);
    }
  }
  if (benergy)
  {
    if (typ!=WAYPOINT)
    {
      ((model*)obj)->setEnergy (energy);
    }
  }
  if (bname)
  {
    obj->setName (name);
  }
  if (bposition)
  {
    obj->setx (x);
    obj->sety (y);
    obj->setz (z);
  }
  if (borientation)
  {
    obj->setrotx (rotx);
    obj->setroty (roty);
    obj->setrotz (rotz);
    if (typ!=WAYPOINT)
    {
      ((model*)obj)->create_matrices ();
    }
  }
  if (bspeed)
  {
    if (typ!=WAYPOINT)
    {
      ((model*)obj)->setMaxspd (maxspd);
      ((model*)obj)->setRotspd (rotspd);
    }
  }

  objlist->add_object (obj);
}

// reads goal information
void mission::read_goal ()
{
  char data[255];
  int end=0;
  int tmp=0;

  if (active_goal<num_goals)
  {
    goals[active_goal].destroylist = NULL;
    goals[active_goal].savelist = NULL;
    goals[active_goal].reachlist = NULL;
    goals[active_goal].failed = 0;
    goals[active_goal].success = 0;
    goals[active_goal].reachobj = NULL;
    while (!end)
    {
      fscanf (f, "%s", &data);
      if (!strcmp (data, "#destroy"))
      {
        goals[active_goal].destroylist = read_goal_list (NULL);
      }
      if (!strcmp (data, "#save"))
      {
        goals[active_goal].savelist = read_goal_list (NULL);
      }
      else if (!strcmp (data,"#reach"))
      {
        fscanf (f, " %d", &tmp);
        goals[active_goal].reachobj = (objlist->in_list (tmp))->obj;
        goals[active_goal].reachlist = read_goal_list (goals[active_goal].reachobj);
      }
      else if (!strcmp (data,"#shorttext"))
      {
        fgets (goals[active_goal].text, sizeof (goals[active_goal].text),f);
      }
      else if (!strcmp (data,"#end_goal"))
      {
        end=1;
      }
      else
      {
        fgets (data, sizeof (data), f);    // skip this line
      }
    }
    active_goal++;
  }
}

// reads destroylist,... information
objectlist* mission::read_goal_list (object* r)
{
  objectlist* l = new objectlist ();
  objectlist* p = NULL;
  if (r!=NULL)
    p = ((fighter*)r)->get_pathlist ();
  else
    p = objlist;
  int c,id;
  fscanf (f," %d",&c);
  for (int j=0;j<c;j++)
  {
    fscanf (f," %d",&id);
    l->add_object ((p->in_list (id))->obj);
  }
  return l;
}

// reads ship information
object* mission::read_ship (char* shp, int typ)
{
  GLfloat no[3];
  int sw[3] = { 0, 1,2};
  int mass=0,energy=0,end=0,wc=0,lc=0,i=0;
  char data[255];
  char file[255],name[255]="\0";
  FILE* tmp = f;
  strcpy (file,"missions/");
  strcat (file,shp);
  f = fopen (file,"r");
  float rotspd=0,maxspd=0;
  object* obj;
  
  no[0]=1; no[1]=1; no[2]=1;

  while (!feof (f)&&!end)
  {
    fscanf (f, "%s", &data);
    if (!strcmp (data, "#file"))
    {
      fscanf (f," %s",file);
    }
    if (!strcmp (data, "#name"))
    {
      fscanf (f," %s",name);
      remove_underscore (name);
    }
    else if (!strcmp (data,"#speed"))
    {
      fscanf (f," %f %f",&maxspd,&rotspd);
    }
    else if (!strcmp (data,"#mass"))
    {
      fscanf (f," %d",&mass);
    }
    else if (!strcmp (data,"#energy"))
    {
      fscanf (f," %d",&energy);
    }
    else if (!strcmp (data,"#normals"))
    {
      fscanf (f," %f %f %f", &no[0], &no[1], &no[2]);
    }
    else if (!strcmp (data,"#swap"))
    {
      fscanf (f," %d %d %d", &sw[0], &sw[1], &sw[2]);
    }
    else if (!strcmp (data,"#num_weapons"))
    {
      fscanf (f," %d",&wc);
    }
    else if (!strcmp (data, "#num_headlights"))
    {
      fscanf (f, "%d",&lc);
      end = 1;
    }
    else
    {
      fgets (data, sizeof (data), f);    // skip this line
    }
  }
  switch (typ) {
    case BUILDING: obj = new building (file,0,0,0,0,0,0,rotspd,mass,energy,no,sw); break;
    case COMPUTER: obj = new computer (file,0,0,0,0,0,0,rotspd,maxspd,mass,energy,no,sw); break;
    case PLAYER: obj = new player (file,0,0,0,0,0,0,rotspd,maxspd,mass,energy,no,sw); break;
    case TURRET: obj = new turret (file,0,0,0,0,0,0,rotspd,mass,energy,no,sw); break;
    case OTHER: obj = new other (file,0,0,0,0,0,0,rotspd,maxspd,mass,energy,no,sw); break;
  }
  obj->setName (name);

  // read wepaons
  if (wc>0)
  {
    i=0;
    obj->set_weaponcount (wc);
    while (i<wc)
    {
      fscanf (f, "%s", &data);
      if (!strcmp (data,"#weapon"))
      {
        read_weapon (obj);
      }
      else
      {
        fgets (data, sizeof (data), f);    // skip this line
      }
      i++;
    }
  }
  // read headlights
  if (lc>0)
  {
    i=0;
    while (i<lc)
    {
      fscanf (f, "%s", &data);
      if (!strcmp (data,"#light"))
      {
        read_light (obj);
      }
      else
      {
        fgets (data, sizeof (data), f);    // skip this line
      }
      i++;
    }
  }

  fclose (f);
  f=tmp;
  return obj;
}

// reads one weapon info for object obj
void mission::read_weapon (object* obj)
{
  char file[255],name[255];
  float x,y,z,x2,y2,z2,time,mvspd,rotspd,rate;
  int power,amount,type,dbl;
  fscanf (f," %d %s %d %s %f %f %f %f %f %f %f %f %f %d %d %f",&type,name,&dbl,file,&x,&y,&z,&x2,&y2,&z2,&mvspd,&rotspd,&time,&power,&amount,&rate);
  if (obj!=NULL)
  {
    remove_underscore (name);
    obj->add_weapon (file,name,x/10,y/10,z/10,x2/10,y2/10,z2/10,time,power,amount,type,dbl,mvspd,rotspd,rate);
  }
}


void mission::read_light (object* obj)
{
  // add a headlight to the object / to the headlight-list
  float x,y,z,xr,yr,zr,cutoff,length,intensity;
  int spec;
  headlight* hl;

  fscanf (f," %f %f %f %f %f %f %f %f %f %d",&x,&y,&z,&xr,&yr,&zr,&cutoff,&length,&intensity,&spec);
  hl = new headlight (x,y,z,xr,yr,zr,obj,cutoff,length,intensity,spec);
  litelist->add_object (hl);
  ((model*)obj)->add_light (hl);
}



// reads targetlist information
void mission::read_targetlist ()
{
  int id,c2;
  object* obj=NULL;
  // read object id and targetcount
  fscanf (f," %d %d",&id,&c2);
  listobj* lobj = objlist->in_list (id);
  if (lobj!=NULL)
  {
    obj=lobj->obj;
  }
  for (int j=0;j<c2;j++)
  {
    read_target (obj);
  }
}

// reads target information for object obj
void mission::read_target (object* obj)
{
  int tid;
  listobj* lobj=NULL;
  fscanf (f," %d",&tid);
  lobj = objlist->in_list (tid);
  if (lobj!=NULL)
  {
    obj->add_object (1,lobj->obj);
  }
}

// reads pathlist information
void mission::read_pathlist ()
{
  int id,c2;
  object* obj=NULL;
  // read object id and waypointcount
  fscanf (f," %d %d",&id,&c2);
  listobj* lobj = objlist->in_list (id);
  if (lobj!=NULL)
  {
    obj=lobj->obj;
  }
  for (int j=0;j<c2;j++)
  {
    read_waypoint (obj,c2-j);
  }
}

// reads path information for object obj
void mission::read_waypoint (object* obj, int num)
{
  int tid;
  listobj* lobj=NULL;
  fscanf (f," %d",&tid);
  lobj = objlist->in_list (tid);
  if (lobj!=NULL)
  {
    waypoint* nu = new waypoint (lobj->obj->getx (),lobj->obj->gety (),lobj->obj->getz (),((waypoint*)lobj->obj)->get_end (),num);
    nu->setID (tid);
    obj->add_object (0,nu);
  }
}


// draws the cockpit if necessary
int mission::drawcockpit ()
{
  if (!demo)
  {
    cockpit->recover ();
    if (cam->getCamstate () != CAM_COCKPIT) return 0;
  
    if (getGamestate ()==GS_INGAME||getGamestate () == GS_MISSIONEND) cockpit->displaykeys ();

    cockpit->draw ();
  }
  return 0;
}

// switches between different views of the camera
void mission::camcontrol ()
{
  char* keys=getkeys ();
  if (getGamestate () == GS_INGAME||getGamestate () == GS_MISSIONEND)
  {
    if (keys[SDLK_1])
    {
      cam->setCamstate (CAM_STILL);
    }
    if (keys[SDLK_2])
    {
      cam->setCamstate (CAM_FLYBY);
    }
    if (keys[SDLK_3])
    {
      cam->setCamstate (CAM_COCKPIT);
    }
    if (keys[SDLK_4])
    {
      cam->setCamstate (CAM_BEHIND);
    }
    if (keys[SDLK_5])
    {
      cam->setCamstate (CAM_FREEVIEW);
    }
    if (keys[SDLK_6])
    {
      cam->setCamstate (CAM_TOP);
    }
    if (keys[SDLK_7])
    {
      cam->setCamstate (CAM_BOT);
    }
    if (keys[SDLK_8])
    {
      keys[SDLK_8]=0;
      cam->toggle ();
    }
  }
}

// adds some particles for more fun
void mission::add_particles ()
{
  GLdouble mrx[16], mry[16], mrz[16], tmp1[16], tmp2[16];
  GLdouble trx[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, cam->getx (), cam->gety (), cam->getz (), 1.0};
  GLdouble pt[4], rpt[4];

  if (cam->getCamstate ()==CAM_COCKPIT||cam->getCamstate ()==CAM_BEHIND||cam->getCamstate ()==CAM_FLYBY)
  {
    GLdouble *tmp2 = cam->getInvmat ();
    matrmult2(trx,tmp2,tmp1);
  }
  else
  {
    // set up the rotation/translation matrices for moving the particles in front of the cam
    setup_mat_xrot (mrx,cam->getxrot ());
    setup_mat_yrot (mry,cam->getyrot ());
    setup_mat_zrot (mrz,cam->getzrot ());
  
    matrmult2(mrx, mry, tmp1);
    matrmult2(mrz, tmp1,tmp2);
    matrmult2(trx, tmp2, tmp1);
  }

  // now place the particles
  GLfloat ticks=gettime ();
  for (int i=0;i<ticks*PPS;i++)
  {
    // my approach is this: set particles in front of GLcam (down the negative z-axis)
    // then transform them manually so that they are in front of the cam

    float x=rand ()/(float)RAND_MAX*200-100;
    float y=rand ()/(float)RAND_MAX*200-100;
    float z=rand ()/(float)RAND_MAX*100-250;

    pt[0] = x; pt[1] = y; pt[2] = z; pt[3] = 1;
    mat_pt_multiply (tmp1, pt, rpt);  // transform the point in front of the cam

    partlist->add_object (new particle (rpt[0],rpt[1],rpt[2],0.2,0.4,0.9,0.8));
  }  
}

// moves all objects
void mission::move_objects () 
{
  objlist->control ();
  bmplist->control ();
  partlist->control ();
  traillist->control ();
  litelist->control ();
  alienlist->control ();
  add_particles ();
}

// draws all objects and the player object dependent of the Camerastate
void mission::draw_objects () 
{
  // if we need to sort the bmplist, do it now
  bmplist->resort ();

  // draw the models
  glAlphaFunc (GL_GREATER, 0.5);
  glEnable (GL_ALPHA_TEST);
  if (cam->getCamstate ()!=CAM_COCKPIT)
  {
    objlist->draw (cam->getx (),cam->gety (),cam->getz ());
  }
  else
  {
    objlist->draw (cam->getx (),cam->gety (),cam->getz ());
  }
  glDisable (GL_ALPHA_TEST);
  
  glDisable (GL_LIGHTING);
  glDisable (GL_COLOR_MATERIAL);
  glDisable (GL_CULL_FACE);
  glDepthMask (GL_FALSE); // make z-buffer read-only

  // draw headlights
  glBlendFunc (GL_SRC_ALPHA,GL_ONE);
  litelist->draw (cam->getx (),cam->gety (), cam->getz ());
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // draw the trails
  traillist->draw (cam->getx (),cam->gety (),cam->getz ());
  
  // draw the bitmaps
  glEnable (GL_TEXTURE_2D);
  bmplist->draw (cam->getx (),cam->gety (),cam->getz ());
  glDisable (GL_TEXTURE_2D);
  glDepthMask (GL_TRUE);

  // draw particles
  partlist->draw (cam->getx (),cam->gety (),cam->getz ());
  

  glEnable (GL_CULL_FACE);
  glEnable (GL_LIGHTING);
}

// removes all destroyed objects
void mission::destroy_objects ()
{

  objlist->destroy_all_objects ();
  bmplist->destroy_all_objects ();
  partlist->destroy_all_objects ();
  traillist->destroy_all_objects ();
  litelist->destroy_all_objects ();
}

// returns the world of this mission
world* mission::getworld ()
{
  return sea;
}

// deletes the collision infos of all objects
void mission::clear_collision_info ()
{
  listobj* obj1;
  obj1=objlist->get_first ();
  while (obj1!=NULL)
  {
    obj1->obj->clear_collision_info ();
    obj1=(listobj*)obj1->next;
  }
}

// performs the collision detection for all objects
void mission::collisiondetection ()
{
  clear_collision_info ();
  listobj* obj1;
  obj1=objlist->get_first ();
  while (obj1!=NULL)
  {
    switch (obj1->obj->Typ ())
    {
      case PLAYER:
        // check collision only with all other objects
        check_collision (obj1,0,1);
        break;
      case COMPUTER:
        // check collision and collision_soon with all other objects
        check_collision (obj1,0,1);
        break;
      case OTHER:
        // check collision and collision_soon with all other objects
        check_collision (obj1,0,1);
        break;
      case TURRET:
        // check collision_soon (fire) with all other objects
        // check collision with missiles
        check_collision (obj1,1,1);
        break;
      case BUILDING:
        // check collision with missiles
        check_collision (obj1,1,0);
        break;
    }
    obj1=(listobj*)obj1->next;
  }
}


// checks obj for collision with other objects
// typ ... collsion with 0 all objects, 1 missiles only
// coll_soon ... 1 check for soon_collision with other objects
void mission::check_collision (listobj* lobj, int typ, int coll_soon)
{
  listobj* tmpobj;
  object* oobj1;
  model* obj1;
  model* obj=(model*) lobj->obj;
  GLfloat d;
  int collide,i;
  if (typ==0||coll_soon==1)
  {
    tmpobj=(listobj*)lobj->next;
  }
  else
  {
    tmpobj=objlist->get_first (HOMING);
    if (tmpobj==NULL)
    {
      tmpobj=objlist->get_first (DIRECT);
    }
  }
  while (tmpobj!=NULL)
  {
    oobj1 = tmpobj->obj;
    // do not check for self collistion
    // do not check bitmaps,waypoints,particles
    if (oobj1!=obj&&oobj1->Typ ()!=BITMAP&&oobj1->Typ ()!=WAYPOINT&&oobj1->Typ ()!=PARTICLE)
    {
      obj1 = (model*) oobj1;
      d = distance (obj,obj1);
      // bounding sphere test
      collide = boundingsphere_collide (obj,obj1,d);
      // perform further collision tests if boundingsphere_test successful
      if (collide==1)
      {
        // check only missiles if typ==1
        if (typ==0||obj1->Typ ()==HOMING||obj1->Typ ()==DIRECT||obj1->Typ ()==TORPEDO)
        {
          collide = 0;
          i=0;
          // bounding box test
          while (collide==0&&i<14)
          {
            collide = obj->inside_bounding_box (obj1->bbc (i,0),obj1->bbc (i,1),obj1->bbc (i,2));
            if (collide==0)
            {
              collide = obj1->inside_bounding_box (obj->bbc (i,0),obj->bbc (i,1),obj->bbc (i,2));
            }
            i++;
          }
          if (collide==1)
          {
            // when two collide..
            obj->add_collision_object (obj1);
            obj1->add_collision_object (obj);
            // play some FX
            if (obj1->Typ ()!=HOMING&&obj1->Typ ()!=DIRECT&&obj1->Typ ()!=TORPEDO)
            {
              Camera* cam = (Camera*) getcam ();
              playsam (1,15000,distantvolume (cam->getx (), cam->gety (), cam->getz (), obj->getx (), obj->gety (), obj->getz ()),199);
            }
          }
        }
      }
      // perform collision soon test
      if (coll_soon==1)
      {
        // ignore missiles for collision_soon
        if (obj1->Typ ()!=HOMING&&obj1->Typ ()!=DIRECT&&obj1->Typ ()!=TORPEDO)
        {
          if (collisionsoon_detection (obj,obj1,d))
          {
            obj->add_collision_soon_object (obj1,d);
          }
          // if second obj is computer,turret or other do collision detection also the other way
          if (obj1->Typ ()==COMPUTER||obj1->Typ ()==TURRET||obj1->Typ ()==OTHER)
          {
            if (collisionsoon_detection (obj1,obj,d))
            {
              obj1->add_collision_soon_object (obj,d);
            }
          }
        }
      }
    }
    tmpobj=(listobj*)tmpobj->next;
  }
}


// checks if the boundingspheres of the two objects with distance d intersect
int mission::boundingsphere_collide (model *obj1, model *obj2, GLfloat d)
{
  if (d>obj1->bounding_rad ()+obj2->bounding_rad ())
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


// adds an object to the objectlist
void mission::add_object (object* obj)
{
  objlist->add_object (obj);
}

// add a bmp
void mission::add_bmp (bitmap *bmp)
{
  bmplist->add_object (bmp);
}

// add a trail
void mission::add_trail (trail *tr)
{
  traillist->add_object (tr);
}


objectlist* mission::get_objlist ()
{
  return objlist;
}

objectlist* mission::get_bmplist ()
{
  return bmplist;
}

objectlist* mission::get_traillist ()
{
  return traillist;
}

objectlist* mission::get_litelist ()
{
  return litelist;
}

objectlist* mission::get_alienlist ()
{
  return alienlist;
}



int mission::collisionsoon_detection (model* obj1, model* obj2, GLfloat d)
{
  float x1=obj2->getx ()-obj1->getx ();
  float y1=obj2->gety ()-obj1->gety ();
  float z1=obj2->getz ()-obj1->getz ();
  x1/=d;
  y1/=d;
  z1/=d;
  float x2=obj1->getdx ();
  float y2=obj1->getdy ();
  float z2=obj1->getdz ();
  float brad=1.5*obj2->bounding_rad ();
  float v = brad/d;
  if (v>1) v=1;
  float amax=fabs (asin (v));
  float w = x1*x2+y1*y2+z1*z2;
  if (w>1) w=1;
  if (w<-1) w=-1;
  float a = fabs (acos (w));
  if (a<=amax)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

// returns the cockpit
Cockpit* mission::getCockpit ()
{
  return cockpit;
}


int mission::getpausemode ()
{
  return pausemenu;}


goal_t* mission::get_goal (int i)
{
  if (i<num_goals)
    return &goals[i];
  else
    return NULL;
}

int mission::get_numgoals ()
{
  return num_goals;
}

void mission::check_goals ()
{
  listobj* tmp;
  listobj* tmp1;
  int success_save;
  int success_destroy;
  int success_reach;
  int success = 1;
  int failed = 0;
  int i;
  for (i=0;i<num_goals;i++)
  {
    success_save=1;
    success_destroy=1;
    success_reach=1;
    if (!goals[i].failed)
    {
      if (goals[i].savelist!=NULL)
      {
        tmp = goals[i].savelist->get_first ();
        while (tmp!=NULL)
        {
          if (objlist->in_list (tmp->obj)==NULL)
          {
            success_save=0;
          }
          tmp = tmp->next;
        }
      }
      if (!goals[i].success)
      {
        if (goals[i].destroylist!=NULL)
        {
          tmp = goals[i].destroylist->get_first ();
          while (tmp!=NULL)
          {
            if (objlist->in_list (tmp->obj)==NULL)
            {
              tmp1 = tmp->next;
              goals[i].destroylist->delete_entry (tmp);
              tmp = tmp1;
            }
            else tmp = tmp->next;
          }
          if (goals[i].destroylist->get_first ()==NULL) 
          {
            success_destroy=1;
          }
          else
          {
            success_destroy=0;
          }
        }
        if (goals[i].reachlist!=NULL)
        {
          tmp = goals[i].reachlist->get_first ();
          if (objlist->in_list (goals[i].reachobj)==NULL)
          {
            goals[i].reachobj=NULL;
          }
          else if (goals[i].reachobj->getmvstate ()==MV_DESTROY) 
          {
            goals[i].reachobj=NULL;
          }
          if (goals[i].reachobj!=NULL)
          {
            while (tmp!=NULL)
            {
              if (((waypoint*)(tmp->obj))->get_reached ())
              {
                tmp1 = tmp->next;
                goals[i].reachlist->delete_entry (tmp);
                tmp = tmp1;
              }  
              else tmp = tmp->next;
            }
            if (goals[i].reachlist->get_first ()==NULL) 
            {
              success_reach=1;
            }
            else
            {
              success_reach=0;
            }
          }
          else
          {
            success_reach = 0;
          }
        }
      }
      if (success_destroy&&success_save&&success_reach)
      {
        goals[i].success=1; 
      }
      else
      {
        goals[i].success=0;
      }
      if (!success_save)
      {
        goals[i].failed=1;
        goals[i].success=0;
      }
    }
  }
  for (i=0;i<num_goals;i++)
  {
    if (!goals[i].success) success = 0;
    if (goals[i].failed) failed = 1;
  }
  if (objlist->in_list (plr)==NULL)
  {
    // player destroyed
    failed = 1;
    player_dead = 1;
  }
  else if (plr->getmvstate ()==MV_DESTROY)
  {
    // player destroyed
    failed = 1;
    player_dead = 1;
  }
  if (success==1&&failed==0)
  {
    // check if player still exists
    if (objlist->in_list (plr)!=NULL)
    {
      // check if player has reached all waypoints
      // check if player is near endpoint
      tmp = (plr->get_pathlist ())->get_first ();
      while (tmp!=NULL)
      {
        if (!((waypoint*)(tmp->obj))->get_reached ())
        {
          success = 0;
        }
        else if (((waypoint*)(tmp->obj))->get_end ())
        {
          if (distance (plr,tmp->obj)>WAYPT_PL_DIST)
          {
            success = 0;
          }
        }
        tmp = tmp->next;
      }
    }
  }
  if (failed==1)
  {
    mis_failure = 1;
  }
  else if (success==1)
  {
    mis_success = 1;
  }
}

int mission::get_failure ()
{
  return mis_failure;
}

int mission::get_success ()
{
  return mis_success;
}

int mission::get_player_dead ()
{
  return player_dead;
}

int mission::get_demo ()
{
  return demo;
}

void mission::set_failure ()
{
  mis_failure = 1;
}

void mission::set_success ()
{
  mis_success = 1;
}

