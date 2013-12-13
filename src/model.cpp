// POSSIBLE WORLDS
// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "model.h"
#include "psworlds.h"
#include "camera.h"
#include "world.h"
#include "sound.h"
#include "mission.h"
#include "fonts.h"
#include "displaylistlist.h"
#include "texturelist.h"
#include "headlight.h"
#include "matrices.h"



// constructor
// calls constructor of superclass to place and orientate object
// reads object information (vertices,...) from the file
model::model (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
       float Rotspd, float Maxspd, int Mass, int Energy, float scaler, GLfloat* norms,int *sw)
        :object (X,Y,Z,rotX,rotY,rotZ)
{
  int i;
  for (i=0; i<10; i++) lites[i]=NULL;
  for (i=0; i<3; i++) ninv[i]=norms[i];
  for (i=0; i<3; i++) nswap[i]=sw[i];
  lightcount=0;

  verts = NULL;
  faces = NULL;
  texVerts = NULL;
  texture = NULL;
  dllist* dlid = ((displaylistlist*)getdisplaylists ())->inlist (filename);
  if (dlid==NULL)
  {
    char* file = (char*)malloc (strlen (filename)+strlen ("objx/")+2);
    strcpy (file,"objx/");
    strcat (file,filename);
    readbin (file);
    create_textures ();
    create_displaylist ();
    ((displaylistlist*)getdisplaylists ())->add (filename,listindex,sphere_rad,minx,maxx,miny,maxy,minz,maxz);
  }
  else
  {
    listindex=dlid->ID;
    sphere_rad=dlid->sphere_rad;
    minx=dlid->minx;
    maxx=dlid->maxx;
    miny=dlid->miny;
    maxy=dlid->maxy;
    minz=dlid->minz;
    maxz=dlid->maxz;
  }
  collisionlist = new objectlist ();
  clear_collision_info ();
  rotspd=Rotspd;
  maxspd=Maxspd;
    mvspd=0;
  mass=Mass;
  setEnergy (Energy);
  if (scaler==0) scale = SCALEFACTOR; else scale = scaler;
  ndx=0;
  ndy=0;
  ndz=0;
  create_matrices ();
  all=NULL;
}


// destructor
model::~model ()
{
  freemem ();
}


void model::setnormals (GLfloat* norms)
{
  // adjust normals
  ninv[0] = norms[0];
  ninv[1] = norms[1];
  ninv[2] = norms[2];
}


// initializes the rotation matrix
void model::create_matrices ()
{
  for (int i=0;i<16;i++)
  {
    mat[i]=0;
    invmat[i]=0;
  }
  mat[0]=1;
  mat[5]=1;
  mat[10]=1;
  mat[15]=1;
  invmat[0]=1;
  invmat[5]=1;
  invmat[10]=1;
  invmat[15]=1;
  rotatex (mat,invmat,rotx);
  rotatey (mat,invmat,roty);
  rotatez (mat,invmat,rotz);
}


// read a binary int
void model::read_int (FILE* f,int32_t *value)
{
  fread (value,sizeof (int32_t),1,f);
}


// read a binary unsigned int
void model::read_uint (FILE* f,uint32_t *value)
{
  fread (value,sizeof (uint32_t),1,f);
}


// read a binary float
void model::read_float (FILE* f,float *value)
{
  fread (value,sizeof (float),1,f);
}


// read a binary char
void model::read_char (FILE* f,char *value)
{
  fread (value,sizeof (char),1,f);
}


// read data from the binary file
void model::readbin (char* filename)
{
  int i;
  uint32_t len;
  uint32_t j;
  FILE *o;
  
  xpath (filename);

  o = fopen (filename, "rb");

  // read materials
  read_int (o,&numMaterials);
  if (numMaterials != 0)
  {
    texture = (texture_t*)malloc (sizeof (texture_t)*numMaterials);
    for (i=0;i<numMaterials;i++)
    {
      read_float (o,&texture[i].ambient[0]);
      read_float (o,&texture[i].ambient[1]);
      read_float (o,&texture[i].ambient[2]);
      read_float (o,&texture[i].diffuse[0]);
      read_float (o,&texture[i].diffuse[1]);
      read_float (o,&texture[i].diffuse[2]);
      read_float (o,&texture[i].specular[0]);
      read_float (o,&texture[i].specular[1]);
      read_float (o,&texture[i].specular[2]);
      read_float (o,&texture[i].shine);
      texture[i].shine = fabs (100-texture[i].shine); // 3DS and openGL have different conceptions of shininess
      read_float (o,&texture[i].uTile);
      read_float (o,&texture[i].vTile);
      read_float (o,&texture[i].uOffset);
      read_float (o,&texture[i].vOffset);
      read_int (o,&texture[i].env);
      read_int (o,&texture[i].texmap);
      read_uint (o,&len);
      for (j=0;j<len;j++)
      {
        read_char (o,&texture[i].texName[j]);
      }
      texture[i].texName[len]='\0';
    }
  }

  // read vertices
  read_int (o,&numVertex);
  verts = (float (*)[3])malloc (sizeof (vec3_t)*numVertex);
  for (i=0;i<numVertex;i++)
  {
    read_float (o,&verts[i][0]);
    read_float (o,&verts[i][1]);
    read_float (o,&verts[i][2]);
  }

  // read tex vertices
  read_int (o,&numTexVertex);
  if (numTexVertex != 0)
  {
    texVerts = (float (*)[2])malloc (sizeof (vec2_t)*numTexVertex);
    for (i=0;i<numTexVertex;i++)
    {
      read_float (o,&texVerts[i][0]);
      read_float (o,&texVerts[i][1]);
    }
  }

  // read faces
  read_int (o,&numFaces);
  faces = (triangle_t*)malloc (sizeof (triangle_t)*numFaces);
  for (i=0;i<numFaces;i++)
  {
    read_int (o,&faces[i].material);
    read_int (o,&faces[i].vertIndex[0]);
    read_int (o,&faces[i].vertIndex[1]);
    read_int (o,&faces[i].vertIndex[2]);
    read_int (o,&faces[i].coordIndex[0]);
    read_int (o,&faces[i].coordIndex[1]);
    read_int (o,&faces[i].coordIndex[2]);
    read_float (o,&faces[i].normal[0]);
    read_float (o,&faces[i].normal[1]);
    read_float (o,&faces[i].normal[2]);
    read_float (o,&faces[i].vertNormals[0][0]);
    read_float (o,&faces[i].vertNormals[0][1]);
    read_float (o,&faces[i].vertNormals[0][2]);
    read_float (o,&faces[i].vertNormals[1][0]);
    read_float (o,&faces[i].vertNormals[1][1]);
    read_float (o,&faces[i].vertNormals[1][2]);
    read_float (o,&faces[i].vertNormals[2][0]);
    read_float (o,&faces[i].vertNormals[2][1]);
    read_float (o,&faces[i].vertNormals[2][2]);
    faces[i].normal[0]*=SCALEFACTOR;
    faces[i].normal[1]*=SCALEFACTOR;
    faces[i].normal[2]*=SCALEFACTOR;
    faces[i].vertNormals[0][0]*=SCALEFACTOR;
    faces[i].vertNormals[0][1]*=SCALEFACTOR;
    faces[i].vertNormals[0][2]*=SCALEFACTOR;
    faces[i].vertNormals[1][0]*=SCALEFACTOR;
    faces[i].vertNormals[1][1]*=SCALEFACTOR;
    faces[i].vertNormals[1][2]*=SCALEFACTOR;
    faces[i].vertNormals[2][0]*=SCALEFACTOR;
    faces[i].vertNormals[2][1]*=SCALEFACTOR;
    faces[i].vertNormals[2][2]*=SCALEFACTOR;
  }

  // read bounding sphere and bounding box values
  read_float (o,&sphere_rad);
  read_float (o,&minx);
  read_float (o,&maxx);
  read_float (o,&miny);
  read_float (o,&maxy);
  read_float (o,&minz);
  read_float (o,&maxz);

  // scale the factors to match the object size
  sphere_rad*=SCALEFACTOR;
  minx*=SCALEFACTOR;
  maxx*=SCALEFACTOR;
  miny*=SCALEFACTOR;
  maxy*=SCALEFACTOR;
  minz*=SCALEFACTOR;
  maxz*=SCALEFACTOR;

  fclose (o);
}


// releases all memory
void model::freemem ()
{

  if (verts != NULL) free (verts);
  if (faces != NULL) free (faces);
  if (texVerts != NULL) free (texVerts);
  if (texture != NULL) free (texture);
  delete collisionlist;

}

void model::create_textures ()
{
  textureObj* texobj=NULL;
  for (int i=0;i<numMaterials;i++)
  {
    if (texture[i].texmap)
    {
      // lookup name in texturelist
      texobj = ((texturelist*)gettexturelist ())->inlist (texture[i].texName);
      if (texobj != NULL)
      {
        // if name exist, take textureobject
        texture[i].texobj = texobj;
      }
      else
      {
        // otherwise create textureobject and add to list
        _FBUF
        sprintf (buf,"gfx/%s",texture[i].texName); texobj = new textureObj (buf,1);
        texture[i].texobj = texobj;
        ((texturelist*)gettexturelist ())->add (texture[i].texName,texobj);
      }
    }
  }
}


// creates the displaylist for the object
void model::create_displaylist ()
{
  listindex = glGenLists (1);
    glNewList (listindex, GL_COMPILE);
  glEnable (GL_COLOR_MATERIAL);
  for (int j=0;j<numMaterials;j++)
  {
    GLfloat ambient[]={texture[j].ambient[0],texture[j].ambient[1],texture[j].ambient[2],1.0};
    GLfloat diffuse[]={texture[j].diffuse[0],texture[j].diffuse[1],texture[j].diffuse[2],1.0};
    GLfloat specular[]={texture[j].specular[0],texture[j].specular[1],texture[j].specular[2],1.0};
    GLfloat shine[]={texture[j].shine};
    glColorMaterial (GL_FRONT,GL_AMBIENT);
    glColor4fv (ambient);
    glColorMaterial (GL_FRONT,GL_DIFFUSE);
    glColor4fv (diffuse);
    glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv (GL_FRONT, GL_SHININESS, shine);
    
    if (texture[j].texmap)
    {
      glBindTexture (GL_TEXTURE_2D, texture[j].texobj->id);
    }
    else
    {
      glBindTexture (GL_TEXTURE_2D, 0);
    }
    glBegin (GL_TRIANGLES);
    for (int i=0;i<numFaces;i++)
    {
      if (faces[i].material==j)
      {
        glNormal3f (faces[i].vertNormals[0][nswap[0]]*ninv[0],faces[i].vertNormals[0][nswap[1]]*ninv[1],faces[i].vertNormals[0][nswap[2]]*ninv[2]);
        glTexCoord2f (texVerts[faces[i].coordIndex[0]][0],texVerts[faces[i].coordIndex[0]][1]);
        glVertex3i (verts[faces[i].vertIndex[0]][0],verts[faces[i].vertIndex[0]][1],verts[faces[i].vertIndex[0]][2]);
        glNormal3f (faces[i].vertNormals[1][nswap[0]]*ninv[0],faces[i].vertNormals[1][nswap[1]]*ninv[1],faces[i].vertNormals[1][nswap[2]]*ninv[2]);
        glTexCoord2f (texVerts[faces[i].coordIndex[1]][0],texVerts[faces[i].coordIndex[1]][1]);
        glVertex3i (verts[faces[i].vertIndex[1]][0],verts[faces[i].vertIndex[1]][1],verts[faces[i].vertIndex[1]][2]);
        glNormal3f (faces[i].vertNormals[2][nswap[0]]*ninv[0],faces[i].vertNormals[2][nswap[1]]*ninv[1],faces[i].vertNormals[2][nswap[2]]*ninv[2]);
        glTexCoord2f (texVerts[faces[i].coordIndex[2]][0],texVerts[faces[i].coordIndex[2]][1]);
        glVertex3i (verts[faces[i].vertIndex[2]][0],verts[faces[i].vertIndex[2]][1],verts[faces[i].vertIndex[2]][2]);
      }
    }
    glEnd ();
  }
  glDisable (GL_COLOR_MATERIAL);
  glEndList ();
}


// draws the object
void model::draw ()
{
  glPushMatrix ();
  glTranslatef (x,y,z);
  glScalef (scale,scale,scale);
  glMultMatrixd (mat);
  glEnable (GL_TEXTURE_2D);
  glCallList (listindex);
  glDisable (GL_TEXTURE_2D);
  glPopMatrix ();
}


// returns the rotation matrix
GLdouble* model::getmat ()
{
  return mat;
}


// returns the inverse rotation matrix
GLdouble* model::getinvmat ()
{
  return invmat;
}


// the following procedures recalculate the matrices depending on movement
void model::left ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatey (mat,invmat,rotspeed);
  setroty (roty+rotspeed);
}
void model::right ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatey (mat,invmat,-rotspeed);
  setroty (roty-rotspeed);
}
void model::rollleft ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatez (mat,invmat,rotspeed);
  setrotz (rotz+rotspeed);
}
void model::rollright ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatez (mat,invmat,-rotspeed);
  setrotz (rotz-rotspeed);
}
void model::up ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatex (mat,invmat,-rotspeed);
  setrotx (rotx-rotspeed);
}
void model::down ()
{
  GLfloat ticks = gettime ();
  GLdouble rotspeed = rotspd * ticks;
  rotatex (mat,invmat,rotspeed);
  setrotx (rotx+rotspeed);
}
void model::inc_speed ()
{
  GLfloat ticks = gettime ();
  if (mvspd<maxspd) mvspd+=20*ticks;
}
void model::dec_speed ()
{
  GLfloat ticks = gettime ();
  if (mvspd>-maxspd) mvspd-=20*ticks;
}


// reacts on collisions with objects in objectlist
// needs to be overwritten for building/missile
void model::collision_handling ()
{
  GLfloat ticks = gettime ();
  if (collisionlist->get_first ()!=NULL)
  {
    listobj* tmpobj = collisionlist->get_first ();
    model* obj;
    GLfloat tmpx=0,tmpy=0,tmpz=0,d=1;
    GLfloat tmpdx=0,tmpdy=0,tmpdz=0;
    GLfloat m,m1=0;
    int n=0;
    while (tmpobj!=NULL)
    {
      obj = (model*)tmpobj->obj;
      if (obj->Typ ()!=HOMING&&obj->Typ ()!=DIRECT&&obj->Typ ()!=TORPEDO)
      {
        m=obj->getMass ();
        m=(mass>m&&m!=0)?(m/mass):1;
        d=distance (obj,this);
        tmpdx=(-obj->getx ()+this->getx ())/d;
        tmpdy=(-obj->gety ()+this->gety ())/d;
        tmpdz=(-obj->getz ()+this->getz ())/d;
        GLfloat dd=fabs (d/2-obj->bounding_rad ());
        if (dd>bounding_rad ()) dd=bounding_rad ();
        this->setx (this->getx ()+m*tmpdx*fabs (dd));
        this->sety (this->gety ()+m*tmpdy*fabs (dd));
        this->setz (this->getz ()+m*tmpdz*fabs (dd));
        tmpx+=m*(tmpdx)*ticks;
        tmpy+=m*(tmpdy)*ticks;
        tmpz+=m*(tmpdz)*ticks;
        n++;
        m1+=m;
      }
      else
      {
        // reduce energy only for objects which are destroyable
        if (destroyable)
        {
          // you can't be shot by missiles you just fired
          if (obj->getmvstate ()!=MV_START)
          {
            // signalise damage to cockpit
            if (Typ ()==PLAYER) ((mission*)getmission ())->getCockpit ()->hit ();
            energy-=obj->getEnergy ();
          }
        }
      }
      tmpobj=tmpobj->next;
    }
    if (n>0)
    {
      m1/=n;
      tmpx/=n;
      tmpy/=n;
      tmpz/=n;
      ndx=tmpx/2;
      ndy=tmpy/2;
      ndz=tmpz/2;
      if (mvspd<0)
      {
        ndx = -ndx;
        ndy = -ndy;
        ndz = -ndz;
      }
      mvspd/=2;
      if (movement_state!=MV_DESTROY)
      {
        movement_state=MV_COLLIDE;
      }
      // reduce energy only for objects which are destroyable
      if (destroyable)
      {
        // signalise damage to cockpit
        if (Typ ()==PLAYER) ((mission*)getmission ())->getCockpit ()->hit ();
        energy-=(1+abs (mvspd))*m1/5;

      }
    }
  }
}


// movement in MV_NORMAL
// needs to be overwritten for building/missile
void model::move_normal ()
{
  GLfloat tdx=dx;
  GLfloat tdy=dy;
  GLfloat tdz=dz;
  dx=-mat[8];
  dy=-mat[9];
  dz=-mat[10];
  GLfloat ticks = gettime ();

  ndx+=(tdx-dx)*mvspd/maxspd*20*ticks;
  ndy+=(tdy-dy)*mvspd/maxspd*20*ticks;
  ndz+=(tdz-dz)*mvspd/maxspd*20*ticks;
  x+=(dx+ndx)*mvspd*ticks;
  y+=(dy+ndy)*mvspd*ticks;
  z+=(dz+ndz)*mvspd*ticks;
  float fact=1-ticks*20*0.08;
  if (fact<=0) fact=0.01;
  ndx*=fact;
  ndy*=fact;
  ndz*=fact;
  if (fastdist3 (ndx,ndy,ndz)<0.05)
  {
    ndx=0;
    ndy=0;
    ndz=0;
  }
}


// movement in MV_COLLIDE
// needs to be overwritten for building/missile
void model::move_collide ()
{
  GLfloat tdx=dx;
  GLfloat tdy=dy;
  GLfloat tdz=dz;
  dx=-mat[8];
  dy=-mat[9];
  dz=-mat[10];
  GLfloat ticks = gettime ();
  GLfloat r = rand ()/(float)RAND_MAX;
  if (r<0.25)
  {
    left ();
  }
  else if (r<0.5)
  {
    right ();
  }
  else if (r<0.75)
  {
    up ();
  }
  else
  {
    down ();
  }
  x+=(dx+ndx)*mvspd*ticks;
  y+=(dy+ndy)*mvspd*ticks;
  z+=(dz+ndz)*mvspd*ticks;

  float fact=1-ticks*20*0.2;
  if (fact<=0) fact=0.01;
  ndx*=fact;
  ndy*=fact;
  ndz*=fact;

  if (fastdist3 (ndx,ndy,ndz)<0.05)
  {
    movement_state=MV_NORMAL;
    ndx=0;
    ndy=0;
    ndz=0;
  }
}


// moves the object
void model::move ()
{
  GLfloat dep;

  // do the world limit tests here
  worldlimits ();

  // now do collision detection with other objx and the seafloor
  collision_handling ();
  alt = sonar (minz);
  // calculate depth, pressure here
  dep = calcdepth (this->gety ());
  pressure = dep/10;
  depth = dep;

  ground_collision ();

  // if object is destroyed
  if (energy<=0&&destroyable)
  {
    //explode the object
    explode ();
  }
  switch (movement_state)
  {
    case MV_COLLIDE:
      move_collide ();
      break;
    default:
      move_normal ();
       break;
  }
}

// initiates an explosion for this object
void model::explode ()
{
  int explotype;

  // if the cam is bound to this object, retreat it!
  if (((Camera*)getcam ())->getActor () == this)
    ((Camera*)getcam ())->retreat ();

  if (movement_state!=MV_DESTROY)
  {
    // delete the headlights
    int i=0;
    while (i<lightcount)
    {
      if (lites[i]!=NULL)
      {
        ((headlight*)lites[i])->lights_out ();
        lites[i] = NULL;
      }
      i++;
    }

    movement_state=MV_DESTROY;
    mvspd=0;
    maxspd=0;
    rotspd=0;
    time_to_live=1.5;
    Camera* cam=(Camera*)getcam ();
    playsam (4,11000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z)*3,255);
    
    // create explosion bitmap, depending on mass
    if (mass<20&&mass!=0)
    {
      explotype = BMP_EXPLO;
    }
    else if (mass<80&&mass!=0)
    {
      explotype = BMP_BIGEXPLO;
    }
    else
    {
      // ok, for the tough ones! spawn multiple explosions >:-)
      explotype = BMP_BIGEXPLO;
      
      bitmap* bmp1 = new bitmap (x+(rand ()/(float)RAND_MAX*160-80),y+(rand ()/(float)RAND_MAX*70-35),z+(rand ()/(float)RAND_MAX*160-80),0,0,0,BMP_BIGEXPLO, -2.0+(rand ()/(float)RAND_MAX*2));
      bitmap* bmp2 = new bitmap (x+(rand ()/(float)RAND_MAX*160-80),y+(rand ()/(float)RAND_MAX*70-35),z+(rand ()/(float)RAND_MAX*160-80),0,0,0,BMP_BIGEXPLO, -2.0+(rand ()/(float)RAND_MAX*2));
      bitmap* bmp3 = new bitmap (x+(rand ()/(float)RAND_MAX*160-80),y+(rand ()/(float)RAND_MAX*70-35),z+(rand ()/(float)RAND_MAX*160-80),0,0,0,BMP_EXPLO, -2.0+(rand ()/(float)RAND_MAX*2));
      bitmap* bmp4 = new bitmap (x+(rand ()/(float)RAND_MAX*160-80),y+(rand ()/(float)RAND_MAX*70-35),z+(rand ()/(float)RAND_MAX*160-80),0,0,0,BMP_EXPLO, -2.0+(rand ()/(float)RAND_MAX*2));
    
      ((mission*)getmission ())->add_bmp (bmp1);
      ((mission*)getmission ())->add_bmp (bmp2);
      ((mission*)getmission ())->add_bmp (bmp3);
      ((mission*)getmission ())->add_bmp (bmp4);
    }

    bitmap* bmp = new bitmap (x,y,z,0,0,0,explotype, 0.0);
    ((mission*)getmission ())->add_bmp (bmp);
    ((mission*) getmission ())->get_bmplist ()->setsort (1);
  }
}

// returns the bounding sphere radius
vec_t model::bounding_rad ()
{
  return sphere_rad;
}


// clears all collision info
void model::clear_collision_info ()
{
  collisionlist->clear_all_entrys ();
  collision_soon_object=NULL;
  collision_soon_distance=1000;
}


// adds an object to the collision list
void model::add_collision_object (object* obj)
{
  if (collisionlist->in_list (obj)==NULL)
  {
    collisionlist->add_object (obj);
  }
}


// replaces the collision_soon_object if the new object is nearer
void model::add_collision_soon_object (object* obj, GLfloat d)
{
  if (d<collision_soon_distance)
  {
    collision_soon_object=obj;
    collision_soon_distance=d;
  }
}


// checks if a point is inside the bounding box of this object
int model::inside_bounding_box (float nx, float ny, float nz)
{
  GLfloat rx,ry,rz;
  nx-=x;
  ny-=y;
  nz-=z;
  rx = invmat[0]*nx+invmat[4]*ny+invmat[8]*nz;
  ry = invmat[1]*nx+invmat[5]*ny+invmat[9]*nz;
  rz = invmat[2]*nx+invmat[6]*ny+invmat[10]*nz;
  if (rx>=minx&&rx<=maxx&&ry>=miny&&ry<=maxy&&rz>=minz&&rz<=maxz)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


// return one coordinate of one corner of the bounding box
GLfloat model::bbc (int i, int coord)
{
  GLfloat cx,cy,cz,point;
  switch (i)
  {
  case  0: cx = minx; cy = miny; cz = minz; break;
  case  1: cx = minx; cy = miny; cz = maxz; break;
  case  2: cx = minx; cy = maxy; cz = minz; break;
  case  3: cx = minx; cy = maxy; cz = maxz; break;
  case  4: cx = maxx; cy = miny; cz = minz; break;
  case  5: cx = maxx; cy = miny; cz = maxz; break;
  case  6: cx = maxx; cy = maxy; cz = minz; break;
  case  7: cx = maxx; cy = maxy; cz = maxz; break;
  case  8: cx = minx; cy = 0;    cz = 0;    break;
  case  9: cx = maxx; cy = 0;    cz = 0;    break;
  case 10: cx = 0;    cy = miny; cz = 0;    break;
  case 11: cx = 0;    cy = maxy; cz = 0;    break;
  case 12: cx = 0;    cy = 0;    cz = minz; break;
  case 13: cx = 0;    cy = 0;    cz = maxz; break;
  }
  switch (coord)
  {
    case 0:
      point = mat[0]*cx+mat[4]*cy+mat[8]*cz;
      point+=x;
      break;
    case 1:
      point = mat[1]*cx+mat[5]*cy+mat[9]*cz;
      point+=y;
      break;
    case 2:
      point = mat[2]*cx+mat[6]*cy+mat[10]*cz;
      point+=z;
      break;
  }
  return point;
}


// returns mass of the object
int model::getMass ()
{
  return mass;  
}


// returns energy of the object
int model::getEnergy ()
{
  return energy;
}

int model::getFullNRG ()
{
  return fullnrg;
}

// returns speed of the object
GLfloat model::getSpeed ()
{
  return mvspd;
}



GLfloat model::sonar (GLfloat sz)
{
  // measure distance to ground level
  int inx, inz, quadx, quadz;
  GLfloat *v1, *v2, *v3, *verts, xdiff, zdiff;
  GLuint *index, *faces, ind1, ind2, ind3, xsize, zsize, poly;
  world* sea;

  // find position of sonar (front side)
  GLfloat xs=x+sz*mat[8];
  GLfloat ys=y+sz*mat[9];
  GLfloat zs=z+sz*mat[10];

  sea = (world*) getworld ();

  xsize = sea->getsizex ();
  zsize = sea->getsizez ();

  // determine in which triangle we are

  quadx = floor (x/MESHSIZE);
  quadz = floor (z/MESHSIZE);
  inx = xs-quadx*MESHSIZE;
  inz = zs-quadz*MESHSIZE;

  quadx<<=1;

  if (inz<inx) poly=1; else poly=0; // if point is below diagonal
  quadx+=poly;

  faces = sea->getfaces ();
  index = faces+(quadx*3+xsize*quadz);
  ind1 = index[0];
  ind2 = index[1];
  ind3 = index[2];

  verts = sea->getvertices ();
  v1 = verts+ind1*3;
  v2 = verts+ind2*3;
  v3 = verts+ind3*3;

  if (poly==0) // left triang
  {
    zdiff = (v2[1]-v1[1])/MESHSIZE;
    xdiff = (v3[1]-v2[1])/MESHSIZE;
  }
  else  // right triang
  {
    zdiff = (v2[1]-v3[1])/MESHSIZE;
    xdiff = (v3[1]-v1[1])/MESHSIZE;
  }
  return ys-(v1[1]+inz*zdiff+inx*xdiff); // calc height
}


void model::ground_collision ()
{
  // checks for collision with the seafloor
  // we usually have the model coordinate origin at the center of gravity, so minimum
  // altitude should be 5 or so
  Camera* cam;
  int samnum;


  if ((alt<5) && (movement_state!=MV_DESTROY))
  {
    // crash!
    x = prevx;
    y = prevy;
    z = prevz;
    y+=2.0;

    cam = (Camera*) getcam ();
    if ((rand () & 0x01) == 0) samnum=2; else samnum=6; // randomize the collision sound
    playsam (samnum,12000,distantvolume (cam->getx (), cam->gety (), cam->getz (), x, y, z),199);
    
    // let's shake
    if (movement_state!=MV_DESTROY)
    {
      movement_state=MV_COLLIDE;
    }

    ndx=-dx*2*mvspd/fabs (mvspd);
    ndy=-dy*2*mvspd/fabs (mvspd);
    ndz=-dz*2*mvspd/fabs (mvspd);
    mvspd/=2;

    // reduce energy
    if (destroyable)
    {
      energy-=abs (mvspd)/10;
      // signalise damage to cockpit
      if (Typ ()==PLAYER) ((mission*)getmission ())->getCockpit ()->hit ();
    }
    
    // spawn dustclouds
    bitmap* bmp = new bitmap (x,y,z,0,0,0,BMP_BIGDUST, 0.0);
    ((mission*)getmission ())->add_bmp (bmp);
    ((mission*) getmission ())->get_bmplist ()->setsort (1);

  }
  prevx = x;
  prevy = y;
  prevz = z;
}

// sets the rotation matrix
void model::setmat (GLdouble* nmat)
{
  for (int i=0;i<16;i++)
  {
    mat[i]=nmat[i];
  }
}

// sets the inverse rotation matrix
void model::setinvmat (GLdouble* nmat)
{
  for (int i=0;i<16;i++)
  {
    invmat[i]=nmat[i];    
  }
}


// return pressure, alt or depth
GLuint model::getAlt ()
{
  return alt;
}

GLuint model::getDepth ()
{
  return depth;
}

GLuint model::getPressure ()
{
  return pressure;
}

GLfloat model::getCompression ()
{
  return compression;
}


void model::worldlimits ()
{
  // make sure the object does not leave the map or dives too deep or too low
  // the limits are stored in the world object

  GLfloat xmax, zmax, maxdep, mindep, sealev;
  world* thesea;

  // fill vars
  thesea = (world*)(((mission*)getmission ())->getworld ());
  xmax = thesea->getxwidth ();
  zmax = thesea->getzwidth ();
  maxdep = thesea->getmaxdepth ();
  mindep = thesea->getmindepth ();
  sealev = thesea->getlevel ();

  // now do the checks
  if ((this->Typ ()==DIRECT) || (this->Typ ()==HOMING) || (this->Typ ()==TORPEDO))
  {
    if ((x<WEAPONBORDER) || (x>xmax-WEAPONBORDER) || (z<WEAPONBORDER) || (z>zmax-WEAPONBORDER) || (y>sealev))
    {
      movement_state = MV_COLLIDE; time_to_live=0.0;
    }
  }
  else
  {
    if (x<BORDER) x=BORDER;
    if (z<BORDER) z=BORDER;
    if (x>xmax-BORDER) x=xmax-BORDER;
    if (z>zmax-BORDER) z=zmax-BORDER;

    compression = 0.0;
    if (y<maxdep) compression = fabs (maxdep-y); // set compression var to depth difference
    if (y>mindep) compression = -fabs (mindep-y); // detto

    if (y>sealev-sphere_rad) y = sealev-sphere_rad; // no swimming above the surface ;)
  }
}

void model::set_enemystatus (int value)
{
  enemystatus=value;
}

int model::get_enemystatus ()
{
  return enemystatus;
}

int model::get_destroyable ()
{
  return destroyable;
}

// sets mass of object
void model::setMass (int value)
{
  mass = value;
}

// sets energy and FullNRG
void model::setEnergy (int value)
{
  energy=value;
  if (energy==0)
  {
    destroyable=0;
  }
  else
  {
    destroyable=1;
  }
  fullnrg = value;
}

// sets maxspd
void model::setMaxspd (float value)
{
  maxspd=value;
}

// sets rotspd
void model::setRotspd (float value)
{
  rotspd=value;
}


void model::add_light (void* lite)
{
  // add a light to the model
  lites[lightcount] = lite;
  lightcount++;
}

void model::bind_alienlight (alienlight* al)
{
  if (lites[0]!=NULL)
  {
    headlight* lite = (headlight*)lites[0];
    al->bind (lite);
    all=al;
    lite->set_r (al->get_r ());
    lite->set_g (al->get_g ());
    lite->set_b (al->get_b ());
  }
}

int model::getDisplaylist ()
{
  return listindex;
}

GLfloat model::getScale ()
{
  return scale;
}