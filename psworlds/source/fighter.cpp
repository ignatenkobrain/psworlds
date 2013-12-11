// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "fighter.h"
#include "direct.h"
#include "homing.h"
#include "mission.h"
#include "sound.h"
#include "torpedo.h"
#include "fonts.h"


fighter::fighter(char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
				 float Rotspd, float Maxspd, int Mass, int Energy,GLfloat *norms, int *sw):
					model(filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,0,norms,sw)
{
	targetlist = new objectlist();
	pathlist = new objectlist();
	target = NULL;
	waypt = NULL;
	weapons = NULL;
	active_weapon=0;
	weaponcount = 0;
	lastshot = 0;
}

// free resources
fighter::~fighter()
{
	if(weaponcount)
	{
		free(weapons);
	}
	targetlist->clear_all_entrys();
	pathlist->clear_all_entrys();
	delete targetlist;
	delete pathlist;
}

// sets the weapon count and initializes the weapon array
void fighter::set_weaponcount(int c)
{
	weaponcount = c;
	active_weapon = 0;
	if(c>0)
	{
		weapons = (weapon_t*)malloc(sizeof(weapon_t)*c);
	}
}

// adds a weapon to the weapon array
void fighter::add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)
{
	if(active_weapon<weaponcount)
	{
		weapons[active_weapon].x=sx;
		weapons[active_weapon].y=sy;
		weapons[active_weapon].z=sz;
		weapons[active_weapon].x2=sx2;
		weapons[active_weapon].y2=sy2;
		weapons[active_weapon].z2=sz2;
		weapons[active_weapon].time_to_live=time;
		weapons[active_weapon].power=power;
		weapons[active_weapon].amount=amount;
		weapons[active_weapon].filename=(char*)malloc(strlen(filename)+1);
		strcpy(weapons[active_weapon].filename,filename);
		weapons[active_weapon].name=(char*)malloc(strlen(name)+1);
        strcpy(weapons[active_weapon].name,name);
		weapons[active_weapon].mvspd=mvspd;
		weapons[active_weapon].rotspd=rotspd;
		weapons[active_weapon].type=type;
		weapons[active_weapon].doubleshot=dbl;
		weapons[active_weapon].rate=rate;
		active_weapon++;
	}
	if(active_weapon==weaponcount)
	{
		active_weapon=0;
	}
}

// fires missiles for all active wepaons
void fighter::shoot()
{
	GLfloat no[3] = {1.0, 1.0, 1.0};
	int sw[3] = {0, 1, 2};
	char* keys = getkeys();
	model* shot=NULL;
	int weapsam=3;
	if(active_weapon<weaponcount)
	{
		if(lastshot>1/weapons[active_weapon].rate)
		{
			if(weapons[active_weapon].amount>0||weapons[active_weapon].amount==-1)	// weapon available
			{
				float x,y,z;
				x = weapons[active_weapon].x;
				y = weapons[active_weapon].y;
				z = weapons[active_weapon].z;
				float sx=mat[0]*x+mat[4]*y+mat[8]*z+getx();
				float sy=mat[1]*x+mat[5]*y+mat[9]*z+gety();
				float sz=mat[2]*x+mat[6]*y+mat[10]*z+getz();
				if(weapons[active_weapon].type==DIRECT)
				{
					shot=new direct(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
				}
				else if(weapons[active_weapon].type==TORPEDO)
				{
					shot = new torpedo(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
					((torpedo*)shot)->setspeed(mvspd);
				}
				else
				{
					shot=new homing(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
					if(target!=NULL)
					{
						((homing*)shot)->set_target(target->obj);
					}
					else
					{
						((homing*)shot)->set_target(NULL);
					}
					((homing*)shot)->setspeed(mvspd);
				}
				shot->set_lifetime(weapons[active_weapon].time_to_live);
				shot->setmat(mat);
				shot->setinvmat(invmat);
				shot->add_collision_object(this);
				((mission*)getmission())->add_object(shot);
				if(weapons[active_weapon].doubleshot)
				{
					x = weapons[active_weapon].x2;
					y = weapons[active_weapon].y2;
					z = weapons[active_weapon].z2;
					float sx=mat[0]*x+mat[4]*y+mat[8]*z+getx();
					float sy=mat[1]*x+mat[5]*y+mat[9]*z+gety();
					float sz=mat[2]*x+mat[6]*y+mat[10]*z+getz();
					if(weapons[active_weapon].type==DIRECT)
					{
	   					shot=new direct(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
					}
					else if(weapons[active_weapon].type==TORPEDO)
					{
						shot = new torpedo(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
						((torpedo*)shot)->setspeed(mvspd);
					}
					else
					{
						shot=new homing(weapons[active_weapon].filename,sx,sy,sz,0,0,0,weapons[active_weapon].rotspd,weapons[active_weapon].mvspd,0,weapons[active_weapon].power,no,sw);
						if(target!=NULL)
						{
							((homing*)shot)->set_target(target->obj);
						}
						else
						{
							((homing*)shot)->set_target(NULL);
						}
						((homing*)shot)->setspeed(mvspd);
					}
					shot->set_lifetime(weapons[active_weapon].time_to_live);
					shot->setmat(mat);
					shot->setinvmat(invmat);
					shot->add_collision_object(this);
					((mission*)getmission())->add_object(shot);
				}
				// reduce weaponcount for
				if(weapons[active_weapon].amount>0)
				{
					weapons[active_weapon].amount--;
				}
			
				// play some sound
				Camera* cam = (Camera*) getcam();
				if(weapons[active_weapon].type==DIRECT)
					playsam(weapsam,15000,distantvolume(cam->getx(), cam->gety(), cam->getz(), sx, sy, sz)/3,100);
				else
				{
					playsam(25,22000,distantvolume(cam->getx(), cam->gety(), cam->getz(), sx, sy, sz)/1.5,100);
					playsam(26,22000,distantvolume(cam->getx(), cam->gety(), cam->getz(), sx, sy, sz)/1,100);
				}
				lastshot=0;
			}
			else
			{
				// play some sound to show that no weapon available
				Camera* cam = (Camera*) getcam();
				playsam(16,16000,distantvolume(cam->getx(), cam->gety(), cam->getz(), x, y, z)/3,200);
				keys[SDLK_SPACE]=0;
			}
		}
	}
}


// (de)activates weapon i
void fighter::switch_weapon()
{
	if(active_weapon<weaponcount-1)
	{
		active_weapon++;
	}
	else
	{
		active_weapon=0;
	}
}



listobj* fighter::getTarget()
{
	return target;
}


listobj* fighter::getWP()
{
	return waypt;
}


// adds an object to the targetlist or pathlist
void fighter::add_object(int target,object* obj)
{
	if(target)
	{
		targetlist->add_object(obj);
	}
	else
	{
		pathlist->add_object(obj);
	}
}

objectlist* fighter::get_pathlist()
{
	return pathlist;
}