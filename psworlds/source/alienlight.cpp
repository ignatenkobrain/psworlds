// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "alienlight.h"
#include "model.h"
#include "mission.h"
#include "fonts.h"

alienlight::alienlight (int num, float X,float Y,float Z, float ambr, float ambg, float ambb, float amba, float difr, float difg, float difb, float difa, float specr, float specg, float specb, float speca, float att, object* obj)
        :object (X,Y,Z,0,0,0)
{
  _FBUF
  sprintf (buf,"Attenuation: %3.2f\n",att); write_log (buf);
  switch (num%7)
  {
    case 1: light = GL_LIGHT1; break;
    case 2: light = GL_LIGHT2; break;
    case 3: light = GL_LIGHT3; break;
    case 4: light = GL_LIGHT4; break;
    case 5: light = GL_LIGHT5; break;
    case 6: light = GL_LIGHT6; break;
    case 0: light = GL_LIGHT7; break;
  }
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  ambient[0] = ambr; ambient[1] = ambg; ambient[2] = ambb; ambient[3] = amba;
  diffuse[0] = difr; diffuse[1] = difg; diffuse[2] = difb; diffuse[3] = difa;
  specular[0] = specr; specular[1] = specg; specular[2] = specb; specular[3] = speca;
  relx = X;
  rely = Y;
  relz = Z;
  o = obj;
  glLightfv (light,GL_AMBIENT,ambient);
  glLightfv (light,GL_DIFFUSE,diffuse);
  glLightfv (light,GL_SPECULAR,specular);
  glLightf (light,GL_CONSTANT_ATTENUATION,0.7);
  glLightf (light,GL_LINEAR_ATTENUATION,att);
  glLightf (light,GL_QUADRATIC_ATTENUATION,att/2);
  glLightf (light, GL_SPOT_CUTOFF, 180.0);
  on = 0;
  glDisable (light); // just to be sure
  hl = NULL;
  r = difr;
  g = difg;
  b = difb;
  a = difa;
}

alienlight::~alienlight ()
{

}

int alienlight::Typ ()
{
  return ALIENLIGHT;
}

// draws the building
void alienlight::draw ()
{
  GLfloat position[4];
  if (o!=NULL)
  {
    if (hl==NULL)
    {
      if (on)
      {
        glDisable (light);
        on = 0;
      }
      else
      {
        position[0]=x; position[1]=y; position[2]=z; position[3]=1.0;
        glLightfv (light, GL_POSITION, position);
        glEnable (light);
        on = 1;
      }
    }
    else
    {
      if (on)
      {
        glDisable (light);
        on = 0;
      }
      else
      {
        if (hl->getOn ())
        {
          GLfloat color[4];
          color[0]=hl->get_r (); color[1]=hl->get_g (); color[2]=hl->get_b (); color[3]=1.0;
          glLightfv (light,GL_DIFFUSE,color);
          glLightfv (light,GL_SPECULAR,color);
          position[0]=hl->getx (); position[1]=hl->gety (); position[2]=hl->getz (); position[3]=1.0;
          GLdouble* matl = hl->get_mat ();
          GLfloat direction[] = { -matl[8], -matl[9], -matl[10] };
          glLightfv (light, GL_POSITION, position);
          glLightfv (light, GL_SPOT_DIRECTION, direction);
          glEnable (light);
          on = 1;
        }
      }
    }
  }
}

// controls the building
void alienlight::control ()
{
  if (((mission*)getmission ())->get_objlist ()->in_list (o)==NULL)
  {
    o = NULL;
  }
  if (o!=NULL)
  {
    GLdouble* mat = ((model*)o)->getmat ();
    // calc new position
    x=mat[0]*relx+mat[4]*rely+mat[8]*relz+((model*)o)->getx ();
    y=mat[1]*relx+mat[5]*rely+mat[9]*relz+((model*)o)->gety ();
    z=mat[2]*relx+mat[6]*rely+mat[10]*relz+((model*)o)->getz ();
  }
}

void alienlight::bind (headlight *h)
{
  hl = h;
  // calculate angle
  GLfloat ang=atan2(hl->getCutoff (),hl->getLength ())*PI_GRAD;
  glLightf (light, GL_SPOT_CUTOFF, ang);
}

GLfloat alienlight::get_r ()
{
  return r;
}

GLfloat alienlight::get_g ()
{
  return g;
}

GLfloat alienlight::get_b ()
{
  return b;
}

// just for compatility reasons
void alienlight::set_weaponcount (int c) {}
void alienlight::add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void alienlight::add_object (int target,object* obj) {}
void alienlight::move () {}
void alienlight::clear_collision_info () {}

