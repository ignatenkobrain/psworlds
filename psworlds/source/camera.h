// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __CAMERA_H_
#define __CAMERA_H_

// camera defines the position of the viewer
//
// if not in STILL or FLYBY mode, the camera is assigned an object.
// the camera then shows the scene from the viewpoint of the corresponding object, either
// the player´s ship, an enemy, a missile, etc.
//
// there are a number of different camera states:
// STILL: the camera position isn´t updated at all
// FLYBY: the camera might follow a given path, but that remains to be determined
// COCKPIT: the camera shows the action from the object´s cockpit
// BEHIND: the camera shows the object from above behind
// FREEVIEW: the camera is bound to an object, but can be moved around the object as desired

#include "psworlds.h"
#include "object.h"


#define CAM_STILL 0
#define CAM_FLYBY 1
#define CAM_COCKPIT 2
#define CAM_BEHIND 3
#define CAM_FREEVIEW 4
#define CAM_TOP 5
#define CAM_BOT 6

#define BEHINDDISTANCE 80.0


class Camera  
{
private:
  //private variables
  GLfloat x,y,z;        // camera position
  GLfloat xrot, yrot, zrot;  // camera rotation angles
  GLfloat zoom;        // zoom factor
  GLfloat free_dist, free_xrot, free_yrot;  // free cam parameters
  GLfloat rotspeed, distspeed;  // speed for freecam movement 
  object *actor;        // the object the camera is bound to
  GLdouble mat[16];
  GLdouble invmat[16];
  int camstate;        // camera state
  int prevstate;        // camstate of last bound player

  void getObjectPos (object *mdl);  // get coordinates from actor´s viewpoint
  void seekObject ();      // seek a new object to bind the camera to

public:
  // public functions
  Camera ();
  virtual ~Camera ();

  void setFreePos (GLfloat dist, GLfloat xr, GLfloat yr);  // set free view around ship
  void setActor (object *mdl);  // set a new actor for the camera
  void setPos (GLfloat nx, GLfloat ny, GLfloat nz, GLfloat xr, GLfloat yr, GLfloat zr);  // set position manually
  void setZoom (GLfloat nzoom);  // set the zoom factor
  void setCamstate (int state);    // set the new camera state
  int getCamstate ();        // return the camera state
  GLfloat getx ();
  GLfloat gety ();
  GLfloat getz ();
  GLfloat getxrot ();
  GLfloat getyrot ();
  GLfloat getzrot ();
  object* getActor ();

  GLdouble *getMat ();
  GLdouble *getInvmat ();

  GLfloat getdepth ();

  void render ();  // start the rendering pipeline by setting up world translation matrix
  void spherecam ();  // create the eerie light effect of an ocean
  void retreat ();  // unbind from the object and find som nice place to watch it from a distance
  void toggle ();  // switch to target / back to player

};



#endif
