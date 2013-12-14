// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "psworlds.h"
#include "player.h"
#include "sound.h"
#include "world.h"
#include "mission.h"
#include "camera.h"
#include "time.h"
#include "fonts.h"
#include "menu.h"
#include "displaylistlist.h"
#include "texturelist.h"
#include "headlight.h"
#include "intro.h"
#include "startscreen.h"
#include "outro.h"
#include "briefing.h"
#include <libconfig.h++>

char    keys[1024];        // keyboard buffer
double    Time;          // microticks passed since last frame
int      fulltime;        // miliseconds passed since init
int      fps = 0;        // display FPS, or not
int      gs;            // the game state
int      prev_gs;        // the previously to a menu call game state
int      cheated = 0;
mission*  mis = NULL;        // the currently running mission
Camera*    cam = NULL;        // the camera
GLuint    screenx, screeny;    // screen size
GLubyte    screenbpp;        // color depth
GLuint    fullscreen;        // determines whether to run fullscreen
GLfloat    whalesound=50;      // random time left until another whale cry is played
int      whalecluster=2;      // another random whale var
menu*    menus;          // the menu objects
int      mission_nr;        // the mission number
int      mission_advance=-1;    // last mission completed, -1 if no game running
int      num_missions;      // number of available missions
displaylistlist* dls;        // all displaylists
texturelist* tl;          // all textures
int      wire=0;          // wireframe mode?
int      caustics=0;        // do we have a second texture unit?
int      yzhack=0;        // do we want the keyboard hack?


void ambient ()
{
  if (gs==GS_INGAME) // play only while actually gaming
  {
    // at random occasions, play some eerie whale cries
    GLfloat ticks = gettime ();

    whalesound-=ticks;
    if (whalesound<0) // play a new whale sound
    {
      playsam ((int)(9+((floor)(rand ()/(float)RAND_MAX*5))), 16000, (int)(195-(rand ()/(float)RAND_MAX*100)),255);
      whalecluster--;
      if (whalecluster<=0) // whalecluster defines if there should be multiple cries within a short timespan
      {
        whalecluster = 1+(int)(rand ()/(float)RAND_MAX*4);
        whalesound = 55.0+(rand ()/(float)RAND_MAX*115.0); // maximum 210 seconds until next whale cry
      }
      else
      {
        whalesound = 2+rand ()/(float)RAND_MAX*5.0; // maximum 7 seconds until next whale cry
      }
    }
  }
}



void killmenu ()
{
  if (menus!=NULL)
  {
    if (menus->killme==1)
    {
      // if the menu wants to set a new state, then do so
      if (menus->new_gs!=0) gs = menus->new_gs;
      else gs = prev_gs;
      delete menus;
      menus = NULL;
    }
  }
}


void kbd ()
{
  // poll the keyboard events

  SDL_Event event;
  while (SDL_PollEvent (&event))
  {
    if (event.type == SDL_KEYDOWN)
    {
      keys[event.key.keysym.sym] = 1;
    }
    else if (event.type == SDL_KEYUP)
    {
      keys[event.key.keysym.sym] = 0;
    }
  }
  // american keyboard hack
  if (yzhack==1)
  {
    if (keys[SDLK_y]==1) keys[SDLK_z]=1;
    if (keys[SDLK_y]==0) keys[SDLK_z]=0;
  }
}

void timing ()
{
  // measure miliseconds passed since last frame
  int lasttime = fulltime;
  char buf[50];

  fulltime = SDL_GetTicks ();
  Time = fulltime-lasttime;
  
  if (Time==0) Time=1; // avoid divisions by zero
  
  sprintf (buf, "FPS: %3.2f\n",1000.0/Time);
  if (fps) print (10, 20, buf, 0, 0.1, 1.0, 0.1, 1.0);
  Time/=1000.0;
  
}


void readconfig ()
{
  // read contents of the config file and set stuff accordingly
  FILE *cf;
  char line[255];
  int val = 0;
  int value = 0;

  _FBUF

  // default values
  screenx = 800;
  screeny = 600;
  screenbpp = 16;
  fullscreen = 0;
  num_missions = 4;
  yzhack = 0;

  write_log ("Reading config\n");

  static char *conffile = getenv ("HOME");
  strcat (conffile, "/.psworlds.conf");
  sprintf (buf, "conffile: %s", conffile);
  write_log (buf);
  using namespace libconfig;
  Config cfg;
  Setting &root = cfg.getRoot ();
  root.add ("width", Setting::TypeInt) = (int) screenx;
  root.add ("height", Setting::TypeInt) = (int) screeny;
  root.add ("depth", Setting::TypeInt) = (int) screenbpp;
  root.add ("fullscreen", Setting::TypeInt) = (int) fullscreen;
  root.add ("missions", Setting::TypeInt) = (int) num_missions;
  root.add ("yzhack", Setting::TypeInt) = (int) yzhack;
  try {
    cfg.readFile (conffile);
    cfg.lookupValue ("width", screenx);
    cfg.lookupValue ("height", screeny);
    cfg.lookupValue ("depth", (int &) screenbpp);
    cfg.lookupValue ("fullscreen", fullscreen);
    cfg.lookupValue ("missions", num_missions);
    cfg.lookupValue ("yzhack", yzhack);
  } catch (FileIOException &fioex) {
    cfg.writeFile (conffile);
  } catch (ParseException &pex) {
    fprintf (stderr, "Incorrect configuration! Remove %s please.", conffile);
    exit (-1);
  }

  if (fullscreen == 1) fullscreen = SDL_FULLSCREEN;

  write_log ("Config file processed!\n");
  return;
}


void loadmission ()
{
  // load the desired mission file
  _FBUF
  FILE* fpeek;

  sprintf (buf,"Attempting to load mission %d\n",getMissionNum ()); write_log (buf);

  if (mis!=NULL) delete mis;
  if (cam!=NULL) delete cam;
  cam = new Camera ();

  if (getMissionNum ()!=0)
  {
    // draw loading message
    glColor4f (0.0,0.0,0.2,0.7);
    glBegin (GL_QUADS);
      glVertex3f (-1.0,0.5,-1.5000001);
      glVertex3f (-1.0,-0.5,-1.5000001);
      glVertex3f (1.0,-0.5,-1.5000001);
      glVertex3f (1.0,0.5,-1.5000001);
    glEnd ();

    sprintf (buf,"LOADING MISSION %d...",getMissionNum ());
    print (300,280,buf,1,1.0,1.0,1.0,1.0);
    sprintf (buf,"missions/mission%d.txt",getMissionNum ());
    xpath (buf);
    fpeek = fopen (buf,"r");
    fgets (buf, 100, fpeek);
    fclose (fpeek);
    print (strlen (buf)/2*-CHARLEN+400,320,buf,1,0.8,0.8,1.0,1.0);
  
    drawstrings ();
    glFlush ();
    SDL_GL_SwapBuffers ();
  }
  // load file
  sprintf (buf,"missions/mission%d.txt",getMissionNum ());
  write_log ("Starting New Mission!\n");
  mis = new mission (buf);
  timing ();
  timing ();

  if (!mis->get_demo ())
  {
    fademusic (1.3);
    stopmusic ();
    briefing* brf = new briefing (getMissionNum ());
    brf->run ();
    delete brf;
    cam->setCamstate (CAM_COCKPIT);
  }
  timing ();
  timing ();
}




void setMission (int ms)
{
  mission_nr = ms;
  if (mission_nr>num_missions) mission_nr = num_missions;
}

int getMissionNum ()
{
  return mission_nr;
}

void setCheated (int c)
{
  cheated = c;
}

int getCheated ()
{
  return cheated;
}


int getGamestate ()
{
  // provide access to gamestate variable
  return gs;
}

double gettime ()
{
  // return time since last frame
  return Time;
}

char* getkeys ()
{
  // provide keyboard buffer
  return keys;
}

void* getcam ()
{
  // access to camera
  return cam;
}

void* getworld ()
{
  // access to world
  return mis->getworld ();
}

void* getmission ()
{
  // access to mission
  return mis;
}

void* getdisplaylists ()
{
  // access to displaylistlist
  return dls;
}

void* gettexturelist ()
{
  // access to texturelist
  return tl;
}

int getxres ()
{
  return screenx;
}

int getyres ()
{
  return screeny;
}


void initGL (void)
{  
    write_log ("Initializing OpenGL.. ");

  // light vars
  GLfloat litediff[] = { 0.38, 0.38, 0.96, 1.0 };
  GLfloat litespec[] = { 0.5, 0.5, 1.0, 1.0 };
  GLfloat liteamb[] = { 0.10, 0.10, 0.8, 1.0 };

  // fog vars
  GLfloat fogcol[] = { 0.0, 0.1, 0.37, 1.0 };
  GLint fogmode = GL_LINEAR;


  glClearColor (0.0, 0.1, 0.37, 0.0);
  glShadeModel (GL_SMOOTH);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  
  // lighting stuff
  glLightfv (GL_LIGHT0, GL_DIFFUSE, litediff);
  glLightfv (GL_LIGHT0, GL_SPECULAR, litespec);
  glLightfv (GL_LIGHT0, GL_AMBIENT, liteamb);
  
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  
  // fog stuff
  glEnable (GL_FOG);
  glFogi (GL_FOG_MODE, fogmode);
  glFogfv (GL_FOG_COLOR, fogcol);
  glFogf (GL_FOG_DENSITY, 0.1);
  glHint (GL_FOG_HINT, GL_NICEST);
  glFogf (GL_FOG_START, 250.0);
  glFogf (GL_FOG_END, 700.0);

  // blending stuff
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glHint (GL_POINT_SMOOTH_HINT,GL_NICEST);        // Really Nice Point Smoothing

  // texture mapping stuff
  glPixelStorei (GL_UNPACK_ALIGNMENT,1);  // determine pixel storage format
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // enable environment mapping
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

  // check number of texture units
  //glGetIntegerv (GL_MAX_TEXTURE_UNITS_ARB, &caustics);
  //sprintf (buf,"Found &d Texture units!\n",caustics);

    write_log ("done!\n");
}

void reshape (int w, int h, GLfloat zoom)
{
  // called whenever the window is resized or set up

  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum (-1.0, 1.0, -0.8, 0.8, 1.5, 720.0);
  glMatrixMode (GL_MODELVIEW);
}



void init ()
{
  // do initialisation stuff
  int i;

  //SDL_ShowCursor (SDL_DISABLE);
  srand ((unsigned)time (NULL));
  for (i=0; i<256; i++) keys[i]=0; // init keyboard buffer
  init_log ();
    write_log ("Starting Possible Worlds\n\n");
  initAudio ();
}



void cleanup ()
{
  // does some cleanup work upon application exit
  if (dls!=NULL) delete dls;
  if (tl!=NULL) delete tl;
  if (mis!=NULL) delete mis;

  // close audio device
  killAudio ();

  // close log
    write_log ("\nThank you for playing Possible Worlds!\n");

  SDL_ShowCursor (SDL_ENABLE);
}


void othercontrols ()
{
  // handle special comands
  

  if (keys[SDLK_ESCAPE] && gs==GS_INGAME) // open ingame-menu
  {
    if (mission_advance == -1)
    {
      menus = new menu (MENU_TITLE, NULL, "");
    }
    else
    {
      if (mis->get_demo ())
      {
        menus = new menu (MENU_GAME, NULL, "");
      }
      else
      {
        menus = new menu (MENU_INGAME, NULL, "");
      }
    }
    prev_gs = gs; gs = GS_MENU;
    keys[SDLK_ESCAPE] = 0;
    if (mis->getpausemode ()==1) stopAudio (1);
  }

  if (keys[SDLK_RETURN] && gs==GS_MISSIONEND)
  {
    keys[SDLK_RETURN] = 0;
    stopAudio (1);
    gs = GS_MISSIONEND1;
  }

  // wireframe on/off
  if (keys[SDLK_F1])
  {
    if (wire==1) {glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); wire=0;}
    else {glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); wire=1; }
    keys[SDLK_F1]=0;
  }

  // fog/no fog naah that's of no use
  if (keys[SDLK_F2])
  {
    //if (glIsEnabled (GL_FOG)) glDisable (GL_FOG); else glEnable (GL_FOG);
    //keys[SDLK_F2]=0;
  }

  // array drawing / manual drawing. array drawing crashes on geforce, so it's deactivated
  if (keys[SDLK_F3])
  {
    //if (mis->getworld ()->getdrawmode ()==0) mis->getworld ()->setdrawmode (1); else mis->getworld ()->setdrawmode (0);
    //keys[SDLK_F3]=0;
  }

  // fps on / off
  if (keys[SDLK_F4])
  {
    fps = !fps;
    keys[SDLK_F4]=0;
  }

}




void render ()
{
  // do the gfx pipeline
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_CULL_FACE);
  
  mis->getworld ()->adjustlight ();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // start!
  // cam transformation
  cam->render ();
  
  // there´s a PushMatrix () in cam->render
  
    // switch alienlights on
    mis->get_alienlist ()->draw (cam->getx (), cam->gety (), cam->getz ());

    // render world
    //mis->getworld ()->draw (cam->getx (), cam->gety (), cam->getz (), cam->getxrot (), cam->getyrot (), cam->getzrot (),1);
    mis->getworld ()->draw (cam->getx (), cam->gety (), cam->getz (), cam->getxrot (), cam->getyrot (), cam->getzrot (),0);

    // render objects
    mis->draw_objects ();

    // switch the alienlights off
    mis->get_alienlist ()->draw (cam->getx (), cam->gety (), cam->getz ());

    // spherecam
    cam->spherecam ();


  glPopMatrix ();

  // prepare lens flares
  prepareflares ();

  // the following operations need no z-buffer and culling
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);
  glDisable (GL_LIGHTING);

  // now draw the cockpit, if necessary
  mis->drawcockpit ();

  // draw fonts
  drawstrings ();  

  // draw lensflares
  finalizeflares ();
  drawflares ();
  
  // draw menus
  if (menus!=NULL)
  {
    menus->draw ();
  }


  // the usual suspects
  glFlush ();
  SDL_GL_SwapBuffers ();
}




void gameloop () 
{
  // the main loop

  cam->setCamstate (CAM_COCKPIT);
  write_log ("\nRunning game...\n\n");
  GLfloat cam_time = 0;
  int showoutro = 0;
  _FBUF

  while (getGamestate () != GS_QUIT) 
  {
    timing ();          // do the timing
    kbd ();            // handle keyboard input
    othercontrols ();      // handle commands like "flat / smooth shading", etc..
    
    if (menus!=NULL)        // handle menu control
      menus->handle ();

    killmenu ();          // check if menu needs killing

    if (mis->get_failure ()||mis->get_success ())
    {
      mis->camcontrol ();      // move the camera around the world
      mis->collisiondetection ();  // check for collisions
      pregetProjections ();    // for lens flare projection, pre-get matrices
      mis->move_objects ();    // move all the game objects
      mis->destroy_objects ();    // removes all destroyed objects
      char text[255];
      if (mis->get_failure ())
      {
        if (mis->get_player_dead ())
        {
          strcpy (text,"YOU DIED IN ACTION. MISSION FAILED!\0");
        }
        else
        {
          strcpy (text,"MISSION FAILED!\0");
        }
      }
      else if (mis->get_success ())
      {
        strcpy (text,"MISSION COMPLETED!\0");
        if (getCheated ())
        {
          print (100,120,"YOU CHEATED! SUCCESS NOT ACKNOWLEDGED!",1,1,1,1,1);
        }
      }
      print (100,100,text,1,1.0,1.0,1.0,1.0);
      int l=140;
      for (int i=0;i<mis->get_numgoals ();i++)
      {
        if (mis->get_goal (i)->failed)
        {
          sprintf (buf,"%d. %s: FAILED",i+1,mis->get_goal (i)->text); print (100,l+=20,buf,0,1,1,1,1);
        }
        else if (mis->get_goal (i)->success)
        {
          sprintf (buf,"%d. %s: OK",i+1,mis->get_goal (i)->text); print (100,l+=20,buf,0,1,1,1,1);
        }
        else
        {
          sprintf (buf,"%d. %s: NOT COMPLETED",i+1,mis->get_goal (i)->text); print (100,l+=20,buf,0,1,1,1,1);
        }
      }
      print (100,l+40,"PRESS ENTER TO PROCEED.",0,1,1,1,1);
      if (gs==GS_INGAME) gs = GS_MISSIONEND;
      ambient ();          // play ambient sounds    
    }
    else if (!((gs==GS_MENU) && (mis->getpausemode ()==1)))
    {
      if (mis->get_demo ())
      {
        cam_time += gettime ();
        if (cam_time>7)
        {
          cam->setFreePos (50+200*rand ()/(float)RAND_MAX,0,360*rand ()/(float)RAND_MAX);
          objectlist* l = mis->get_objlist ();
          if (l->in_list (cam->getActor ()!=NULL))
          {
            listobj* lobj = l->in_list (cam->getActor ());
            if (lobj->next!=NULL)
            {
              if (lobj->next->obj->Typ ()==COMPUTER)
              {
                cam->setActor (lobj->next->obj);
              }
              else
              {
                cam->setActor (l->get_first ()->obj);
              }
            }
            else
            {
              cam->setActor (l->get_first ()->obj);
            }
          }
          else
          {
            cam->setActor (l->get_first ()->obj);
          }
          cam_time = 0;
        }
        mis->collisiondetection ();  // check for collisions
        pregetProjections ();    // for lens flare projection, pre-get matrices
        mis->move_objects ();    // move all the game objects
        mis->destroy_objects ();    // removes all destroyed objects
      }
      else
      {
        mis->camcontrol ();      // move the camera around the world
        mis->collisiondetection ();  // check for collisions
        pregetProjections ();    // for lens flare projection, pre-get matrices
        mis->move_objects ();    // move all the game objects
        mis->destroy_objects ();    // removes all destroyed objects
        mis->check_goals ();      // check mission goals
      }
      ambient ();          // play ambient sounds    
    }
    if (gs==GS_NEWGAME)
    {
      
      setMission (1);
      mission_advance=0;
      menus = new menu (MENU_GAME, NULL, "");
      prev_gs = GS_INGAME; gs = GS_MENU;
      if (mis->getpausemode ()==1) stopAudio (1);
      timing ();
      timing ();
    }
    if (gs==GS_QUITGAME)
    {
      menus = new menu (MENU_TITLE, NULL, "");
      prev_gs = GS_INGAME; gs = GS_MENU;
      if (mis->getpausemode ()==1) stopAudio (1);
      mission_advance=-1;  
    }
    if (gs==GS_ABORT)
    {
      menus = new menu (MENU_GAME, NULL, "");
      prev_gs = GS_INGAME; gs = GS_MENU;
      int i = getMissionNum ();
      setMission (0);
      loadmission ();
      setMission (i);
      if (mis->getpausemode ()==1) stopAudio (1);
      cam->setCamstate (CAM_FREEVIEW);
      timing ();
      timing ();
    }
    if (gs==GS_LOADGAME)
    {
      setMission (mission_advance+1);
      menus = new menu (MENU_GAME, NULL, "");
      prev_gs = GS_INGAME; gs = GS_MENU;
      if (mis->getpausemode ()==1) stopAudio (1);
    }
    if (gs==GS_MISSIONEND1)
    {
      if (!getCheated ())
      {
        if (!mis->get_failure ()&&mis->get_success ())
        {
          if (getMissionNum ()==mission_advance+1)
          {
            setMissionAdvance (mission_advance+1);
            setMission (mission_advance+1);
          }
          if (mission_advance==num_missions && getMissionNum () == mission_advance)
          {
            showoutro = 1;
          }
        }
      }
      setCheated (0);
      int i = getMissionNum ();
      setMission (0);
      loadmission ();
      setMission (i);
      if (mis->getpausemode ()==1) stopAudio (1);
      cam->setCamstate (CAM_FREEVIEW);
      if (showoutro)
      {
        showoutro = 0;
        intro* outr = new intro (OUTRO);
        outr->run ();
        delete outr;
        outro* credits = new outro ();
        credits->run ();
        delete credits;
      }
      sprintf (buf, "sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
      menus = new menu (MENU_GAME, NULL, "");
      prev_gs = GS_INGAME; gs = GS_MENU;
      timing ();
      timing ();
    }
    if (gs==GS_MENU)
    {
      if (mission_advance != -1)
      {
        if (mis->get_demo ())
        {
          if (mission_advance == num_missions)
            print (200,80,"ALL MISSIONS COMPLETED",1,1.0,1.0,1.0,1.0);
        }
      }
    }

    render ();          // rendering pipeline
    updateAudio ();        // check audio systems  
  }
  fademusic (1.0);
  stopmusic ();
  write_log ("Quitting game!\n");
}


int  getMissionAdvance ()
{
  return mission_advance;
}

int  getNumMission ()
{
  return num_missions;
}

int getWire ()
{
  return wire;
}

void setMissionAdvance (int ma)
{
  mission_advance = ma;
  if (mission_advance > num_missions) mission_advance = num_missions;
}


int main (int argc, char **argv) 
{
  // the main function. initializes openGL and Audio Device
  _FBUF

  init ();
  readconfig ();
  
  // initialize glut, SDL and openGL  
  
  glutInit (&argc, argv);
  SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_SetVideoMode (screenx, screeny, screenbpp, SDL_OPENGL | fullscreen);
  SDL_ShowCursor (SDL_DISABLE);
  
  initGL ();
  atexit (SDL_Quit);

  // load bitmap textures
  loadbmps ();

  // load all samples
  loadsampleset ();

  // load all fonts
  loadfontset ();

  // load lens flares
  loadflaregfx ();

  
  // the camera object
  cam = new Camera ();

  // precalc stuff
  precalc_angles ();

  // default mission is 1
  setMission (0);
  dls = new displaylistlist ();
  tl = new texturelist ();
  loadmission ();
  cam->setCamstate (CAM_FREEVIEW);
  cam->setFreePos (200,0,50);

  timing (); // start timer
  fulltime = Time;
  
  // play splash screen
  write_log ("Loading intro..\n");
  startscreen* startscr = new startscreen ();
  startscr->run ();
  delete startscr;
  
  // play intro
  intro* intr = new intro (INTRO);
  intr->run ();
  delete intr;
  
  // load demo mod
  sprintf (buf, "sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
  menus = new menu (MENU_TITLE, NULL, "");
  prev_gs = GS_INGAME; gs = GS_MENU;

  mission_advance=-1;
  
  // enter game loop
  timing ();
  timing ();
  
  gameloop ();  
  
  cleanup ();
  return 0;
}
