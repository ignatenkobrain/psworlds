// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "utility.h"
#include "mission.h"

#define LOGFILE	"log.txt"


GLfloat fastdist2(GLfloat x, GLfloat y)
{
	// fast pythagoras approximation
	/*if(x<0) x=-x;
	if(y<0) y=-y;

	if(x>y)
		return (x+(y/4.0));
	else
		return (y+(x/4.0));*/
	return sqrt(x*x+y*y);
}


GLfloat fastdist3(GLfloat x, GLfloat y, GLfloat z)
{
	// the same, only for 3 dimensions
	/*GLfloat res;

	if(x<0) x=-x;
	if(y<0) y=-y;
	if(z<0) z=-z;

	if(x>y)
		res = (x+(y/4.0));
	else
		res = (y+(x/4.0));
	return fastdist2(res, z);*/
	return sqrt(x*x+y*y+z*z);
}

GLfloat distance(object* obj1, object* obj2)
{
	GLfloat x = obj1->getx()-obj2->getx();
	GLfloat y = obj1->gety()-obj2->gety();
	GLfloat z = obj1->getz()-obj2->getz();
	return fastdist3(x,y,z);
}

void normcrossprod(GLfloat *v1, GLfloat *v2, GLfloat *out)
{
	// calculate normalized cross-product of two vectors

	GLfloat d;
	GLfloat mult = 1.0;

	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];

	d = sqrt(out[0]*out[0]+out[1]*out[1]+out[2]*out[2]);

	// the y vector cannot be negative
	if(out[1]<0.0) mult=-1.0;

	out[0]/=(d*mult);
	out[1]/=(d*mult);
	out[2]/=(d*mult);
}

int distantvolume(GLfloat vx, GLfloat vy, GLfloat vz, GLfloat sx, GLfloat sy, GLfloat sz)
{
	// calculates volume for distant events, depending on source and viewpoint
	GLfloat xdst, ydst, zdst, dist;
	
	xdst = sx-vx; ydst = sy-vy; zdst = sz-vz;
	dist = ((int)(fastdist3(xdst, ydst, zdst))/35);
	if(dist==0) dist=1;
	return 255 / dist;
}

GLfloat calcdepth(GLfloat z)
{
	// calculates the depth in meters
	GLfloat dep;
	int base;
	
	base = ((mission*)getmission())->getworld()->getbase();
	dep = base-z*0.27;

	return dep;
}


GLfloat heading(GLfloat xd, GLfloat zd)
{
	// calculate the heading of a craft, depending on its movement vectors
	GLfloat hdng;
	GLfloat	len = fastdist2(xd,zd);
	xd/=len; zd/=len;
	hdng = atan2(-xd,zd);
	hdng*=PI_GRAD;
	hdng-=180.0;
	while(hdng<0) hdng+=360.0;
	return hdng;
}

void remove_underscore(char* s)
{
	for(unsigned int i=0;i<strlen(s);i++)
	{
		if(s[i]=='_') s[i]=' ';
	}
}

void write_log(char* s)
{
    FILE* file = fopen(LOGFILE,"a");
    fprintf(file,"%s",s);
    fclose(file);
}


void init_log()
{
	FILE* file = fopen(LOGFILE,"w");
	fprintf(file,"Logfile initialized\n\n");
	fclose(file);
}


void xpath(char *fname)
{
	// parse the filenames for win / un*x
	unsigned int i;

	for(i=0; i<strlen(fname); i++)
	{
		if((fname[i]=='/') && (SYSTEM == WINDOWS))
			fname[i] = 92;
	}
}



void makestring(char* &target, char* txt)
{
	// alloc space for a string and copy content
	target = (char*)malloc(strlen(txt)+1*sizeof(char));
	strcpy(target, txt);
}
