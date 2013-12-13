// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "matrices.h"

// multiplies matrixes mat1 and mat2, puts result in tmp, optimized for rotations
void matrmult (GLdouble* mat1, GLdouble* mat2, GLdouble* tmp)
{
  tmp[0]=mat1[0]*mat2[0]+mat1[4]*mat2[1]+mat1[8]*mat2[2];
  tmp[1]=mat1[1]*mat2[0]+mat1[5]*mat2[1]+mat1[9]*mat2[2];
  tmp[2]=mat1[2]*mat2[0]+mat1[6]*mat2[1]+mat1[10]*mat2[2];
  tmp[4]=mat1[0]*mat2[4]+mat1[4]*mat2[5]+mat1[8]*mat2[6];
  tmp[5]=mat1[1]*mat2[4]+mat1[5]*mat2[5]+mat1[9]*mat2[6];
  tmp[6]=mat1[2]*mat2[4]+mat1[6]*mat2[5]+mat1[10]*mat2[6];
  tmp[8]=mat1[0]*mat2[8]+mat1[4]*mat2[9]+mat1[8]*mat2[10];
  tmp[9]=mat1[1]*mat2[8]+mat1[5]*mat2[9]+mat1[9]*mat2[10];
  tmp[10]=mat1[2]*mat2[8]+mat1[6]*mat2[9]+mat1[10]*mat2[10];
  tmp[3]=0;
  tmp[7]=0;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;
}


// multiplies matrixes mat1 and mat2, puts result in tmp, capable of translations
void matrmult2(GLdouble* m1, GLdouble* m2, GLdouble* tmp)
{
  tmp[0] = m1[0]*m2[0]+m1[4]*m2[1]+m1[8]*m2[2]+m1[12]*m2[3];
  tmp[1] = m1[1]*m2[0]+m1[5]*m2[1]+m1[9]*m2[2]+m1[13]*m2[3];
  tmp[2] = m1[2]*m2[0]+m1[6]*m2[1]+m1[10]*m2[2]+m1[14]*m2[3];
  tmp[3] = m1[3]*m2[0]+m1[7]*m2[1]+m1[11]*m2[2]+m1[15]*m2[3];
  
  tmp[4] = m1[0]*m2[4]+m1[4]*m2[5]+m1[8]*m2[6]+m1[12]*m2[7];
  tmp[5] = m1[1]*m2[4]+m1[5]*m2[5]+m1[9]*m2[6]+m1[13]*m2[7];
  tmp[6] = m1[2]*m2[4]+m1[6]*m2[5]+m1[10]*m2[6]+m1[14]*m2[7];
  tmp[7] = m1[3]*m2[4]+m1[7]*m2[5]+m1[11]*m2[6]+m1[15]*m2[7];

  tmp[8] = m1[0]*m2[8]+m1[4]*m2[9]+m1[8]*m2[10]+m1[12]*m2[11];
  tmp[9] = m1[1]*m2[8]+m1[5]*m2[9]+m1[9]*m2[10]+m1[13]*m2[11];
  tmp[10] = m1[2]*m2[8]+m1[6]*m2[9]+m1[10]*m2[10]+m1[14]*m2[11];
  tmp[11] = m1[3]*m2[8]+m1[7]*m2[9]+m1[11]*m2[10]+m1[15]*m2[11];

  tmp[12] = m1[0]*m2[12]+m1[4]*m2[13]+m1[8]*m2[14]+m1[12]*m2[15];
  tmp[13] = m1[1]*m2[12]+m1[5]*m2[13]+m1[9]*m2[14]+m1[13]*m2[15];
  tmp[14] = m1[2]*m2[12]+m1[6]*m2[13]+m1[10]*m2[14]+m1[14]*m2[15];
  tmp[15] = m1[3]*m2[12]+m1[7]*m2[13]+m1[11]*m2[14]+m1[15]*m2[15];
}


// rotation around x relative to the momentary rotation
void rotatex (GLdouble* mat, GLdouble* invmat, GLdouble rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
  GLdouble tmp[16],tmp1[16],tmp2[16];
  
  tmp[0]=1;
  tmp[1]=0;
  tmp[2]=0;
  tmp[3]=0;
  tmp[4]=0;
  tmp[5]=c;
  tmp[6]=s;
  tmp[7]=0;
  tmp[8]=0;
  tmp[9]=-s;
  tmp[10]=c;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;
  
  matrmult (mat,tmp,tmp1);

  tmp[0]=1;
  tmp[1]=0;
  tmp[2]=0;
  tmp[3]=0;
  tmp[4]=0;
  tmp[5]=c;
  tmp[6]=-s;
  tmp[7]=0;
  tmp[8]=0;
  tmp[9]=s;
  tmp[10]=c;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;

  matrmult (tmp,invmat,tmp2);

  for (int i=0;i<16;i++)
  {
    mat[i]=tmp1[i];
    invmat[i]=tmp2[i];
  }
}

// rotation around y relative to the momentary rotation
void rotatey (GLdouble* mat, GLdouble* invmat,GLdouble rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
  GLdouble tmp[16],tmp1[16],tmp2[16];

  tmp[0]=c;
  tmp[1]=0;
  tmp[2]=-s;
  tmp[3]=0;
  tmp[4]=0;
  tmp[5]=1;
  tmp[6]=0;
  tmp[7]=0;
  tmp[8]=s;
  tmp[9]=0;
  tmp[10]=c;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;

  matrmult (mat,tmp,tmp1);

  tmp[0]=c;
  tmp[1]=0;
  tmp[2]=s;
  tmp[3]=0;
  tmp[4]=0;
  tmp[5]=1;
  tmp[6]=0;
  tmp[7]=0;
  tmp[8]=-s;
  tmp[9]=0;
  tmp[10]=c;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;

  matrmult (tmp,invmat,tmp2);

  for (int i=0;i<16;i++)
  {
    mat[i]=tmp1[i];
    invmat[i]=tmp2[i];
  }
}

// rotation around z relative to the momentary rotation
void rotatez (GLdouble* mat, GLdouble* invmat,GLdouble rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
  GLdouble tmp[16],tmp1[16],tmp2[16];

  tmp[0]=c;
  tmp[1]=s;
  tmp[2]=0;
  tmp[3]=0;
  tmp[4]=-s;
  tmp[5]=c;
  tmp[6]=0;
  tmp[7]=0;
  tmp[8]=0;
  tmp[9]=0;
  tmp[10]=1;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;

  matrmult (mat,tmp,tmp1);

  tmp[0]=c;
  tmp[1]=-s;
  tmp[2]=0;
  tmp[3]=0;
  tmp[4]=s;
  tmp[5]=c;
  tmp[6]=0;
  tmp[7]=0;
  tmp[8]=0;
  tmp[9]=0;
  tmp[10]=1;
  tmp[11]=0;
  tmp[12]=0;
  tmp[13]=0;
  tmp[14]=0;
  tmp[15]=1;

  matrmult (tmp,invmat,tmp2);

  for (int i=0;i<16;i++)
  {
    mat[i]=tmp1[i];
    invmat[i]=tmp2[i];
  }
}

// set mat to the identity matrix
void set_identity (GLdouble* mat)
{
  int i;
  for (i=0;i<16;i++)
  {
    mat[i]=0;
  }
  mat[0]=1;
  mat[5]=1;
  mat[10]=1;
  mat[15]=1;
}


// multiply a point with a matrix
void mat_pt_multiply (GLdouble* mat, GLdouble* pt, GLdouble *res)
{
  res[0] = mat[0]*pt[0]+mat[4]*pt[1]+mat[8]*pt[2]+mat[12];
  res[1] = mat[1]*pt[0]+mat[5]*pt[1]+mat[9]*pt[2]+mat[13];
  res[2] = mat[2]*pt[0]+mat[6]*pt[1]+mat[10]*pt[2]+mat[14];
  res[3] = mat[3]*pt[0]+mat[7]*pt[1]+mat[11]*pt[2]+mat[15];
}


// set up the matrix for an x-rotation
void setup_mat_xrot (GLdouble *mat, GLfloat rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
    
  mat[0]=1; mat[1]=0;  mat[2]=0; mat[3]=0;
  mat[4]=0; mat[5]=c;  mat[6]=s; mat[7]=0;
  mat[8]=0; mat[9]=-s; mat[10]=c; mat[11]=0;
  mat[12]=0; mat[13]=0; mat[14]=0; mat[15]=1;
}


// set up the matrix for a y-rotation
void setup_mat_yrot (GLdouble *tmp, GLfloat rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
    
  tmp[0]=c; tmp[1]=0; tmp[2]=-s; tmp[3]=0;
  tmp[4]=0; tmp[5]=1; tmp[6]=0; tmp[7]=0;
  tmp[8]=s; tmp[9]=0; tmp[10]=c; tmp[11]=0;
  tmp[12]=0; tmp[13]=0; tmp[14]=0; tmp[15]=1;
}


// set up the matrix for a z-rotation
void setup_mat_zrot (GLdouble *tmp, GLfloat rot)
{
  GLdouble c=cos (rot*GRAD_PI);
  GLdouble s=sin (rot*GRAD_PI);
    
  tmp[0]=c; tmp[1]=s; tmp[2]=0; tmp[3]=0;
  tmp[4]=-s; tmp[5]=c; tmp[6]=0; tmp[7]=0;
  tmp[8]=0; tmp[9]=0; tmp[10]=1; tmp[11]=0;
  tmp[12]=0; tmp[13]=0; tmp[14]=0; tmp[15]=1;
}

