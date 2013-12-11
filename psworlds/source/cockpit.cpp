// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "cockpit.h"
#include "fonts.h"
#include "model.h"
#include "player.h"
#include "mission.h"
#include "sound.h"
#include "matrices.h"


Cockpit::~Cockpit()
{
	// cleanup a bit
	int i;
	
	write_log("Cleaning up Cockpit..");
	
	for(i=0; i<3; i++)
	{
		if(cracks[i]!=NULL)
		{
			delete cracks[i]->x;
			delete cracks[i]->y;
			delete cracks[i]->alpha;
			crack* tmp = cracks[i];
			delete tmp;
		}
	}
	
	delete left;
	delete mid;
	delete rite;
	delete strip;

	delete dleft;
	delete dmid;
	delete drite;
	delete dradar;
	
	delete env;

	delete minimap;

	glDeleteLists(panels,1);
	glDeleteLists(leftdis,1);
	glDeleteLists(middis,1);
	glDeleteLists(ritedis,1);
	glDeleteLists(radardis,1);
	glDeleteLists(xhair,1);

	write_log("done!\n");
}


Cockpit::Cockpit(int type, player* plr, void* mis)
{
	// construct the cockpit for the given type and create a display list for it
	_FBUF

	write_log("Setting up Cockpit..\n");

	pongmode=0;
	// first set the display states
	display[0] = CP_MAP;
	display[1] = CP_INFO;
	display[2] = CP_RADAR;
	alpha = 0.28;

	setPlayer(plr);

	// set cockpit modulate colors
	cpcol[0] = 0.8; cpcol[1] = 0.8; cpcol[2] = 0.8;

	// intialize cracks
	cracks[0] = NULL;
	cracks[1] = NULL;
	cracks[2] = NULL;

	// set the radar zoom state and position
	radarxzoom = 1500.0; radaryzoom = radarxzoom*0.8667;
	radarxpos = 0.0025*259; radarypos = -0.8+0.0027*101;
	radarxextent = 90*0.0025; radaryextent = 85*0.0025; radaraltextent = 22*0.0027;
	sonarpingtime = SONARPINGDELAY;

	// set minimap zoom
	mapzoom = 2.0;

	// load the textures
    write_log("Loading Cockpit textures.\n");
	sprintf(buf, "gfx/cockpit%dleft.tga",type); left = new textureObj(buf, 1);
	sprintf(buf, "gfx/cockpit%dmid.tga",type); mid = new textureObj(buf, 1);
	sprintf(buf, "gfx/cockpit%drite.tga",type); rite = new textureObj(buf, 1);
	sprintf(buf, "gfx/cockpit%dstrip.tga",type); strip = new textureObj(buf, 1);

	sprintf(buf, "gfx/display%dleft.tga",type); dleft = new textureObj(buf,1);
	sprintf(buf, "gfx/display%dmid.tga",type); dmid = new textureObj(buf,1);
	sprintf(buf, "gfx/display%drite.tga",type); drite = new textureObj(buf,1);
	sprintf(buf, "gfx/display%dradar.tga",type); dradar = new textureObj(buf,1);
	
	sprintf(buf, "gfx/environment.tga"); env = new textureObj(buf,1);
	

	// build minimap
	minimap = new textureObj(256,256,3,((mission*)mis)->getworld()->getminimap(),1);

	// load the compass objects
	GLfloat no[3]; no[0]=1; no[1]=1; no[2]=1;
	int sw[3]; sw[0]=0; sw[1]=1; sw[2]=2;
	sprintf(buf, "compass.obj"); horizon = new compass(buf, plr, no,sw);

	// now create the display list
	write_log("Generating display lists..");
	panels = glGenLists(1);

	glNewList(panels, GL_COMPILE);
	
	// left part
	glBindTexture(GL_TEXTURE_2D, left->id);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-1.0, -0.8+0.0027*256, -1.5000001);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-1.0, -0.8, -1.5000001);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-1.0+0.0025*256, -0.8, -1.5000001);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-1.0+0.0025*256, -0.8+0.0027*256, -1.5000001);
	glEnd();

	// middle part
	glBindTexture(GL_TEXTURE_2D, mid->id);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-1.0+0.0025*256, -0.8+0.0027*256, -1.5000001);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-1.0+0.0025*256, -0.8, -1.5000001);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-1.0+0.0025*512, -0.8, -1.5000001);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-1.0+0.0025*512, -0.8+0.0027*256, -1.5000001);
	glEnd();

	// right part
	glBindTexture(GL_TEXTURE_2D, rite->id);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-1.0+0.0025*512, -0.8+0.0027*256, -1.5000001);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-1.0+0.0025*512, -0.8, -1.5000001);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-1.0+0.0025*768, -0.8, -1.5000001);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-1.0+0.0025*768, -0.8+0.0027*256, -1.5000001);
	glEnd();

	// strip
	glBindTexture(GL_TEXTURE_2D, strip->id);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-1.0+0.0025*768, -0.8+0.0027*256, -1.5000001);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-1.0+0.0025*768, -0.8, -1.5000001);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-1.0+0.0025*800, -0.8, -1.5000001);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-1.0+0.0025*800, -0.8+0.0027*256, -1.5000001);
	glEnd();

	glEndList();


	leftdis = glGenLists(1);
	middis = glGenLists(1);
	ritedis = glGenLists(1);
	radardis = glGenLists(1);
	
	// display list for left display
	glNewList(leftdis, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, dleft->id);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3f(-1.0+22*0.0025, -0.8+0.0027*(256-60), -1.5000001);
		glTexCoord2i(0,0);
		glVertex3f(-1.0+22*0.0025, -0.8+0.0027*(256-229), -1.5000001);
		glTexCoord2i(1,0);
		glVertex3f(-1.0+222*0.0025, -0.8+0.0027*(256-229), -1.5000001);
		glTexCoord2i(1,1);
		glVertex3f(-1.0+222*0.0025, -0.8+0.0027*(256-60), -1.5000001);
	glEnd();
	glEndList();

	// middle display
	glNewList(middis, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, dmid->id);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3f(-1.0+(30+256)*0.0025, -0.8+0.0027*(256-18), -1.5000001);
		glTexCoord2i(0,0);
		glVertex3f(-1.0+(30+256)*0.0025, -0.8+0.0027*(256-204), -1.5000001);
		glTexCoord2i(1,0);
		glVertex3f(-1.0+(256+246)*0.0025, -0.8+0.0027*(256-204), -1.5000001);
		glTexCoord2i(1,1);
		glVertex3f(-1.0+(256+246)*0.0025, -0.8+0.0027*(256-18), -1.5000001);
	glEnd();
	glEndList();

	// rite display
	glNewList(ritedis, GL_COMPILE);
		
	glBindTexture(GL_TEXTURE_2D, drite->id);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3f(-1.0+(55+512)*0.0025, -0.8+0.0027*(256-63), -1.5000001);
		glTexCoord2i(0,0);
		glVertex3f(-1.0+(55+512)*0.0025, -0.8+0.0027*(256-242), -1.5000001);
		glTexCoord2i(1,0);
		glVertex3f(-1.0+(512+235)*0.0025, -0.8+0.0027*(256-242), -1.5000001);
		glTexCoord2i(1,1);
		glVertex3f(-1.0+(512+235)*0.0025, -0.8+0.0027*(256-63), -1.5000001);
	glEnd();
	glEndList();

	// radar display
	glNewList(radardis, GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, dradar->id);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3f(-1.0+(55+512)*0.0025, -0.8+0.0027*(256-63), -1.5000001);
		glTexCoord2i(0,0);
		glVertex3f(-1.0+(55+512)*0.0025, -0.8+0.0027*(256-242), -1.5000001);
		glTexCoord2i(1,0);
		glVertex3f(-1.0+(512+235)*0.0025, -0.8+0.0027*(256-242), -1.5000001);
		glTexCoord2i(1,1);
		glVertex3f(-1.0+(512+235)*0.0025, -0.8+0.0027*(256-63), -1.5000001);
	glEnd();
	glEndList();

	// crosshair
	xhair = glGenLists(1);

	glNewList(xhair, GL_COMPILE);

	glColor4f(0.0, 0.8, 0.1, 0.6);
	glBegin(GL_LINES);
		glVertex3f(0.0-0.0025*4, 0.0, -1.5000001);
		glVertex3f(0.0-0.0025*12, 0.0, -1.5000001);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(0.0+0.0025*4, 0.0, -1.5000001);
		glVertex3f(0.0+0.0025*12, 0.0, -1.5000001);
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0-0.0027*4, -1.5000001);
		glVertex3f(0.0, 0.0-0.0027*12, -1.5000001);
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0+0.0027*4, -1.5000001);
		glVertex3f(0.0, 0.0+0.0027*12, -1.5000001);
	glEnd();

	glEndList();


	// set up glScissor for the radar
	float xmod, ymod;
	xmod = getxres()/800.0; ymod = getyres()/600.0;

	glScissor((55+513)*xmod, (256-241)*ymod, ((512+234)-(55+513))*xmod, ((256-64)-(256-241))*ymod);	


	write_log("done! Cockpit complete!\n");
}



void Cockpit::draw()
{
	// draw crosshair
	int i,j;
	GLfloat u,v;
	GLfloat cxr, cyr, czr;

	// draw metal girders with some totally stupid, random environment mapping
	// nobody will notice ;)
	
	glBindTexture(GL_TEXTURE_2D,env->id);
	
	glColor4f(0.7, 0.7, 0.7, 1.0);
	cxr = ((Camera*)getcam())->getxrot()/360.0;
	cyr = ((Camera*)getcam())->getyrot()/360.0;
	czr = ((Camera*)getcam())->getzrot()/360.0;
	u=cxr+cyr*2+czr*4;
	v=cxr*4+cyr*2+czr;

	glBegin(GL_QUADS);
		glTexCoord2f(u+0.2,v+1);
		glVertex3f(-0.57,-0.8,-1.5000001);
		glTexCoord2f(u+0.2,v);
		glVertex3f(-0.47,-0.792,-1.8000001);
		glTexCoord2f(u,v+1);
		glVertex3f(-0.4,-0.792,-1.8000001);
		glTexCoord2f(u,v);
		glVertex3f(-0.5,-0.8,-1.5000001);
		
		glTexCoord2f(u+0.2,v);
		glVertex3f(-0.47,-0.792,-1.8000001);
		glTexCoord2f(u+0.2,v+1);
		glVertex3f(-0.41,-0.5,-2.0);
		glTexCoord2f(u,v);
		glVertex3f(-0.38,-0.5,-2.0);
		glTexCoord2f(u,v+1);
		glVertex3f(-0.4,-0.792,-1.8000001);
		
		glTexCoord2f(u+0.2,v+1);
		glVertex3f(0.57,-0.8,-1.5000001);
		glTexCoord2f(u+0.2,v);
		glVertex3f(0.47,-0.792,-1.8000001);
		glTexCoord2f(u,v+1);
		glVertex3f(0.4,-0.792,-1.8000001);
		glTexCoord2f(u,v);
		glVertex3f(0.5,-0.8,-1.5000001);

		glTexCoord2f(u+0.2,v);
		glVertex3f(0.47,-0.792,-1.8000001);
		glTexCoord2f(u+0.2,v+1);
		glVertex3f(0.4,-0.5,-2.0);
		glTexCoord2f(u,v);
		glVertex3f(0.37,-0.5,-2.0);
		glTexCoord2f(u,v+1);
		glVertex3f(0.4,-0.792,-1.8000001);	
	glEnd();

	
	//glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	
	// draw windshield cracks
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLineWidth(2.0);
	for(i=0; i<3; i++)
	{
		if(cracks[i]!=NULL)
		{
			glPushMatrix();
			glRotatef(cracks[i]->angle,0.0, 0.0, 1.0);
				glTranslatef(cracks[i]->xpos,cracks[i]->ypos,0);
				
				glBegin(GL_LINE_STRIP);
				for(j=0;j<cracks[i]->numverts;j++)
				{
						glColor4f(0.7,0.7,1.0,cracks[i]->alpha[j]);
						glVertex3f(cracks[i]->x[j],cracks[i]->y[j],-1.5000001);
				}
				glEnd();
			glPopMatrix();
		}
	}
	glLineWidth(1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glCallList(xhair);

	glEnable(GL_TEXTURE_2D);
	
	// call the panels display list here
	
	glColor4f(cpcol[0], cpcol[1], cpcol[2], 1.0);
	glCallList(panels);

	// draw gauges and radar
	gauges();
	
	

	// draw the compass
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	horizon->draw();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	if(pongmode==1)
		playpong();
}



void Cockpit::displaykeys()
{
	// react to display control keys like switch display state or change alpha value
	GLfloat ticks=gettime();
	GLfloat change = ticks*ALPHA_PER_SECOND;
	char* keys = getkeys();

	// make displays transparent
	if(keys[SDLK_9])
	{
		alpha-=change; if(alpha<0.17) alpha=0.17;
	}
	if(keys[SDLK_0])
	{
		alpha+=change; if(alpha>0.8) alpha=0.8;
	}

	// switch display states
	if(keys[SDLK_q])
	{
		keys[SDLK_q]=0; if(display[0]==CP_MAP) display[0]=CP_TARGET; else display[0]=CP_MAP;
		playsam(8,15000,135,200);
	}
	if(keys[SDLK_w])
	{
		keys[SDLK_w]=0; if(display[1]==CP_INFO) display[1]=CP_OBJECTIVES; else display[1]=CP_INFO;
		playsam(8,15000,135,200);
	}
	if(keys[SDLK_e])
	{
		keys[SDLK_e]=0; if(display[2]==CP_RADAR) display[2]=CP_EQUIP; else display[2]=CP_RADAR;
		playsam(8,15000,135,200);
	}
	
	// zoom radar
	if(keys[SDLK_k])
	{
		radarxzoom+=ticks*RADARZOOMER;
		if(radarxzoom>RADARMINZOOM) radarxzoom = RADARMINZOOM; 
		radaryzoom = radarxzoom*0.8667;
	}
	if(keys[SDLK_l])
	{
		radarxzoom-=ticks*RADARZOOMER;
		if(radarxzoom<RADARMAXZOOM) radarxzoom = RADARMAXZOOM; 
		radaryzoom = radarxzoom*0.8667;
	}

	// zoom map
	if(keys[SDLK_o])
	{
		mapzoom-=ticks*MAPZOOMER;
		if(mapzoom<MAPMINZOOM) mapzoom = MAPMINZOOM;
	}
	if(keys[SDLK_p])
	{
		mapzoom+=ticks*MAPZOOMER;
		if(mapzoom>MAPMAXZOOM) mapzoom = MAPMAXZOOM;
	}


	// play sonar ping sample
	if(display[2] == CP_RADAR)
	{
		sonarpingtime-=ticks;
		if(sonarpingtime<0) 
		{
			sonarpingtime=SONARPINGDELAY;
			playsam(14,16000,125,255);
		}
	}
}


void Cockpit::recover()
{
	// adjust panel color
	if(cpcol[2]<0.8)
	{
		// recover damage color
		GLfloat ticks = gettime();
		cpcol[0]-=ticks*DAMAGEFADE/2;
		cpcol[1]+=ticks*DAMAGEFADE/1.2;
		cpcol[2]+=ticks*DAMAGEFADE;
		if(cpcol[0]<0.8) cpcol[0]=0.8;
		if(cpcol[1]>0.8) cpcol[1]=0.8;
		if(cpcol[2]>0.8) cpcol[2]=0.8;
	}
}



void Cockpit::setPlayer(player *pl)
{
	this->pl = pl;
}



void Cockpit::gauges()
{
	// draw the info onto the displays
	_FBUF
	GLfloat dcol[4] = {0.9, 0.9, 0.9, alpha};
	GLfloat fcol[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat scol[4] = {1.0, 1.0, 1.0, 1.0 };
	GLfloat nfcol[4] = {0.8, 0.8, 0.0, 1.0 };	// no more weapons, weapon not selected
	GLfloat nscol[4] = {1.0, 0.1, 0.0, 1.0 };	// no more weapons, weapon selected
	GLfloat ccol[4] = {0.0, 0.0, 0.0, 1.0 };
	GLfloat hdng, compr;
	model* tgt=NULL;
	waypoint* wpt = NULL;
	int i;
	int mb = 385, md=15;
	int lb = 425, ld=16;
	int rb = 425, rd=16;
	
	GLfloat ticks = gettime();

	// get waypoint object
	wpt = (waypoint*)pl->getwaypoint();

	// get target object
    if((model*)pl->getTarget()!=NULL)
    {
	    tgt = (model*)pl->getTarget()->obj;
		objectlist* objlist=((mission*)getmission())->get_objlist();
		if(objlist->in_list(tgt)==NULL)
		{
			tgt=NULL;
		}
		else
		{
			if(tgt->getmvstate()==MV_DESTROY)
			{
				tgt=NULL;
			}
		}
    }
	else
    {
        display[0] = CP_MAP;
    }

	// rotate the compass appropriately
	horizon->control();



	// call the displays

	// calculate the player heading anyway, we need it for many display capabilities
	hdng = heading(pl->getdx(), pl->getdz());
	
	// left display first. display either target info or minimap
	if(display[0] == CP_MAP)
	{
		// minimap! set up texture coordinates
		// NOW THIS ROUTINE COMPLETELY RULEZZZZZZZZZ
		GLfloat mx1, mx2, my1, my2;
		GLfloat xmax, ymax, plx, ply;
		GLfloat dw = 0.390625; // display height and width (200/512), (169/512)
		GLfloat dh = 0.330078125;
		GLfloat xpl, ypl;

		plx = pl->getx(); ply = pl->getz();
		xmax = (((mission*)getmission())->getworld())->getxwidth();
		ymax = (((mission*)getmission())->getworld())->getzwidth();

		// scale ship coords to minimap coords
		plx = plx/xmax; ply = ply/ymax;
		
		// calc minimap coords
		mx1 = plx-dw/mapzoom; mx2 = plx+dw/mapzoom;
		my1 = ply-dh/mapzoom; my2 = ply+dh/mapzoom;
		
		// check the map doesn´t wrap
		if(mx1<0) { mx1=0; mx2=dw/mapzoom*2; }
		if(my1<0) { my1=0; my2=dh/mapzoom*2; }
		if(mx2>1) { mx2=1; mx1=1.0-dw/mapzoom*2; }
		if(my2>1) { my2=1; my1=1.0-dh/mapzoom*2; }

		// set the miniship coordinates and convert them to device coordinates
		// which will then be mapped to viewport coords
		xpl = plx-mx1; ypl = ply-my1;
		xpl = xpl*256*mapzoom; ypl=ypl*256*mapzoom;
				
		// draw the map
		glColor4f(dcol[0], dcol[1], dcol[2], dcol[3]+0.24);
		
		glBindTexture(GL_TEXTURE_2D, minimap->id);
		glBegin(GL_QUADS);
			glTexCoord2f(mx1,my1);
			glVertex3f(-1.0+22*0.0025, -0.8+0.0027*(256-60), -1.5000001);
			glTexCoord2f(mx1,my2);
			glVertex3f(-1.0+22*0.0025, -0.8+0.0027*(256-229), -1.5000001);
			glTexCoord2f(mx2,my2);
			glVertex3f(-1.0+222*0.0025, -0.8+0.0027*(256-229), -1.5000001);
			glTexCoord2f(mx2,my1);
			glVertex3f(-1.0+222*0.0025, -0.8+0.0027*(256-60), -1.5000001);
		glEnd();

		// draw the player
		glPushMatrix();
		glTranslatef(-1.0+(22+xpl)*0.0025, (-104-ypl)*0.0027, 0);
		glRotatef(-hdng, 0.0, 0.0, 1.0);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);
			glColor4f(0.3, 1.0, 0.3, dcol[3]+0.24);
			glVertex3f(0.0, 7*0.0027, -1.5000001);
			glVertex3f(-5*0.0025, -7*0.0027, -1.5000001);
			glVertex3f(5*0.0025, -7*0.0027, -1.5000001);
		glEnd();
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);

		// print zoom info
		sprintf(buf, "ZOOM: %2.2f X", mapzoom); print(90, lb, buf, 0, fcol[0],fcol[1],fcol[2],fcol[3]);
	}
	else
	{
		if(tgt!=NULL)
		{
			// display small wireframe of object
			glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
			glPushMatrix();
				glColor4f(0.0,0.0,0.0,0.4);
				GLfloat scale = tgt->getScale()*0.25/tgt->bounding_rad();
				glTranslatef(-0.9,-0.7,-2);
				glScalef(scale,scale,scale);
				glMultMatrixd(pl->getinvmat());
				glMultMatrixd(tgt->getmat());
				if(!getWire()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glCallList(tgt->getDisplaylist());
				if(!getWire()) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPopMatrix();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		// first draw panel
		glColor4fv(dcol);
		glCallList(leftdis);

		// now display INFO
		// header
		print(90,lb,"TARGET:",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		if(tgt!=NULL)
		{
			// name
			if(tgt->get_enemystatus()==ENEMY)
			{
				sprintf(buf, "%s", tgt->getName()); print(60,lb+=ld,buf,0,nscol[0],nscol[1],nscol[2],nscol[3]);				
			}
			else
			{
				sprintf(buf, "%s", tgt->getName()); print(60,lb+=ld,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);
			}
		
			// heading
			print(40,lb+=ld*2, "HEADING",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf, "%3.2f^", heading((tgt->getx()-pl->getx()), (tgt->getz()-pl->getz()))); print(130,lb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);
		
			// speed
			print(40,lb+=ld, "VELOCITY",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf,"%3.2f KTS", tgt->getSpeed()*0.52483); print(130,lb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

			// distance
			print(40,lb+=ld, "DISTANCE", 0, fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf, "%4.0f M", fastdist3((tgt->getx()-pl->getx()), (tgt->gety()-pl->gety()), (tgt->getz()-pl->getz()))*0.27); print(130,lb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

			// most interestingly, the energy
			print(40,lb+=ld, "HULL",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf,"%3.2f", ((float)tgt->getEnergy()/tgt->getFullNRG()*100.0)); strcat(buf,"%"); print(130,lb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);
		}
	}
	
	
	// middle display. either ship/waypoint info or mission targets
	if(display[1] == CP_INFO)
	{
		glColor4fv(dcol);
		glCallList(middis);

		// get the player´s compression value and set emergency color accordingly
		compr = fabs(pl->getCompression());
		ccol[0] = 0.0 + compr/1100.0;
		//ccol[1] = 0.8 - compr/1800.0;


		// heading
		print(300,mb,"HEADING",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		sprintf(buf,"%3.2f^", hdng); print(400,mb, buf, 0, fcol[0],fcol[1],fcol[2],fcol[3]);

		// speed
		print(300,mb+=md,"VELOCITY",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		sprintf(buf,"%3.2f KTS", pl->getSpeed()*0.52483); print(400,mb, buf, 0, fcol[0],fcol[1],fcol[2],fcol[3]);

		// depth
		print(300,mb+=md,"DEPTH",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		sprintf(buf, "%d M", pl->getDepth()); print(400,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

		// pressure
		print(300,mb+=md,"PRESSURE",0,ccol[0],ccol[1],ccol[2],ccol[3]);
		sprintf(buf, "%d BAR", pl->getPressure()); print(400,mb,buf,0,ccol[0],ccol[1],ccol[2],ccol[3]);

		// sonar
		print(300,mb+=md,"SONAR",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		sprintf(buf, "%d M", (int)(pl->getAlt()*0.27)); print(400,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

		mb+=2*md;

		if(wpt!=NULL)
		{
			// next waypoint
			sprintf(buf,"NEXT WAYPOINT: W %d",wpt->get_serial());
			print(325,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

			if(wpt->get_reached())
			{
				print(325,mb+md,"CHECKED",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			}
			// waypoint heading
			print(300,mb+=2*md,"WPT HEADING",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf,"%3.2f^", heading(wpt->getx()-pl->getx(), wpt->getz()-pl->getz()));
			print(420,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

			// waypoint distance
			print(300,mb+=md,"WPT DISTANCE",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf,"%d M", (int)distance(pl,wpt)); print(420,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);

			// waypoint depth
			print(300,mb+=md,"WPT DEPTH",0,fcol[0],fcol[1],fcol[2],fcol[3]);
			sprintf(buf,"%d M", (int)(calcdepth(wpt->gety()))); print(420,mb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);
		}

	}
	else if(display[1] == CP_OBJECTIVES)
	{
		glColor4fv(dcol);
		glCallList(middis);

		// title
		print(330,mb,"MISSION OBJECTIVES:",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		
		int j = ((mission*)getmission())->get_numgoals();
		for(int i=0;i<j;i++)
		{
			if((((mission*)getmission())->get_goal(i))->failed)
			{
				print(300,mb+=2*md,(((mission*)getmission())->get_goal(i))->text,0,nscol[0],nscol[1],nscol[2],nscol[3]);	
			}
			else if((((mission*)getmission())->get_goal(i))->success)
			{
				print(300,mb+=2*md,(((mission*)getmission())->get_goal(i))->text,0,fcol[0],fcol[1],fcol[2],fcol[3]);	
			}
			else
			{
				print(300,mb+=2*md,(((mission*)getmission())->get_goal(i))->text,0,nfcol[0],nfcol[1],nfcol[2],nfcol[3]);	
			}
		}
	
	}
	
	
	// right display. either weapon info or radar
	if(display[2] == CP_EQUIP)
	{
		glColor4fv(dcol);
		glCallList(ritedis);

		// grab player´s weapon array
		GLfloat* wcol;
		weapon_t* weaps = pl->getweapons();
		int weapc = pl->getweapcount();
		int actw = pl->getactiveweapon();

		// title
		print(635,rb,"STATUS",0,fcol[0],fcol[1],fcol[2],fcol[3]);

		// hull
		print(585,rb+=2*rd,"HULL",0,fcol[0],fcol[1],fcol[2],fcol[3]);
		sprintf(buf,"%3.2f", ((float)pl->getEnergy()/pl->getFullNRG()*100.0)); strcat(buf,"%"); print(665,rb,buf,0,fcol[0],fcol[1],fcol[2],fcol[3]);
	
		// print all weapons
		rb+=2*rd;
		for(i=0; i<weapc; i++)
		{
			// change color if weapon is selected
			if(i==actw&&weaps[i].amount==0)
			{
				wcol = nscol;
			}
			if(i!=actw&&weaps[i].amount==0)
			{
				wcol = nfcol;
			}
			if(i==actw&&weaps[i].amount!=0)
			{
				wcol = scol;
			}
			if(i!=actw&&weaps[i].amount!=0)
			{
				wcol = fcol;
			}

			sprintf(buf, "%s:", weaps[i].name);
			print(585,rb,buf,0,wcol[0], wcol[1], wcol[2], wcol[3]);
			if(weaps[i].amount>=0)
			{
				sprintf(buf, "%d X", weaps[i].amount);
				print(700,rb,buf,0,wcol[0], wcol[1], wcol[2], wcol[3]);
			}
			rb+=rd;
		}
	
	}
	else if(display[2] == CP_RADAR)
	{
		glColor4fv(dcol);
		glCallList(radardis);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(2.0);

		object* tmp;
		listobj* obj;
		int mass;
		GLfloat px, py, pz, ox, oy, oz, oalt;
		GLfloat rcol[4]; // radar object color
		GLdouble rmat[16], rpt[4], res[4];

		rcol[3] = 0.9;

		// setup the rotation matrix for the radar objx
		setup_mat_zrot(rmat, -hdng);
		px = pl->getx(); py = pl->gety(); pz = pl->getz();

		// draw radar screen
		glEnable(GL_SCISSOR_TEST);
		
		glPushMatrix();
			// shift radar screen into place	
			glTranslatef(radarxpos, radarypos, 0.0);
			
			// grab first list element
			obj = (((mission*)getmission())->get_objlist())->get_first();
			
			while(obj!=NULL) // walk object list
			{
				tmp = obj->obj; // grab object

				// we only draw ships, buildings and homies.. and other objx
				if(tmp->Typ()!=PLAYER && tmp->Typ()!=DIRECT && tmp->Typ()!=WAYPOINT)
				{
					ox = tmp->getx(); oy = tmp->gety(); oz = tmp->getz();
					
					// check if object is in radar range
					if((fabs(px-ox)<radarxzoom) && (fabs(py-oy)<radaryzoom) && (fabs(pz-oz)<radarxzoom))
					{
						// mass might be used for color coding, just like in elite
						mass = ((model*)tmp)->getMass();
						if(mass<10.0) { rcol[0] = 0.1; rcol[1] = 0.2; rcol[2] = 0.85; }
						else if(mass<20.0) { rcol[0] = 1.0; rcol[1] = 0.2; rcol[2] = 0.1; }
						else if(mass<150.0) { rcol[0] = 0.8; rcol[1] = 0.55; rcol[2] = 0.1; }
						else if(mass<500.0) { rcol[0] = 0.7; rcol[1] = 0.1; rcol[2] = 0.85; }
						else { rcol[0] = 1.0; rcol[1] = 1.0; rcol[2] = 1.0; }
						glColor4fv(rcol);

						// calc positions on radar
						ox = (px-ox)/radarxzoom*radarxextent;
						oalt = (py-oy)/radaryzoom*radaraltextent;
						oz = (pz-oz)/radarxzoom*radaryextent;

						// rotate the radar object into place using our own matrix multiplications
						rpt[0] = ox; rpt[1] = oz; rpt[2] = 0.0; rpt[3] = 1.0;
						mat_pt_multiply(rmat, rpt, res);

						// draw the object on the radar screen
						glBegin(GL_LINE_STRIP);
							glVertex3f(-res[0]+0.01, res[1], -1.5000001);
							glVertex3f(-res[0], res[1], -1.5000001);
							glVertex3f(-res[0], res[1]+oalt, -1.5000001);	
						glEnd();

						if(tgt!=NULL)
						if(tmp == tgt) // mark player´s target!
						{
							glColor4f(0.5, 0.8, 1.0, 0.9);
							glLineWidth(1.0);
							glBegin(GL_LINE_STRIP);
								glVertex3f(-res[0]+0.015, res[1]+0.01, -1.5000001);
								glVertex3f(-res[0]+0.015, res[1]-0.01, -1.5000001);
								glVertex3f(-res[0]-0.015, res[1]-0.01, -1.5000001);	
								glVertex3f(-res[0]-0.015, res[1]+0.01, -1.5000001);
								glVertex3f(-res[0]+0.015, res[1]+0.01, -1.5000001);
							glEnd();
							glLineWidth(2.0);
						}
					}
				}
				obj = (listobj*)(obj->next); // continue in list
			}
			glLineWidth(1.0);

			// finally, draw next waypoint
			if(wpt!=NULL)
			{
				ox = wpt->getx(); oz = wpt->getz();
				ox = (px-ox)/radarxzoom*radarxextent;
				oz = (pz-oz)/radarxzoom*radaryextent;

				// rotate the radar object into place using our own matrix multiplications
				rpt[0] = ox; rpt[1] = oz; rpt[2] = 0.0; rpt[3] = 1.0;
				mat_pt_multiply(rmat, rpt, res);

				// draw the object on the radar screen
				glColor4f(1.0,1.0,1.0,0.7);
				glBegin(GL_LINES);
					glVertex3f(-res[0]-0.01, res[1], -1.5000001);
					glVertex3f(-res[0]+0.01, res[1], -1.5000001);
					glVertex3f(-res[0], res[1]-0.01, -1.5000001);	
					glVertex3f(-res[0], res[1]+0.01, -1.5000001);	
				glEnd();
			}


		glPopMatrix();

		// print zoom state
		sprintf(buf, "ZOOM: %2.2f X", RADARMINZOOM/radarxzoom); print(630, rb, buf, 0, fcol[0],fcol[1],fcol[2],fcol[3]);

		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_TEXTURE_2D);
	}
}

// sets displayfunctions
void Cockpit::setDisplay(int nr, int value)
{
	display[nr]=value;
}


void Cockpit::hit()
{
	// paint display in red if hit
	cpcol[0] = 1.0; cpcol[1]=0.2; cpcol[2]=0.0;
}


void Cockpit::genCrack(int num)
{
	// generate a crack in the windshield
	
	GLfloat cx=0.0, cy=0.0, length=0.0;
	cracks[num] = new crack;
	Camera* cam = (Camera*)getcam();

	switch(num)
	{
	case 0: // first one, small crack
		cracks[num]->numverts = 12;
		playsam(17,16000,distantvolume(cam->getx(), cam->gety(), cam->getz(), pl->getx(), pl->gety(), pl->getz()),255);
		break;
	case 1: // large crack
		cracks[num]->numverts = 23;
		playsam(18,16000,distantvolume(cam->getx(), cam->gety(), cam->getz(), pl->getx(), pl->gety(), pl->getz()),255);
		break;
	case 2: // water leak
		cracks[num]->numverts = 16;
		playsam(19,16000,distantvolume(cam->getx(), cam->gety(), cam->getz(), pl->getx(), pl->gety(), pl->getz()),255);
		break;
	}
	cracks[num]->xpos = ((rand()/(float)RAND_MAX)-1.0)/1.5;
	cracks[num]->ypos = ((rand()/(float)RAND_MAX)-0.8)/2.2+0.3;
	cracks[num]->angle = (rand()/(float)RAND_MAX)*360.0;
	cracks[num]->x = (GLfloat*)malloc(cracks[num]->numverts*sizeof(GLfloat));
	cracks[num]->y = (GLfloat*)malloc(cracks[num]->numverts*sizeof(GLfloat));
	cracks[num]->alpha = (GLfloat*)malloc(cracks[num]->numverts*sizeof(GLfloat));


	// randomize crack verts
	for(int i=0; i<cracks[num]->numverts; i++)
	{
		cracks[num]->x[i] = cx;
		cracks[num]->y[i] = cy;
		cracks[num]->alpha[i] = 0.75-(rand()/(float)RAND_MAX)/2;
		cx+=(rand()/(float)RAND_MAX)/12;
		cy=((rand()/(float)RAND_MAX)-0.5)/9;
	}
	length = cracks[num]->x[cracks[num]->numverts-1];
	cracks[num]->xpos-=length/2;
}

int Cockpit::getpongable()
{
	// return rite displaystate
	if(display[2]==CP_RADAR)
		return 1;
	else
		return 0;
}


void Cockpit::setPongmode(int p)
{
	// set the easteregg state
	_FBUF

	if(p==0) // switch it off
	{
		if(pongmode==1)
		{
			pongmode=0;
			fademusic(1.0); stopmod();
			sprintf(buf,"sfx/utah-saints.mod"); loadmod(buf); setmodvol(TITLEVOL);
		}
	}
	if(p==1)
	{
		if(pongmode==0)
		{
			pongmode=1;
			fademusic(1.0); stopmod();
			sprintf(buf,"sfx/wasserfast.mod"); loadmod(buf); playmod(); setmodvol(200);
			// init coords
			pg.bx = 50.0;
			pg.by = 50.0;
			pg.y[0] = 50.0;
			pg.y[1] = 50.0;
			pg.bxd = rand()/(float)RAND_MAX*50-25;
			if(pg.bxd<0) pg.bxd = 81.0; else pg.bxd = -81.0;
			pg.byd = rand()/(float)RAND_MAX*130-65;
			points1=0;
			points2=0;
		}
	}
}


int Cockpit::getPongmode()
{
	return pongmode;
}


void Cockpit::playpong()
{
	// play the most incredible PONG version ever ;)
	GLfloat bx, by;
	GLfloat pdy[2], pdx[2];
	GLfloat ticks = gettime();
	char* keys = getkeys();
	GLfloat padspeed = 75.0;

	// control
	if(keys[SDLK_UP])
	{
		pg.y[0]+=padspeed*ticks;
	}
	if(keys[SDLK_DOWN])
	{
		pg.y[0]-=padspeed*ticks;
	}

	// AI :)
	if(pg.by<pg.y[1]) pg.y[1]-=padspeed*ticks/1.2;
	if(pg.by>pg.y[1]) pg.y[1]+=padspeed*ticks/1.2;


	for(int i=0; i<2; i++)
	{
		if(pg.y[i]<15) pg.y[i]=15;
		if(pg.y[i]>85) pg.y[i]=85;
	}

	// colision detection
	if(pg.bx<10)
	{
		if(fabs(pg.by-pg.y[0])<18)
		{
			pg.bxd*=-1;
			pg.bx = 10;
			pg.byd=(pg.by-pg.y[0])*7.16;
			playsam(14,22000,255,255);
		}
	}
	if(pg.bx>90)
	{
		if(fabs(pg.by-pg.y[1])<18)
		{
			pg.bxd*=-1;
			pg.bx = 90;
			pg.byd=(pg.by-pg.y[1])*7.16;
			playsam(14,22000,255,255);
		}
	}


	// movement
	pg.bx+=pg.bxd*ticks;
	pg.by+=pg.byd*ticks;

	if(pg.bx < 0)
	{
		pg.bx=50.0;
		pg.bxd = rand()/(float)RAND_MAX*50-25;
		if(pg.bxd<0) pg.bxd = 81.0; else pg.bxd = -81.0;
		pg.byd = rand()/(float)RAND_MAX*130-65;
		playsam(14,14000,255,255);
		points2++;
		pg.y[0]=50.0; pg.y[1]=50.0;
	}
	if(pg.bx > 100)
	{
		pg.bx=50.0;
		pg.bxd = rand()/(float)RAND_MAX*50-25;
		if(pg.bxd<0) pg.bxd = 81.0; else pg.bxd = -81.0;
		pg.byd = rand()/(float)RAND_MAX*130-65;
		playsam(14,14000,255,255);
		points1++;
		pg.y[0]=50.0; pg.y[1]=50.0;
	}

	if(pg.by < 0) {pg.by=0.0; pg.byd*=-1; playsam(14,24000,255,255);}
	if(pg.by > 100){ pg.by=100.0; pg.byd*=-1; playsam(14,24000,255,255);}

	bx = radarxpos+(pg.bx-50.0)/275.0;
	by = radarypos+(pg.by-50.0)/300.0;
	pdx[0] = radarxpos+(3-50.0)/275.0;
	pdx[1] = radarxpos+(97-50.0)/275.0;
	pdy[0] = radarypos+(pg.y[0]-50.0)/300.0;
	pdy[1] = radarypos+(pg.y[1]-50.0)/300.0;

	glColor4f(0.1, 0.8, 0.1, alpha+0.3);

	glBegin(GL_QUADS);
		glVertex3f(bx-0.01, by-0.01, -1.5000001);
		glVertex3f(bx-0.01, by+0.01, -1.5000001);
		glVertex3f(bx+0.01, by+0.01, -1.5000001);
		glVertex3f(bx+0.01, by-0.01, -1.5000001);
	
		glVertex3f(pdx[0]-0.01, pdy[0]-0.05, -1.5000001);
		glVertex3f(pdx[0]-0.01, pdy[0]+0.05, -1.5000001);
		glVertex3f(pdx[0]+0.01, pdy[0]+0.05, -1.5000001);
		glVertex3f(pdx[0]+0.01, pdy[0]-0.05, -1.5000001);

		glVertex3f(pdx[1]-0.01, pdy[1]-0.05, -1.5000001);
		glVertex3f(pdx[1]-0.01, pdy[1]+0.05, -1.5000001);
		glVertex3f(pdx[1]+0.01, pdy[1]+0.05, -1.5000001);
		glVertex3f(pdx[1]+0.01, pdy[1]-0.05, -1.5000001);
	glEnd();

	_FBUF
	sprintf(buf, "%d", points1); print(620, 500, buf, 1, 0.2, 0.8, 0.2, 0.5);
	sprintf(buf, "%d", points2); print(680, 500, buf, 1, 0.2, 0.8, 0.2, 0.5);
	
	
}

