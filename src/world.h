// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//



#ifndef __WORLD_H_
#define __WORLD_H_

#include "psworlds.h"
#include "tgaloader.h"


#define MESHSIZE 40.0	// define the size of seafloor quads
#define BORDER 750.0	// define the border size of the world
#define WEAPONBORDER 100.0 // define border for weapons

class world
{

private:
	GLfloat *vertices;	// mapping V->XYZ	[][]
	GLuint *faces;		// mapping F->012	[][]
	GLfloat *normals;	// mapping N->XYZ	[][]
	GLfloat *texcoords;	// mapping F->0->XY 1->XY 2->XY	[][][]
	GLubyte *minimap;	// the miniature seafloor map
	GLuint haze;		// determines the size of the map sector we want to draw
	GLuint sizex, sizez;	// size of arrays
	GLfloat litepos[4];	// light source position
	int drawmode;		// array drawing or manual drawing
	textureObj*	texture;// the seafloor texture
	textureObj* water1;	// water texture one
	textureObj* water2; // second water texture
	GLuint base;		// the depth in meters of the lowest part of the map
	GLfloat xwidth, zwidth; // maximum x and z positions for models
	GLuint xquads, zquads;	// amount of quads, x and z
	GLfloat mindepth, maxdepth; // minimum / maximum allowed depth to dive
	GLfloat sealev;			// the height of the sealevel
	GLfloat wavecount, wave;// counter responsible for swapping water textures
	GLfloat waveheight, wavespeed;	// speed and height of the waves

	int generateworld(int xsize, int ysize, float scale, unsigned char *img);
	

public:
	world(char *fname, float scale, float sealevel, float maxdepth, float mindepth, float wavheight, float wavspd);
	virtual ~world();

	void setHaze(GLuint distance);	// set haze
	GLuint getHaze();				// get haze

	int draw(float x, float y, float z, float xrot, float yrot, float zrot, int pass);
	int getdrawmode();
	void setdrawmode(int mode);

	void makeminimap(GLubyte* data);	// generate a small overview world map for the cockpit

	void adjustlight();		// set fog, clear color and light according to camera pos and world attributes

	// providers
	int getsizex();
	int getsizez();

	GLfloat getxwidth();
	GLfloat getzwidth();

	GLfloat getmaxdepth();
	GLfloat getmindepth();
	GLfloat getlevel();

	GLfloat* getvertices();
	GLuint* getfaces();
	GLuint getbase();

	GLubyte* getminimap();
};



#endif

