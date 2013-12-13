// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __MATRICES_H_
#define __MATRICES_H_

#include "psworlds.h"

// matrix operations optimized for rotations
// they can't be used for translations, ... !!

// multiplies matrixes mat1 and mat2, puts result in tmp
// optimized for rotation
void matrmult (GLdouble* mat1, GLdouble* mat2, GLdouble* tmp);
// capable of translation
void matrmult2(GLdouble* m1, GLdouble* m2, GLdouble* tmp);



// rotations around x,y,z respectively, relativ to the momentary rotation
void rotatex (GLdouble* mat, GLdouble* invmat, GLdouble rot);
void rotatey (GLdouble* mat, GLdouble* invmat,GLdouble rot);
void rotatez (GLdouble* mat, GLdouble* invmat,GLdouble rot);

// set mat to the identity matrix
void set_identity (GLdouble* mat);

// multiply a point
void mat_pt_multiply (GLdouble* mat, GLdouble* pt, GLdouble *res);

// setup rotation matrices
void setup_mat_xrot (GLdouble *mat, GLfloat rot);
void setup_mat_yrot (GLdouble *tmp, GLfloat rot);
void setup_mat_zrot (GLdouble *tmp, GLfloat rot);

#endif

