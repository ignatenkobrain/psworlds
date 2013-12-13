// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __MODEL_H_
#define __MODEL_H_


#include <stdio.h>
#include "object.h"
#include "objectlist.h"
#include "tgaloader.h"
#include "headlight.h"
#include "alienlight.h"

// enemystatus
#define NEUTRAL    0
#define FRIEND    1
#define ENEMY    2

#define SCALEFACTOR 0.1

typedef struct
{
  char* filename;    // objectfile of missile
  char* name;      // name of missile (for display in cockpit)
  float x,y,z;    // 1st startpoint of missile
  float x2,y2,z2;    // 2nd startpoint of missile
  float time_to_live;  // existence of shot in seconds
  int power;      // energy reduced from hit objects
  int amount;      // amount of objects available
  float mvspd;    // speed of missile
  float rotspd;    // rotspd of missiles (homing)
  float rate;      // shots/second
  int doubleshot;    // 0 ... one or 1 ... two shots
  int type;      // DIRECT or HOMING
} weapon_t;



typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];

// a triangle in the model
typedef struct
{
  int vertIndex[3];    // indices for the verts that make up this triangle
  int coordIndex[3];    // indices for the tex coords to texture this face
  vec3_t vertNormals[3];  // vertex normals
  vec3_t normal;      // face normal
  int material;      // index for the material of this face
} triangle_t;


// subtexture information for the model
typedef struct
{
  char texName[255];  // texture name
  int texmap;      // is there a texture?
  int env;      // environment mapping?
  vec_t uTile;    // u tiling of texture
  vec_t vTile;    // v tiling of texture
  vec_t uOffset;    // u offset of texture
  vec_t vOffset;    // v offset of texture
  vec3_t ambient;    // ambient values
  vec3_t diffuse;    // diffuse values
  vec3_t specular;  // specular values
  vec_t shine;    // shine values
  textureObj* texobj;  // corresponding texture object
} texture_t;

class model : public object
{
protected:
  GLfloat prevx, prevy, prevz;  // coordinates in previous frame
  int numVertex;    // number of verts in the model
  int numFaces;    // number of faces in the model
  int numTexVertex;  // number of texture coords
  int numMaterials;  // number of materials
  vec3_t *verts;    // model verts
  vec2_t *texVerts;  // texture coords
  triangle_t *faces;  // faces of model
  texture_t *texture;  // texture/material information
  GLfloat alt;    // distance to seafloor
  GLuint depth;    // depth in meters
  GLuint pressure;  // pressure that inflicts on the model
  GLfloat compression;  // if we dive too low or too deep, we encounter
              // compression (+) or decompression (-) which damages the hull. AI should avoid these regions
  GLfloat scale;    // allows the missionfile or whatever to further adjust the scale parameter
  GLfloat ninv[3];  // normal inverter
  int nswap[3];      // normal index resorter

  int lightcount;    // the number of headlights this model posesses
  void* lites[10];  // an array of headlights
  alienlight* all;

  void freemem ();              // releases all memory
  void readbin (char* filename);      // read data from the binary file
  void read_int (FILE* f,int32_t *value);    // read a binary int
  void read_uint (FILE* f,uint32_t *value);  // read a binary unsigned int
  void read_float (FILE* f,float *value);  // read a binary float
  void read_char (FILE* f,char *value);  // read a binary char

  void create_displaylist ();  // creates the OPENGL displaylist for this model
  void create_textures ();    // creates the textureobjects for this model
  int listindex;        // index of the displaylist
  
  GLdouble mat[16];      // rotation matrix
  GLdouble invmat[16];    // inverse rotation matrix

  int mass;      // mass of object (for reaction at collision, 0 ... fixed object)
  int energy;      // energy of object
  int fullnrg;    // the initial energy of the object
  GLfloat rotspd;    // rotation/sec
  GLfloat mvspd;    // units/sec
  GLfloat maxspd;    // maximal speed in units/sec
  int enemystatus;  // friend or enemy ?
  int destroyable;  // is the object destroyable ?

  objectlist* collisionlist;      // all objects with which this object collides
  object* collision_soon_object;    // the next object in sight
  GLfloat collision_soon_distance;  // distance to this object

  vec_t sphere_rad;  // radius for bounding sphere
  vec_t minx;      // values for bounding box
  vec_t maxx;      // ...
  vec_t miny;      // ...
  vec_t maxy;      // ...
  vec_t minz;      // ...
  vec_t maxz;      // ...

  vec_t ndx;  // movement during collision, drifting
  vec_t ndy;  // ...
  vec_t ndz;  // ...

  virtual void left ();    // turns the object left
  virtual void right ();    // turns the object right
  virtual void up ();      // turns the object up
  virtual void down ();    // turns the object down
  virtual void rollleft ();  // rolls the object left
  virtual void rollright ();  // rolls the object right
  virtual void inc_speed ();  // increases speed
  virtual void dec_speed ();  // decreases speed

  // the following procedures include standards for flying objects
  // they have to be overwritten for buildings and turrets

  virtual void collision_handling ();  // checks all objects in collision list and reacts correspondingly
  virtual void move_normal ();      // movement in MV_NORMAL
  virtual void move_collide ();    // movement in MV_COLLIDE
  virtual void ground_collision ();  // handles collision with seafloor
  virtual void explode ();        // initiates an explosion for this object

public:

  // constructor, reads object information from binary file
  model (char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
          float Rotspd, float Maxspd, int Mass, int Energy, float scaler, GLfloat* norms,int *sw);

  virtual ~model ();    // destructor
  virtual void draw ();  // draws the object
  virtual void move ();  // moves the object depending on the rotationmatrix and mvspd

  // clears all collision information
  virtual void clear_collision_info ();

  // creates the rotation matrix for this model
  void create_matrices ();

  // adds an object to the collisionlist if it isn't already there
  void add_collision_object (object* obj);

  // replaces the collision soon object if it is nearer than the one set
  void add_collision_soon_object (object* obj, GLfloat d);

  // checks if a point is inside the bounding box this objec
  int inside_bounding_box (float nx, float ny, float nz);

  // distance to ground
  GLfloat sonar (GLfloat z);

  // don´t leave the world limits
  void worldlimits ();

  // return one corner of the bounding box
  GLfloat bbc (int i, int coord);

  vec_t bounding_rad ();  // returns the bounding sphere radius
  GLdouble* getmat ();    // returns the rotation matrix
  GLdouble* getinvmat ();  // returns the inverse rotation matrix

  void setnormals (GLfloat* norms);
  void setmat (GLdouble* nmat);  // sets the rotation matrix
  void setinvmat (GLdouble* nmat);  // sets the inverse rotation matrix

  void setMaxspd (float value);  // sets maxspd
  void setRotspd (float value);  // sets rotspd
  int getMass ();        // returns mass of the object
  void setMass (int value);  // sets mass of object
  void setEnergy (int value);  // sets energy and FullNRG
  int getEnergy ();      // returns energy of the object
  int getFullNRG ();      // return initial energy
  int get_destroyable ();
  GLfloat getSpeed ();  // returns speed of the object
  GLuint getAlt ();  // return altitude
  GLuint getDepth ();  // return depth in meters
  GLuint getPressure ();  // returns pressure in atmospheres (bar)
  GLfloat getCompression ();  // returns compression value

  virtual void set_enemystatus (int value);
  int get_enemystatus ();

  void add_light (void* lite);

  void bind_alienlight (alienlight* al);

  int getDisplaylist ();

  GLfloat getScale ();
};

#endif

