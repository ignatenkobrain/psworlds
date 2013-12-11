// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __PLAYER_H_
#define __PLAYER_H_


#include "fighter.h"
#include "psworlds.h"

// this class represents the vessel controlled by the player
class player : public fighter  
{
private:
	int bcrack[3];
	int targetmode;
public:
	player(char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
		   float Rotspd, float Maxspd, int Mass, int Energy, GLfloat *norms,int *sw);
	virtual ~player();
	virtual int Typ();
	virtual void draw();
	virtual void control();
	virtual void explode();
	void clear_target();
	void findnext_target();
	void check_target();
	weapon_t* getweapons();
	int getweapcount();
	int getactiveweapon();
	void* getwaypoint();
};

#endif
