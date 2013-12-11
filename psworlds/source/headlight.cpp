// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "headlight.h"
#include "matrices.h"
#include "model.h"
#include "fonts.h"
#include "camera.h"
#include "tgaloader.h"


GLfloat xang[14], yang[14];
GLdouble mmat[16];
GLdouble projection[16];
GLint viewport[4];

flarelist* flist = NULL;
flarelist* flarestart = NULL;

textureObj* flare;



void loadflaregfx()
{
	// load flare TGA
	_FBUF
	sprintf(buf,"gfx/lensflare.tga");
	flare = new textureObj(buf,1);
}


void pregetProjections()
{
	// viewport and projection matrix need to be grabbed only once a frame
	glGetDoublev(GL_PROJECTION_MATRIX,projection);
	glGetIntegerv(GL_VIEWPORT,viewport);
}


flarelist::flarelist(GLdouble fx, GLdouble fy, GLdouble fz, GLfloat dist, GLfloat strength)
{
	// calc the predicted screen position
	GLdouble sx, sy, sz;
	
	gluProject(0.0,0.0,-FLAREORIGIN, mmat, projection, viewport, &sx, &sy, &sz);
	x = sx; y = sy;
	
	if(visible())
	{
		visi=1;
		// calc world coordinates
		GLdouble pinky[3];
		pinky[0]=0.0; pinky[1]=0.0; pinky[2]=-FLAREORIGIN;
		mat_pt_multiply(mmat,pinky,wc);
	}
	else visi=0;

	// calculate size
	alpha = strength;
	if(dist<1.5) dist=1.5;
	size = 25*(300.0/(dist/3.2));

	next = NULL;

}


flarelist::~flarelist()
{
	// foobar
}


void flarelist::add(flarelist* nu)
{
	next = nu;
}


int flarelist::visible()
{
	// check if the corresponding headlight origin is visible
	int res = 0;
	
	if(x>0 && x<getxres() && y>0 && y<getyres())
	{
		res = 1;
	}


	return res;
}


void flarelist::draw()
{
	// just a basic bitmap overlay of the flare image
	if(visi)
	{
		GLfloat pxs = 800.0/getxres();
		GLfloat pys = 600.0/getyres();
		// draw flare!
		glBegin(GL_QUADS);
		glColor4f(1.0,1.0,1.0,alpha+0.22);
		glTexCoord2i(0,1);
		glVertex3f((-size+x)*pxs*0.0025-1.0,(-size+y)*pys*0.0027-0.8,-1.5000001);
		glTexCoord2i(0,0);
		glVertex3f((-size+x)*pxs*0.0025-1.0,(size+y)*pys*0.0027-0.8,-1.5000001);
		glTexCoord2i(1,0);
		glVertex3f((size+x)*pxs*0.0025-1.0,(size+y)*pys*0.0027-0.8,-1.5000001);
		glTexCoord2i(1,1);
		glVertex3f((size+x)*pxs*0.0025-1.0,(-size+y)*pys*0.0027-0.8,-1.5000001);
		glEnd();
	}
}


void flarelist::prepare()
{
	// flare preparation
	if(visi)
	{
		// draw yellow dot
		glVertex3f(wc[0],wc[1],wc[2]);
	}
}



void flarelist::finalcheck()
{
	if(visi)
	{
		_FBUF
		GLubyte px[27];
		int drawme=0;
		glReadPixels(x-1,y-1,3,3,GL_RGB,GL_UNSIGNED_BYTE,&px);
		sprintf(buf,"R:%d G:%d B:%d\n AT: X:%d Y:%d",px[0], px[1], px[2], (int)x, (int)y);
		//print(100,300,buf,1,1.0,1.0,1.0,1.0);
		//write_log(buf);
		visi=0;
		for(int i=0; i<9; i++)
		{
			if(px[i*3]==255 && px[i*3+1]==255 && px[i*3+2]==0)
				visi=1;
		}
	}
}



// flare-wrappers

void addflare(GLfloat fx, GLfloat fy, GLfloat fz, GLfloat strength)
{
	// add a new flare object to the list
	// first, calc distance
	flarelist* nu;
	Camera* cam = (Camera*)getcam();
	GLfloat dist = fastdist3(cam->getx()-fx, cam->gety()-fy, cam->getz()-fz);

	if((dist<1000.0) && (mmat[14]<0)) // add if distance is near and object in front of cam
	{
		nu = new flarelist(mmat[12], mmat[13], mmat[14], dist, strength);
		
		// if this is the first flare, set the listfirst pointer
		if(flist == NULL)
		{
			flarestart = nu;
			flist = flarestart;
		}
		else
		{
			// else just append this one
			flist->add(nu);
			flist = flist->next;
		}
	}
}



void drawflares()
{
	// draw all lens flares
	flarelist *dummy;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glBindTexture(GL_TEXTURE_2D, flare->id);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	
	// run through the list of strings
	while(flarestart!=NULL)
	{		
		flarestart->draw();
		dummy = flarestart->next;
		delete (flarestart);	// clean up immediately
		flarestart = dummy;
	}


	// for the next frame
	flarestart = NULL;
	flist = NULL;

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}


void prepareflares()
{
	// prepare flares: draw yellow points wherever there큦 a light source
	flarelist* temp=flarestart;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glColor4f(1.0,1.0,0.0,1.0);
	glBegin(GL_POINTS);
	while(temp!=NULL)
	{
		temp->prepare();
		temp = temp->next;
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FOG);
	glEnable(GL_LIGHTING);
}


void finalizeflares()
{
	// prepare flares: draw yellow points wherever there큦 a light source
	flarelist* temp=flarestart;

	while(temp!=NULL)
	{
		temp->finalcheck();
		temp = temp->next;
	}
}




headlight::~headlight()
{
	// foobar
}


headlight::headlight(float X,float Y,float Z,float rotX, float rotY, float rotZ, void* owner, float coff, float len, float intense, int spec):object(X,Y,Z,rotX,rotY,rotZ)
{
	// construct headlight and calc direction matrix
	
	for(int i=0;i<16;i++)
	{
		dir[i]=0;
		invdir[i]=0;
	}
	dir[0]=1;
	dir[5]=1;
	dir[10]=1;
	dir[15]=1;
	invdir[0]=1;
	invdir[5]=1;
	invdir[10]=1;
	invdir[15]=1;
	rotatex(dir,invdir,rotx);
	rotatey(dir,invdir,roty);
	rotatez(dir,invdir,rotz);

	host = (model*)owner;

	cutoff = coff;
	length = len;
	intensity = intense;
	cintensity = intensity;
	on=LIGHT_ON;
	// player큦 light is always off at first
	if(((model*)host)->Typ()==PLAYER) {on = LIGHT_OFF; cintensity=0.0; }

	bulb = spec;

	lx = x; ly = y; lz = z;

	r = 0.4;
	g = 0.7;
	b = 1.0;
}


void headlight::control()
{
	// update lite큦 position and angle
	if(host!=NULL)
	{
		// grab hosts matrix
		GLdouble* mat = ((model*)host)->getmat();

		// calc new position
		x=mat[0]*lx+mat[4]*ly+mat[8]*lz+((model*)host)->getx();
		y=mat[1]*lx+mat[5]*ly+mat[9]*lz+((model*)host)->gety();
		z=mat[2]*lx+mat[6]*ly+mat[10]*lz+((model*)host)->getz();

		// calc new angle
		matrmult(mat, dir, ldir);

		// fade out if light is switched off
		if((on==LIGHT_OFF) && (cintensity>0.0))
		{
			GLfloat ticks = gettime();
			cintensity-=ticks*LIGHT_FADE;
		}

	}
}


void headlight::move()
{

}

void headlight::draw()
{
	// draw the thing!
	GLfloat cx, cy;
	
	if(cintensity>0.0)
	{
		glPushMatrix();
		glTranslatef(x,y,z);
		glMultMatrixd(ldir);
		glColor4f(1.0, 1.0, 1.0, 0.6);
	
		glGetDoublev(GL_MODELVIEW_MATRIX,mmat);
		
		// if necessary, draw a flare as well
		if(bulb == HL_FLARE || bulb == HL_BEAMANDFLARE)
			addflare(x, y, z-FLAREORIGIN, cintensity);
		
		// if necessary, draw the beam
		if(bulb == HL_BEAM || bulb == HL_BEAMANDFLARE)
		{
			glBegin(GL_TRIANGLE_FAN);
				glColor4f(0.74*r, 0.74*g, 0.74*b, cintensity-0.1);
				glVertex3f(0.0,0.0,0.0);
				glColor4f(r, g, b, 0.0);
				// draw the cone bottom
				for(int i=0; i<13; i++)
				{
					cx = xang[i]*cutoff;
					cy = yang[i]*cutoff;
					glVertex3f(cx,cy,-length);
				}
			glEnd();
		}
		glPopMatrix();
	}
}


void headlight::lights_out()
{
	// mark the headlight for destruction, most likely because the host was destroyed
	this->destroy=1;
	this->host=NULL;
}


int headlight::Typ()
{
	return HEADLIGHT;
}


void headlight::toggle()
{
	// switch lite on/off
	if(this->on==LIGHT_ON)
	{
		on = LIGHT_OFF; cintensity = intensity;
	}
	else 
	{	
		on = LIGHT_ON; cintensity = intensity;
	}
}


void headlight::up()
{
	// pitch up
	GLfloat ticks = gettime();
	GLdouble rotspeed = -LITE_ROTSPD * ticks;
	rotatex(this->dir,this->invdir,rotspeed);
	setrotx(rotx+rotspeed);
	if(rotx+rotspeed<290.0 && rotx+rotspeed>180.0) 
	{ 
		rotspeed = 290.0-rotx; rotatex(this->dir,this->invdir,rotspeed);
		setrotx(rotx+rotspeed);
	}
}


void headlight::down()
{
	// pitch down
	GLfloat ticks = gettime();
	GLdouble rotspeed = LITE_ROTSPD * ticks;
	rotatex(this->dir,this->invdir,rotspeed);
	setrotx(rotx+rotspeed);
	if(rotx+rotspeed>70.0 && rotx+rotspeed<180.0) 
	{ 
		rotspeed = 70.0-rotx; rotatex(this->dir,this->invdir,rotspeed);
		setrotx(rotx+rotspeed);
	}
}

void headlight::left()
{
	// swerve left
	GLfloat ticks = gettime();
	GLdouble rotspeed = LITE_ROTSPD * ticks;
	rotatey(this->dir,this->invdir,rotspeed);
	setroty(roty+rotspeed);
	if(roty+rotspeed>70.0 && roty+rotspeed<180.0) 
	{ 
		rotspeed = 70.0-roty; rotatey(this->dir,this->invdir,rotspeed);
		setroty(roty+rotspeed);
	}
}


void headlight::right()
{
	// swerve right
	GLfloat ticks = gettime();
	GLdouble rotspeed = -LITE_ROTSPD * ticks;
	rotatey(this->dir,this->invdir,rotspeed);
	setroty(roty+rotspeed);
	if(roty+rotspeed<290.0 && roty+rotspeed>180.0) 
	{ 
		rotspeed = 290.0-roty; rotatey(this->dir,this->invdir,rotspeed);
		setroty(roty+rotspeed);
	}
}

GLdouble* headlight::get_mat()
{
	return ldir;
}

int headlight::getOn()
{
	return on;
}

GLfloat headlight::getCutoff()
{
	return cutoff;
}

GLfloat headlight::getLength()
{
	return length;
}

// compatibility methods
void headlight::add_object(int target, object *obj) {}
void headlight::add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void headlight::set_weaponcount(int c) {}
void headlight::clear_collision_info() {}


void precalc_angles()
{
	// precalc the cone angle cornerpoints
	int j=0;
	
	for(int i=0; i<=360; i+=30)
	{
		xang[j] = cos(i*GRAD_PI);
		yang[j] = sin(i*GRAD_PI);
		j++;
	}
}

void headlight::change_r(GLfloat dr)
{
	r+=dr;
	if(r>1) r=1;
	if(r<0) r=0;
}

void headlight::change_g(GLfloat dg)
{
	g+=dg;
	if(g>1) g=1;
	if(g<0) g=0;
}

void headlight::change_b(GLfloat db)
{
	b+=db;
	if(b>1) b=1;
	if(b<0) b=0;
}

void headlight::set_r(GLfloat nr)
{
	r = nr;
	if(r>1) r=1;
	if(r<0) r=0;
}

void headlight::set_g(GLfloat ng)
{
	g = ng;
	if(g>1) g=1;
	if(g<0) g=0;
}

void headlight::set_b(GLfloat nb)
{
	b = nb;
	if(b>1) b=1;
	if(b<0) b=0;
}


GLfloat headlight::get_r()
{
	return r;
}

GLfloat headlight::get_g()
{
	return g;
}

GLfloat headlight::get_b()
{
	return b;
}