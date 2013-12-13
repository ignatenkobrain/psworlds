// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __BITMAP_H_
#define __BITMAP_H_

#define BMP_SMALLDUST 0
#define BMP_BIGDUST 1
#define BMP_EXPLO 2
#define BMP_BIGEXPLO 3
#define BMP_GUNHIT 4
#define BMP_GUNEXPLO 5


#include "object.h"
#include "tgaloader.h"

// used for explosions, displays a quad with changing textures


class bitmap : public object  
{
private:
  GLfloat w,h;  // width and height
  GLfloat wd, hd;  // width and height adjusters per second
  GLfloat yadd;    // bitmaps can do height travel
  GLfloat alpha, dalpha;  // alpha value and adjuster
  GLuint type;  // the BMP_xx type
  GLfloat lifespan;  // lifespan in seconds
  GLfloat duration;  // the time the bitmap has been active
  GLuint txtid;    // the id of the current texture in use

public:
  bitmap (GLfloat X,GLfloat Y,GLfloat Z,GLfloat rotX, GLfloat rotY, GLfloat rotZ, GLuint type, GLfloat time);
  virtual ~bitmap ();
  virtual int Typ ();
  virtual void draw ();  // draws the object
  virtual void control ();  // rotates bitmap, switches frames, fadeout, ...

  // just for compatility reasons
  virtual void move ();
  virtual void clear_collision_info ();
  virtual void set_weaponcount (int c);
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
  virtual void add_object (int target,object* obj);
};


// this function is called at startup and loads the necessary textures
void loadbmps ();



#endif
