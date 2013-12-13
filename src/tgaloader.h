// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __TGALOADER_H_
#define __TGALOADER_H_

#include "psworlds.h"

// texture object 

class textureObj
{
public:
	GLubyte *img;	// pixel data
	GLuint bpp;		// bits per pixel
	GLuint w, h;	// image width and height
	GLuint id;		// texture id for binding

	textureObj(int w, int h, int cc, GLubyte* dat, int makemipmap);
	textureObj(char *fname, int makemipmap);
	virtual ~textureObj();
	GLubyte *getPixels();
};


#endif

