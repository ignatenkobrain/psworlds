// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "camera.h"
#include "model.h"
#include "fonts.h"
#include "matrices.h"
#include "mission.h"
#include "player.h"


Camera::Camera ()
{
  // initialize the camera with coordinates 0,0,0 and no bound object

  setPos (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  setZoom (1.0);
  setActor (NULL);
  setCamstate (CAM_STILL);
  setFreePos (BEHINDDISTANCE, 0.0, 0.0);
  rotspeed = 90.0; distspeed = 30.0;
}

Camera::~Camera ()
{
  // nothing to do here because no resources allocated
}



void Camera::getObjectPos (object *mdl)
{
  // get the position and angle of the bound object
  // the actual camera position and orientation depends on the camstate

  GLfloat ox, oy, oz, oxrot, oyrot, ozrot;
  GLfloat xdiff, ydiff, zdiff;
  GLfloat a, b, c;
  GLfloat nxr, nyr, nzr;
  int i;
  GLdouble* tmp;

  if (mdl!=NULL)
  {

    // get the object´s coordinates and orientation
    ox = mdl->getx ();
    oy = mdl->gety ();
    oz = mdl->getz ();

    oxrot = mdl->getrotx ();
    oyrot = mdl->getroty ();
    ozrot = mdl->getrotz ();
    
    tmp = ((model*)mdl)->getinvmat ();
    for (i=0;i<16;i++) {
      mat[i]=tmp[i];
    }
    tmp = ((model*)mdl)->getmat ();
    for (i=0;i<16;i++) {
      invmat[i]=tmp[i];
    }

    // now decide where to actually put the camera
    switch (camstate)
    {
    case CAM_STILL:  // fixed position somewhere
      {
        // do not affect camera position
        break;
      }
    case CAM_FLYBY: // watch the object from some given point
      {
        xdiff = ox-x; ydiff = oy-y; zdiff = oz-z;
        a = xdiff;
        b = ydiff;
        c = zdiff;

        nxr = 180;
        nyr = atan2(a, c)*PI_GRAD;
        nzr= 180;
        set_identity (mat);
        set_identity (invmat);
        rotatez (mat,invmat,nzr);
        rotatey (mat,invmat,nyr);
        rotatex (mat,invmat,nxr);
        setPos (x,y,z, nxr, nyr, nzr);
      
        break;
      }
    case CAM_COCKPIT: // view from object cockpit
      {
        setPos (ox,oy,oz,oxrot,oyrot,ozrot);
        break;
      }
    case CAM_BEHIND: // view object from behind
    {
      GLfloat ny=tan (15*GRAD_PI);
      GLfloat bdst = ((model*)mdl)->bounding_rad ()*2.8;
      xdiff = invmat[4]*ny+invmat[8];
      ydiff = invmat[5]*ny+invmat[9];
      zdiff = invmat[6]*ny+invmat[10];
      setPos (ox+xdiff*bdst, oy+ydiff*bdst, oz+zdiff*bdst, oxrot, oyrot, ozrot);
      break;
    }
    case CAM_FREEVIEW:  // free view around the object
      {
        GLfloat ticks = gettime ();
        char* keys = getkeys ();
        GLfloat dststep = distspeed*ticks;
        GLfloat rotstep = rotspeed*ticks;
        GLfloat xdiff, zdiff,altmod;

        if (!((mission*)getmission ())->get_demo ())
        {
          if (keys[SDLK_9]) // zoom in
          {
            setFreePos (free_dist-dststep, 0, free_yrot);
          }
          if (keys[SDLK_0]) // zoom out
          {
            setFreePos (free_dist+dststep, 0, free_yrot);
          }
          if (keys[SDLK_p]) // rotate left
          {
            setFreePos (free_dist, 0, free_yrot-rotstep);
          }
          if (keys[SDLK_o]) // rotate right
          {
            setFreePos (free_dist, 0, free_yrot+rotstep);
          }
        }
        
        xdiff = cos (free_yrot*GRAD_PI);
        zdiff = sin (free_yrot*GRAD_PI);

        altmod=0;
        if (actor!=NULL)
        {
          if (actor->Typ ()==BUILDING)
            altmod=5.0;
        }

        setPos (ox+xdiff*free_dist, oy+altmod, oz+zdiff*free_dist, 0.0, -free_yrot+90.0, 0.0);


        break;
      }
    case CAM_TOP: // view target from top
      {
        setPos (ox, oy+BEHINDDISTANCE*2, oz, -90, 0, 0);
        break;
      }
    case CAM_BOT: // view target from bottom
      {
        setPos (ox, oy-BEHINDDISTANCE*2, oz, 90, 0, 0);
        break;
      }

    default: write_log ("Undefined Camstate!\n"); break;
    }
  }
}



void Camera::seekObject ()
{
  // bind camera to player´s ship
  // well quite.. i´m not sure we need this anymore
}


void Camera::setFreePos (GLfloat dist, GLfloat xr, GLfloat yr)
{
  // set the freecam position
  if (dist<15.0) dist=15.0;
  while (xr>360.0) xr-=360.0; while (xr<0.0) xr+=360.0;
  while (yr>360.0) yr-=360.0; while (yr<0.0) yr+=360.0;

  free_dist = dist;
  free_xrot = xr;
  free_yrot = yr;
}


void Camera::setActor (object *mdl)
{
  // bind new object
  actor = mdl;
}



void Camera::setPos (GLfloat nx, GLfloat ny, GLfloat nz, GLfloat xr, GLfloat yr, GLfloat zr)
{
  // set cam position
  x = nx; y = ny; z = nz;
  xrot = xr; yrot = yr; zrot = zr;
}


void Camera::setZoom (GLfloat nzoom)
{
  // set new zoom
  zoom = nzoom;
  reshape (getxres (), getyres (), zoom);
}


void Camera::setCamstate (int state)
{
  // assign new camerastate

  // if the target is not the player, Cockpit is forbidden!
  if (actor!=NULL)
  {  
    if (actor->Typ ()!=PLAYER)
    {
      if (state==CAM_COCKPIT) state = camstate; // prohibit Cockpit
      else camstate = state;
    }
    else
      camstate = state;
  }
}


int Camera::getCamstate ()
{
  return camstate;
}


void Camera::render ()
{
  // start the graphics pipleline
  getObjectPos (actor); // set the new location for the camera
  
  if (camstate==CAM_COCKPIT||camstate==CAM_BEHIND||camstate==CAM_FLYBY)
  {  
    glPushMatrix ();
      glMultMatrixd (mat);
      glPushMatrix ();
        glTranslatef (-x, -y, -z);
  }
  else
  {
    glPushMatrix ();
      glRotatef (-yrot, 0.0, 1.0, 0.0);
      glRotatef (-zrot, 0.0, 0.0, 1.0);
      glRotatef (-xrot, 1.0, 0.0, 0.0);
      glPushMatrix ();
        glTranslatef (-x, -y, -z);
  }
  // next in the pipeline are the models..
}



void Camera::spherecam ()
{
  // create the cool effect of light coming from the surface and gradually fading away
  // the deeper it gets
  // this is achieved by an almost transparent sphere encapsulating the camera

  int i;
  GLfloat minusser;

  minusser = calcdepth (this->z)/2300.0*0.15;
  if (minusser>0.15) minusser=0.15;


  glPopMatrix ();

  glBindTexture (GL_TEXTURE_2D, 0);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_LIGHTING);
  glDisable (GL_CULL_FACE);
  glBegin (GL_TRIANGLE_FAN);
    // upper sphere half first
    glColor4f (1.0,1.0,1.0,0.20-minusser);
    glVertex3f (0.0,2.5,0.0);
    glColor4f (1.0,1.0,1.0,0.0);
    for (i=0; i<=360.0; i+=30)
    {
      glVertex3f (cos (i*GRAD_PI)*3, 0.5, sin (i*GRAD_PI)*3);
    }
  glEnd ();
  
  glBegin (GL_TRIANGLE_FAN);
    // lower sphere half
    glColor4f (0.0, 0.0, 0.0, 0.19-minusser);
    glVertex3f (0.0,-2.5,0.0);
    glColor4f (0.0, 0.0, 0.0, 0.0);
    for (i=0; i<=360.0; i+=30)
    {
      glVertex3f (cos (i*GRAD_PI)*3, -0.5, sin (i*GRAD_PI)*3);
    }
  glEnd ();

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_CULL_FACE);
}


void Camera::retreat ()
{
  // unbind the Camera from its bound object and find some distant place to watch
  // the object. this is useful if the object is getting destroyed
  
  setCamstate (CAM_STILL);

  float randy = rand ()/(float)RAND_MAX*360.0;
  GLfloat xdiff = cos (randy*GRAD_PI);
  GLfloat zdiff = sin (randy*GRAD_PI);
  GLfloat dst = ((model*)actor)->bounding_rad ()*5.0;

  setPos (actor->getx ()+xdiff*dst, actor->gety (), actor->getz ()+zdiff*dst, 0.0, -randy+90.0, 0.0);

  actor = NULL;
}


void Camera::toggle ()
{
  // bind cam to player´s target or back to the player (if it exists)
  listobj* lpl = ((mission*)getmission ())->get_objlist ()->get_first (PLAYER);
  if (lpl!=NULL)
  {
    player* pl = (player*)lpl->obj;
    if (getActor () == pl)
    {
      // if the player has a currently selected target, bind it!
      if (pl->getTarget ()!=NULL)
      {
        prevstate = camstate;
        setActor (pl->getTarget ()->obj);    
        setCamstate (CAM_BEHIND);
      }
    }
    else
    {
      // otherwise bind back to the player
      setActor (pl);
      setCamstate (prevstate);
    }

  }


}




// providers

GLfloat Camera::getx ()
{
  return x;
}

GLfloat Camera::gety ()
{
  return y;
}

GLfloat Camera::getz ()
{
  return z;
}

GLfloat Camera::getxrot ()
{
  return xrot;
}

GLfloat Camera::getyrot ()
{
  return yrot;
} 

GLfloat Camera::getzrot ()
{
  return zrot;
}


GLdouble* Camera::getMat ()
{
  return mat;
}

GLdouble* Camera::getInvmat ()
{
  return invmat;
}

GLfloat Camera::getdepth ()
{
  return calcdepth (y);
}

object* Camera::getActor ()
{
  return actor;
}


