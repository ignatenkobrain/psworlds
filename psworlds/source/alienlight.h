// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __ALIENLIGHT_H_
#define __ALIENLIGHT_H_

#include "object.h"
#include "psworlds.h"
#include "headlight.h"

class alienlight : public object  
{
private:
	GLenum light;
	GLfloat relx;
	GLfloat rely;
	GLfloat relz;
	object* o;
	int on;
	headlight* hl;
	GLfloat r,g,b,a;
public:
	alienlight(int num, float X,float Y,float Z, float ambr, float ambg, float ambb, float amba, float difr, float difg, float difb, float difa, float specr, float specg, float specb, float speca, float att, object* obj);
	virtual ~alienlight();
	virtual int Typ();
	virtual void draw();
	virtual void control();
	virtual void set_weaponcount(int c);
	virtual void add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate);
	virtual void add_object(int target,object* obj);
	virtual void move();
	virtual void clear_collision_info();
	void bind(headlight* h);
	GLfloat get_r();
	GLfloat get_g();
	GLfloat get_b();
};

#endif
