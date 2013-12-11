// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "bitmap.h"
#include "camera.h"

// the array of textures
textureObj *bmps[15];
GLuint bmpids[15];


// constructor
bitmap::bitmap(float X,float Y,float Z,float rotX, float rotY, float rotZ, GLuint type, GLfloat time):object(X,Y,Z,rotX,rotY,rotZ)
{
	// depending on type, select various sizes etc for the bitmap
	this->type = type;
	
	if(time>0.0) time = 0.0;

	duration = time;
	switch(type)
	{
	case BMP_SMALLDUST:
		// a small dust fountain, quickly becoming oblique, then gradually fading away
		txtid = bmpids[0];
		w = 1.0; h = 1.0; wd=8.0; hd=8.0;
		alpha = 0.5; dalpha = 10.0;
		lifespan=2.2; yadd=1.0;
		break;
	case BMP_BIGDUST:
		// larger dust fountain, i.e. if a ship crashes
		txtid = bmpids[7];
		w = 3.2; h = 3.2; wd = 24.0; hd = 24.0;
		alpha = 0.5; dalpha = 10.0;
		lifespan = 2.8; yadd=-1.0;
		break;
	case BMP_EXPLO:
		// a medium explosion, for a small ship like a fighter
		txtid = bmpids[1];
		w=18.0; h=18.0; wd=68.0; hd=68.0;
		alpha=1.0; dalpha=0.0;
		lifespan = 5.2; yadd=0.0;
		break;
	case BMP_BIGEXPLO:
		// the very big one. BLAAAAAAST!!!!!
		txtid = bmpids[8];
		w = 33.0; h = 33.0; wd = 133.0; hd =133.0;
		alpha = 0.6; dalpha = 2.0;
		lifespan = 7.0; yadd = 0.0;
		break;
	case BMP_GUNHIT:
		// whenever a normal shot hits a model
		txtid = bmpids[5];
		w = 1.6; h=1.6; wd=0.7; hd=0.7;
		alpha=0.8; dalpha=0.0;
		lifespan = 1.2; yadd = 3.0;
		break;
	case BMP_GUNEXPLO:
		// when a missile hits something
		txtid = bmpids[1];
		w = 9.5; h=9.5; wd=0.4; hd=0.4;
		alpha = 0.8; dalpha=0.0;
		lifespan = 2.0; yadd = 3.0;
		break;
	default: lifespan=0; duration=1.0; break; // if type is undefined, the bitmap will be killed immediately
	}
}


// destructor
bitmap::~bitmap()
{
	// foobar
}


int bitmap::Typ()
{
	return BITMAP;
}



void bitmap::draw()
{
	// rotate towards camera and draw the bitmap
	
	if(duration>=0.0)
	{
		GLuint camstate;
		
		Camera* cam = (Camera*) getcam();
		camstate = cam->getCamstate();

		glBindTexture(GL_TEXTURE_2D, txtid);
		glColor4f(0.7,0.7,1.0,alpha);

		// rotate the bitmap so that it´s always facing us
		glPushMatrix();
			glTranslatef(x,y,z);
			
			if(camstate==CAM_COCKPIT||camstate==CAM_BEHIND||camstate==CAM_FLYBY)
				glMultMatrixd(cam->getInvmat());
			else
			{
				glRotatef(cam->getyrot(), 0.0, 1.0, 0.0);
				glRotatef(cam->getzrot(), 0.0, 0.0, 1.0);
				glRotatef(cam->getxrot(), 1.0, 0.0, 0.0);
			}
			
			glBegin(GL_QUADS);
			glTexCoord2i(0,1);
			glVertex3f(-w,h,0.0);
			glTexCoord2i(0,0);
			glVertex3f(-w,-h,0.0);
			glTexCoord2i(1,0);
			glVertex3f(w,-h,0.0);
			glTexCoord2i(1,1);
			glVertex3f(w,h,0.0);
			glEnd();
		glPopMatrix();

	}
}

void bitmap::move()
{
	// nothing to do here
}


void bitmap::control()
{
	// control the bitmap parameters depending on their type	
	GLfloat ticks = gettime();
	duration+=ticks;
	
	if(duration>=0.0) // if the delay is over, start the animation
	{
		// evolve!
		w+=wd*ticks; h+=hd*ticks;
		alpha+=dalpha*ticks;
		y+=yadd*ticks;

		// kill bitmap if time´s up
		if(duration>lifespan) destroy=1;
		else
		{
		
			switch(type)
			{
			case BMP_SMALLDUST:
				if(duration<0.3) // STEP 1: initial phase, enlarge the dustcloud
				{
					if(w>4.5) w=4.5;
					if(h>4.5) h=4.5;
					if(alpha>1.0) alpha=1.0;
				}
				else if(duration<lifespan) // STEP 2: gradually fade out
				{
					wd = 2.6; hd = 2.6; dalpha = -0.6;
					if(alpha>1.0) alpha=1.0;
					if(alpha<0.0) alpha=0.0;
				}
				break;
			case BMP_BIGDUST:
				if(duration<0.3) // STEP 1: quickly enlarge
				{
					if(w>10.0) w=10.0;
					if(h>10.0) h=10.0;
					if(alpha>1.0) alpha=1.0;
				}
				else if(duration<lifespan) // STEP2: fade away..
				{
					wd = 4.0; hd = 4.0; dalpha = -0.4;
					if(alpha>1.0) alpha=1.0;
					if(alpha<0.0) alpha=0.0;
				}
				break;
			case BMP_EXPLO:
				if(duration<0.25) // STEP 1: initial, short bright flash
				{
					if(w>35.0) w=35.0;
					if(h>35.0) h=35.0;
				}
				else if(duration<0.50) // STEP 2: longer flash of drowning explosion
				{
					txtid=bmpids[2];
					w=35.0; h=35.0; wd=0.0; hd=0.0;
				}
				else if(duration<2.9) // STEP 3: evovling debris and bubbles
				{
					txtid=bmpids[3];
					wd=4.0; hd=5.0; dalpha=-0.31;
				}
				else if(duration<=lifespan) // STEP 4: fading away
				{
					txtid=bmpids[4];
					wd=3.0; hd=3.5; dalpha= -0.1;
				}
				break;
			case BMP_BIGEXPLO:
				if(duration<0.3) // STEP 1: explosion begins to evolve
				{
					if(w>73.0) w=73.0;
					if(h>73.0) h = 73.0;
					if(alpha>1.0) alpha=1.0;
				}
				else if(duration<0.8) // STEP 2: explosion full blast
				{
					txtid = bmpids[9];
					wd = 70.0; hd = 70.0;
					dalpha = 0.0;
				}
				else if(duration<1.2) // STEP 3: explosion becoming convex
				{
					txtid = bmpids[10];
					wd = 30.0; hd = 30.0;
					dalpha = 0.0;
				}
				else if(duration<2.2) // STEP 4: overconvex, beginning to fade
				{
					txtid = bmpids[11];
					wd = 12.0; hd = 12.0;
					dalpha = -0.20;
				}
				else if(duration<lifespan) // STEP 5: drowning
				{
					txtid = bmpids[12];
					wd = 4.0; hd = 4.0;
					dalpha = -0.17;
				}
				break;
			case BMP_GUNHIT:
				if(duration<0.1) // STEP 1: initial tiny explosion
				{
					// do nothing special
				}
				else if(duration<lifespan) // STEP 2: some bubbly stuff
				{
					txtid = bmpids[6];
					dalpha = -0.81;
				}
				break;
			case BMP_GUNEXPLO:
				if(duration<0.13) // STEP 1: explosion
				{
					// foobar
				}
				else if(duration<lifespan) // STEP 2: fade out bubbles
				{
					txtid = bmpids[4];
					dalpha = -0.8;
				}
				break;
			}
		}
	}
}

// just for compatility reasons
void bitmap::clear_collision_info() {}
void bitmap::set_weaponcount(int c) {}
void bitmap::add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void bitmap::add_object(int target,object* obj) {}



void loadbmps()
{
	// load bitmap textures
	
	_FBUF

    write_log("Loading bitmap textures\n");

	sprintf(buf, "gfx/smalldust1.tga");
	bmps[0] = new textureObj(buf,1); bmpids[0] = bmps[0]->id;
	
	sprintf(buf, "gfx/explo1.tga");
	bmps[1] = new textureObj(buf,1); bmpids[1] = bmps[1]->id;

	sprintf(buf, "gfx/explo2.tga");
	bmps[2] = new textureObj(buf,1); bmpids[2] = bmps[2]->id;

	sprintf(buf, "gfx/explo3.tga");
	bmps[3] = new textureObj(buf,1); bmpids[3] = bmps[3]->id;

	sprintf(buf, "gfx/explo4.tga");
	bmps[4] = new textureObj(buf,1); bmpids[4] = bmps[4]->id;

	sprintf(buf, "gfx/gunhit1.tga");
	bmps[5] = new textureObj(buf,1); bmpids[5] = bmps[5]->id;

	sprintf(buf, "gfx/gunhit2.tga");
	bmps[6] = new textureObj(buf,1); bmpids[6] = bmps[6]->id;

	sprintf(buf, "gfx/bigdust1.tga");
	bmps[7] = new textureObj(buf,1); bmpids[7] = bmps[7]->id;

	sprintf(buf, "gfx/bigexplo1.tga");
	bmps[8] = new textureObj(buf,1); bmpids[8] = bmps[8]->id;

	sprintf(buf, "gfx/bigexplo2.tga");
	bmps[9] = new textureObj(buf,1); bmpids[9] = bmps[9]->id;

	sprintf(buf, "gfx/bigexplo3.tga");
	bmps[10] = new textureObj(buf,1); bmpids[10] = bmps[10]->id;

	sprintf(buf, "gfx/bigexplo4.tga");
	bmps[11] = new textureObj(buf,1); bmpids[11] = bmps[11]->id;

	sprintf(buf, "gfx/bigexplo5.tga");
	bmps[12] = new textureObj(buf,1); bmpids[12] = bmps[12]->id;
	

    write_log("Loading bitmap textures complete!\n");
}

