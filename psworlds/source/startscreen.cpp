// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "startscreen.h"
#include "psworlds.h"
#include "tgaloader.h"
#include "sound.h"
#include "menu.h"
#include "fonts.h"
#include "utility.h"
#include "other.h"


#define startscreen_FADESPEED 0.75
#define MOD_FADESPEED startscreen_FADESPEED

startscreen::startscreen()
{
	write_log("Creating startscreen...");

	initsound();

	write_log("done!\n");
}

startscreen::~startscreen()
{
	initnormalvoices();
	write_log("startscreen quitted\n");
}


void startscreen::initsound()
{
	// destroy all previous channels
	destroyvoices();
	initintrovoices();
}


void startscreen::bootpic()
{
	// display booting info and pictures and start music!
	GLfloat wt = 0, prevwt=0;
	GLfloat litepos[] = { 0.2, 1.0, 0.4, 0.0 };
	GLfloat no[] = {1.0, 1.0, 1.0};
	int swp[3]={ 0, 1, 2};
	_FBUF
	sprintf(buf,"logo.obj");
	other* logo = new other(buf, 0, 0, 0.0, 0, 0, 0, 0, 0, 1000, 0.1,no,swp);
	other* logo2 = new other("logo2.obj", 0, 0, 0.0, 0, 0, 0, 0, 0, 1000, 0.1,no,swp);

	GLfloat bgcl=1.0; GLfloat bgfade=1.2;
	playsam(21,21000,255,255);
	while(wt<4.5)
	{
		timing();
		bgcl-=bgfade*gettime();
		glClearColor(bgcl, bgcl, bgcl, bgcl);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor4f(0.5, 0.58, 0.8, 1.0);
		glBegin(GL_LINES);
			glVertex3f(-0.94,-0.62,-1.5000001);
			glVertex3f(0.94,-0.62,-1.5000001);
		glEnd();
		print(30,560,"A STURM / UMLAUF PRODUCTION",2,0.5,0.58,0.8,1.0);
		drawstrings();
		glFlush();
		SDL_GL_SwapBuffers();
		wt+=gettime();
	}

	wt=0.0;
	playsam(22,21000,255,255);
	GLfloat logorot = 0.0;
	GLfloat logorotspd = 72.0;
	GLfloat logoz = -1.2;
	GLfloat lzsp = -6.36;
	GLfloat logoy = -0.2;
	GLfloat lysp = 1.84;
	GLfloat logorot2 = -15.0;
	GLfloat logorotspd2 = 75.0;
	GLfloat logoz2 = 1.0;
	GLfloat lzsp2 = -6.76;
	GLfloat logoy2 = -0.2;
	GLfloat lysp2 = 1.84;
	GLfloat cover=1.0;
	GLfloat cvspd = 0.0;

	glLightfv(GL_LIGHT0, GL_POSITION,litepos);


	while(wt<12.0)
	{
		timing();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		

		// trigger logo zoom out
		if(prevwt<7.5 && wt>=7.5)
		{
			playsam(23,22000,255,255);
			lzsp=12.7; lzsp2=11; lysp=-3.4; lysp2=-3.4;
		}

		// trigger fade
		if(prevwt<9.0 && wt>=9.0)
		{
			cvspd=-0.5;
		}

		// logo rotation
		logorot+=logorotspd*gettime();
		logoz+=lzsp*gettime();
		logoy+=lysp*gettime();
		logorot2+=logorotspd2*gettime();
		logoz2+=lzsp2*gettime();
		logoy2+=lysp2*gettime();
		cover+=cvspd*gettime();

		if(logorot>360.0)
		{
			logorot=360.0;
			logorot2=360.0;
			lzsp=0; lzsp2=0; lysp=0; lysp2=0; logorotspd=0; logorotspd2=0;
		}

		// draw line
		glBegin(GL_LINES);
			glColor4f(0.5, 0.58, 0.8, cover);
			glVertex3f(-0.96,-0.15,-1.5000001);
			glVertex3f(0.96,-0.15,-1.5000001);
		glEnd();

		// logo drawing
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glPushMatrix();		
			glTranslatef(0,logoy,logoz);
			glRotatef(logorot+180,0.0,1.0,0.0);
			logo->draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,logoy-5,logoz);
			glRotatef(logorot+180,0.0,1.0,0.0);
			logo2->draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,logoy2,logoz2);
			glRotatef(logorot2+180,0.0,1.0,0.0);
			logo->draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,logoy2-5,logoz2);
			glRotatef(logorot2+180,0.0,1.0,0.0);
			logo2->draw();
		glPopMatrix();
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_2D);

		// text
		print(20, 400, "POSSIBLE WORLDS VERSION 1.1", 2, 0.5, 0.58,0.8,cover);
		if(SYSTEM==WINDOWS)
			print(20, 420, "VISUAL C++ BUILD", 2, 0.5, 0.58,0.8,cover);
		else
			print(20, 420, "GCC LINUX BUILD", 2, 0.5, 0.58,0.8,cover);
		print(20, 440, "DESIGNED FOR CGLU23 2001", 2, 0.5, 0.58,0.8,cover);
		print(20, 460, "TECHNICAL UNIVERSITY OF VIENNA", 2, 0.5, 0.58,0.8,cover);
		print(20, 480, "SEE CREDITS FOR MORE INFO", 2, 0.5, 0.58,0.8,cover);
		print(20, 500, "ALL MATERIAL BY STURM / UMLAUF EXCEPT WHERE STATED", 2, 0.5, 0.58,0.8,cover);
		print(20, 520, "OPENGL ACCELERATION HEAVILY RECOMMENDED", 2, 0.5, 0.58,0.8,cover);

		// stuff
		drawstrings();
		glFlush();
		SDL_GL_SwapBuffers();
		prevwt=wt;
		wt+=gettime();
	}
	delete logo; delete logo2;
}



void startscreen::run()
{
	// the sequence main loop
	timing();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	bootpic();	// display boot picutre

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	
}


