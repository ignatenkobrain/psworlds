// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __UTILITY_H_
#define __UTILITY_H_

#include "psworlds.h"
#include "object.h"

// mathematics
#define PI 3.1415926535
#define GRAD_PI 0.017453292
#define PI_GRAD 57.29577951

GLfloat fastdist2 (GLfloat x, GLfloat y);
GLfloat fastdist3 (GLfloat x, GLfloat y, GLfloat z);
GLfloat distance (object* obj1, object* obj2);
int  distantvolume (GLfloat vx, GLfloat vy, GLfloat vz, GLfloat sx, GLfloat sy, GLfloat sz);
void normcrossprod (GLfloat *v1, GLfloat *v2, GLfloat *out);
GLfloat calcdepth (GLfloat z);
GLfloat heading (GLfloat xd, GLfloat zd);

// file access
#define _FBUF char buf[100];  // for easier file handling

void xpath (char *fname);
void remove_underscore (char* s);
void write_log (const char *s);
void init_log ();

// other
void errortest ();
void makestring (char *&target, const char *txt);


#endif
