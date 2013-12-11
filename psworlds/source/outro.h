// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __OUTRO_H_
#define __OUTRO_H_

#include "tgaloader.h"
#include "other.h"


class outro  
{
private:
	textureObj* bg[12];		// the background textures
	textureObj* layer[4];	// the color layers
	GLfloat alpha[5];		// alpha vals for each layer and the background
	GLfloat al1, al2;		// text alpha values
	GLfloat allal;			// alpha for all layers

	other* logo;
	other* logo2;			// logo models
	GLfloat lpos, lpos2;	// logo position
	GLfloat lspd;			// logo speed
	GLfloat bgal,bgx, bgy;	// background scaler
	GLfloat bgwait;

	int killme;
	int quit;
	GLfloat vol;			// mod volume
	GLfloat xsc[6], zsc[6];	// x and z-scale factors for layers and font
	GLfloat xd[4],yd[4],zd[4];	// movement speeds
	GLfloat rot[4];			// layer rotation
	GLfloat rsp[4];			// layer rotation speeds
	GLfloat lc[4][4];		// layer colors
	GLfloat ta[4];			// target alpha
	GLfloat lx[4], ly[4], lz[4];	// layer positions
	GLfloat ts[4];			// layer texture scale

	int step, numsteps;		// text steps
	GLfloat tm;				// time
	GLfloat prevtime;		// time in previous frame

	char* txt[40][15];		// message text for each step
	GLfloat ft[40];			// time for each frame

	void control();
	void draw();
	void initsound();

public:
	outro();
	virtual ~outro();

	void run();


};

#endif

