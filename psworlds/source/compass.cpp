// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "compass.h"
#include "camera.h"

compass::compass(char *fname, player* pl, GLfloat* norms,int *sw):model(fname, -0.385, -0.6, -1.6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0,0.007,norms,sw)
{
	write_log("Generated artificial horizon\n");
	// set player object
	owner = pl;
}

compass::~compass()
{

}



void compass::control()
{
	// orient the compass appropriately
	double* plmat = ((Camera*)getcam())->getMat();

	// make a copy of the current matrix
	mat[0] = plmat[0]; mat[4] = plmat[4]; mat[8] = plmat[8]; mat[12] = plmat[12];
	mat[1] = plmat[1]; mat[5] = plmat[5]; mat[9] = plmat[9]; mat[13] = plmat[13];
	mat[2] = plmat[2]; mat[6] = plmat[6]; mat[10] = plmat[10]; mat[14] = plmat[14];
	mat[3] = plmat[3]; mat[7] = plmat[7]; mat[11] = plmat[11]; mat[15] = plmat[15];
}

int compass::Typ()
{
	return COMPASS;
}

// compatibility stuff yadda yadda blah
void compass::set_weaponcount(int c) {}
void compass::add_weapon(char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate) {}
void compass::add_object(int target,object* obj) {}

