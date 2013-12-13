// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "trail.h"
#include "model.h"
#include "fonts.h"



trail::~trail()
{
	// foobar
}

trail::trail(float X,float Y,float Z,float rotX, float rotY, float rotZ, object* lead, GLuint state):object(X,Y,Z,0,0,0)
{
	// constructor
	start[0] = X;
	start[1] = Y;
	start[2] = Z;
	startheight = 0.0;

	leader = lead;
	this->state = state;
	this->initialstate = state;
	matrix = ((model*)leader)->getmat();

	if((state==TRAIL_FOLLOW) || (state==TRAIL_MISSILE))
	{
		// for dumb weapons, we need a copy of the matrix because we still need it
		// after the leader has been destroyed
		grabmatrix();
	}
	

	gotmatrix = 0; // TODO: hack! see header
	startalpha = 0.45;
	endalpha = 0.45;
}


void trail::grabmatrix()
{
	// make a copy of the current matrix
	fixmatrix[0] = matrix[0]; fixmatrix[4] = matrix[4]; fixmatrix[8] = matrix[8]; fixmatrix[12] = matrix[12];
	fixmatrix[1] = matrix[1]; fixmatrix[5] = matrix[5]; fixmatrix[9] = matrix[9]; fixmatrix[13] = matrix[13];
	fixmatrix[2] = matrix[2]; fixmatrix[6] = matrix[6]; fixmatrix[10] = matrix[10]; fixmatrix[14] = matrix[14];
	fixmatrix[3] = matrix[3]; fixmatrix[7] = matrix[7]; fixmatrix[11] = matrix[11]; fixmatrix[15] = matrix[15];
}



void trail::divorce()
{
	// release the trail from its leader because the leader probably died

	if(state!=TRAIL_FREE)
	{
		// a trail can only be divorced once, otherwise me might encounter "multiple dying"
		// which is unlikely, but would result in the trail fading away twice
		if(state==TRAIL_HOMING) {grabmatrix();gotmatrix=1;} // now that the homie´s dead, remain static
		
		this->time_to_live = 2.0;
		this->state = TRAIL_FREE;
		this->leader = NULL;
		this->movement_state=MV_DESTROY;
	}
}



void trail::move()
{
	// nothing to do here, the trail is moved in control()
}


void trail::control()
{
	// controls positioning of the trail
	GLfloat ticks = gettime();


	// in case this trail is leaderless
	if(movement_state==MV_DESTROY)
	{
		update_lifetime();
		startalpha-=ticks*TRAILFADE*0.5;
		endalpha-=ticks*TRAILFADE*0.5;
		startheight+=ticks*TRAILLIFT;
	}
	else
	{
		// follow the leader
		switch(state)
		{
		case TRAIL_FOLLOW: // for the common shot
			x = leader->getx();
			y = leader->gety();
			z = leader->getz();
			dist = fastdist3(x-start[0], y-start[1], z-start[2]);
			startalpha-=ticks*TRAILFADE;
			startheight+=ticks*TRAILLIFT;
			break;
		case TRAIL_MISSILE: // for all dumb missiles
			// dumb missiles and shots behave similarly, only their trail length
			// is limited because they move slower
			x = leader->getx();
			y = leader->gety();
			z = leader->getz();
			dist = fastdist3(x-start[0], y-start[1], z-start[2]);
			if(dist>TRAIL_MISSILE_LENGTH) dist = TRAIL_MISSILE_LENGTH;
			startalpha-=ticks*TRAILFADE;
			break;
		case TRAIL_HOMING: // for homing torpedoes
			// homies draft only a rather short trail
			x = leader->getx();
			y = leader->gety();
			z = leader->getz();
			dist = fastdist3(x-start[0], y-start[1], z-start[2]);
			if(dist>TRAIL_HOMIE_LENGTH) dist = TRAIL_HOMIE_LENGTH;
			startalpha-=ticks*TRAILFADE;
			break;
		}
	}




}


void trail::draw()
{
	// draw the trail down the negative z-axis, then rotate/translate into position
	GLfloat startcol[4] = { 0.6, 0.6, 1.0, startalpha };
	GLfloat endcol[4] = { 0.6, 0.6, 1.0, endalpha };
	GLfloat starttop = startheight*1.4;
	GLfloat zstart, zend;

	if(gotmatrix==0) { grabmatrix(); gotmatrix=1;} // TODO: HACK!! see header. well it works, but it suuucks
	
	glPushMatrix();
	
		// torpedoes have a finite trail length and depend on the endpoint
		if(initialstate==TRAIL_FOLLOW) 
		{
			glTranslatef(start[0], start[1], start[2]);
			zstart=0.0; zend = -dist;
		}
		else 
		{
			// addidtionally, they need to be drawn reversed because they are
			// oriented at the bullet itself. this is not important for multiplication,
			// so that´s why we need initialstate for translation
			glTranslatef(x,y,z);
			zstart=dist; zend = 0.0;
		}

		// homies might change orientation, so we need to calc with the updated matrix
		if(state!=TRAIL_HOMING)
			glMultMatrixd(fixmatrix);
		else
			glMultMatrixd(matrix);

		glBegin(GL_QUAD_STRIP);
			glColor4fv(startcol);
			glVertex3f(0,1+starttop,zstart);
			glColor4fv(endcol);
			glVertex3f(0,0.2,zend);
			glColor4fv(startcol);
			glVertex3f(1,startheight,zstart);
			glColor4fv(endcol);
			glVertex3f(0.2,0,zend);
			glColor4fv(startcol);
			glVertex3f(-1,startheight,zstart);
			glColor4fv(endcol);
			glVertex3f(-0.2,0,zend);
			glColor4fv(startcol);
			glVertex3f(0,1+starttop,zstart);
			glColor4fv(endcol);
			glVertex3f(0,0.2,zend);
		glEnd();
	glPopMatrix();

}



void trail::clear_collision_info()
{
	// just for compatibility reasons
	// nothing to do here, trails can't collide
}


void trail::set_weaponcount(int c)
{
	// just for compatibility reasons
}


void trail::add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{
	// just for compatibility reasons
}


void trail::add_object(int target,object* obj)
{
	// just for compatibility reasons
}

int trail::Typ() 
{
	return TRAIL;
}

