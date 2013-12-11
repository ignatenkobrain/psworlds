// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __COMPUTER_H_
#define __COMPUTER_H_

#include "fighter.h"
#include "objectlist.h"


// the class representing computer fighters
class computer : public fighter  
{
private:
	int retreat;		// is computer currently retreating after attack?
	int r_left;			// -1 left, 0 straight, 1 right while retreating
	int r_up;			// -1 up, 0 straigt, 1 down while retreating
	float retreat_time;	// amount of time computer is already retreating	
	void cleanup_targetlist();	// delete all entrys in targetlist from destroyed objects
public:
	computer(char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
		     float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw);
	virtual ~computer();
	virtual int Typ();
	virtual void draw();
	virtual void control();	// "AI"
};

#endif

