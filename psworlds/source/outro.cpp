// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "outro.h"
#include "psworlds.h"
#include "sound.h"
#include "fonts.h"
#include "menu.h"
#include "utility.h"
#include "other.h"


#define CREDITS_FADESPEED 0.6
#define CMOD_FADESPEED CREDITS_FADESPEED

#define OUTERZ 12.0
#define INNERZ 1.0
#define OUTERX 4.0
#define INNERX 1.0

#define CREDVOL 160

#define XSCALE_SPEED CREDITS_FADESPEED*(OUTERX-INNERX)
#define ZSCALE_SPEED CREDITS_FADESPEED*(OUTERZ-INNERZ)

#define L_REAR -15.0
#define L_FRONT -5.5


outro::outro()
{
	GLfloat no[3]={ 1.0, 1.0, 1.0};
	int swp[3]={ 0, 1, 2};
	_FBUF
	int i,j;

	write_log("Creating credits sequence...");
	logo = new other("logo.obj", 0, 0, 0.0, 0, 0, 0, 0, 0, 1000, 0.1, no,swp);
	logo2 = new other("logo2.obj", 0, 0, 0.0, 0, 0, 0, 0, 0, 1000, 0.1, no,swp);
	lpos = 25; lpos2 = 62; lspd=-3.0;

	for(i=0;i<40;i++)
	{ 
		for(j=0; j<15; j++) 
		{
			txt[i][j]=NULL;
		}
	}
	al1=0.0;
	al2=0.0;
	allal=1.0;
	bgal = 0.0;
	bgx=10.0; bgy=1.0; bgwait=12.0;
	tm=0.0;
	step=0;
	killme=0;
	quit=0;


	xsc[4]=INNERX; zsc[4]=OUTERZ;
	xsc[5]=INNERX; zsc[5]=OUTERZ;


	// load layer gfx
	sprintf(buf, "gfx/credlayer1.tga"); layer[0] = new textureObj(buf,1);
	sprintf(buf, "gfx/credlayer2.tga"); layer[1] = new textureObj(buf,1);
	sprintf(buf, "gfx/credlayer3.tga"); layer[2] = new textureObj(buf,1);
	sprintf(buf, "gfx/credlayer4.tga"); layer[3] = new textureObj(buf,1);

	// load bg gfx
	for(i=0; i<12; i++)
	{
		sprintf(buf,"gfx/credits/credits%d.tga",i+1);
		bg[i] = new textureObj(buf,1);
	}

	this->xd[0] = rand()/(float)RAND_MAX*2.0-1.0;
	this->xd[1] = rand()/(float)RAND_MAX*2.0-1.0;
	this->xd[2] = rand()/(float)RAND_MAX*2.0-1.0;
	this->xd[3] = rand()/(float)RAND_MAX*2.0-1.0;

	this->yd[0] = rand()/(float)RAND_MAX*2.0-1.0;
	this->yd[1] = rand()/(float)RAND_MAX*2.0-1.0;
	this->yd[2] = rand()/(float)RAND_MAX*2.0-1.0;
	this->yd[3] = rand()/(float)RAND_MAX*2.0-1.0;

	this->zd[0] = 3.0; this->zd[1] = 2.1;
	this->zd[2] = 5.6; this->zd[3] = 3.6;

	this->xsc[0] = 1.0; this->xsc[1] = 1.0;
	this->xsc[2] = 1.0; this->xsc[3] = 1.0;
	this->zsc[0] = 1.0; this->zsc[1] = 1.0;
	this->zsc[2] = 1.0; this->zsc[3] = 1.0;

	this->rot[0] = rand()/(float)RAND_MAX*360.0;
	this->rot[1] = rand()/(float)RAND_MAX*360.0;
	this->rot[2] = rand()/(float)RAND_MAX*360.0;
	this->rot[3] = rand()/(float)RAND_MAX*360.0;

	this->rsp[0] = 0.0; this->rsp[1] = 3.0;
	this->rsp[2] = -2.0; this->rsp[3] = 0.0;

	this->lc[0][0] = 1.0; this->lc[0][1] = 0.1; this->lc[0][2] = 0.0; this->lc[0][3] = 0.0;
	this->lc[1][0] = 0.3; this->lc[1][1] = 0.5; this->lc[1][2] = 1.0; this->lc[1][3] = 0.0;
	this->lc[2][0] = 0.1; this->lc[2][1] = 1.0; this->lc[2][2] = 0.2; this->lc[2][3] = 0.0;
	this->lc[3][0] = 0.05; this->lc[3][1] = 0.3; this->lc[3][2] = 1.0; this->lc[3][3] = 0.0;

	this->ta[0] = 0.5; this->ta[1] = 0.0;
	this->ta[2] = 0.4; this->ta[3] = 0.95;

	this->lx[0] = -30.0; this->lx[1] = -30.0; this->lx[2] = -30.0; this->lx[3] = -30.0;
	this->ly[0] = -30.0; this->ly[1] = -30.0; this->ly[2] = -30.0; this->ly[3] = -30.0;
	this->lz[0] = L_REAR-3.0; this->lz[1] = L_REAR-3.2; this->lz[2] = L_REAR-1.5; this->lz[3] = L_REAR-1.7;

	this->ts[0] = 2; this->ts[1] = 0.8; this->ts[2] = 1; this->ts[3] = 0.5;


	// script intro and outro sequences
	makestring(txt[0][0], "POSSIBLE WORLDS");
	makestring(txt[0][1]," ");
	makestring(txt[0][2],"NOTHING IS IMPOSSIBLE");

	makestring(txt[1][0], "BY");
	makestring(txt[1][1], " ");
	makestring(txt[1][2], "EIKE J. UMLAUF A.K.A. EIKZILLA");
	makestring(txt[1][3], "AND");
	makestring(txt[1][4], "GERWIN STURM A.K.A. THE BRAIN");
		
	makestring(txt[2][0], "CREDITS");

	makestring(txt[3][0], "IDEA AND PLANNING");
	makestring(txt[3][1], " ");
	makestring(txt[3][2], "EIKE J. UMLAUF");
	makestring(txt[3][3], "GERWIN STURM");
	
	makestring(txt[4][0], "PROGRAMMING");
	makestring(txt[4][1], " ");
	makestring(txt[4][2], "EIKZILLA");
	makestring(txt[4][3], "THE BRAIN");
	
	makestring(txt[5][0], "OBJECT HANDLING & 3D-MODEL PROGRAMMING");
	makestring(txt[5][1], " ");
	makestring(txt[5][2], "THE BRAIN");
	
	makestring(txt[6][0], "DESIGN, GRAPHICS, VISUAL FX & SOUND");
	makestring(txt[6][1], " ");
	makestring(txt[6][2], "EIKZILLA");
	
	makestring(txt[7][0], "3D MODELS");
	makestring(txt[7][1], " ");
	makestring(txt[7][2], "EIKZILLA");
	makestring(txt[7][3], "THE BRAIN");
	
	makestring(txt[8][0], "ENEMY AI");
	makestring(txt[8][1], " ");
	makestring(txt[8][2], "THE BRAIN");
	
	makestring(txt[9][0], "GRAPHICS PROGRAMMING, INTRO & CREDITS");
	makestring(txt[9][1], " ");
	makestring(txt[9][2], "EIKZILLA");
	
	makestring(txt[10][0], "ALL GRAPHICS, SOUND AND MODELS BY");
	makestring(txt[10][1], " ");
	makestring(txt[10][2], "EIKZILLA + THE BRAIN");

	makestring(txt[11][0], "EXCEPT");
	
	makestring(txt[12][0], "MENU MUSIC");
	makestring(txt[12][1], "UTAH SAINTS - WHAT CAN YOU DO FOR ME");
	makestring(txt[12][2], "(SALT LAKE MIX)");
	makestring(txt[12][3], " ");
	makestring(txt[12][4], "INTRO MUSIC");
	makestring(txt[12][5], "TURRICAN 2 THEME BY CHRIS HUELSBECK");
	makestring(txt[12][6], "TRACKED BY UNKNOWN, REMIXED BY EIKZILLA");
	makestring(txt[12][7], " ");
	makestring(txt[12][8], "OUTRO MUSIC");
	makestring(txt[12][9], "TURRICAN CREDITS BY CHRIS HUELSBECK");
	makestring(txt[12][10], " ");
	makestring(txt[12][11], "CREDITS MUSIC");
	makestring(txt[12][12], "APIDYA CREDITS BY CHRIS HUELSBECK");
	makestring(txt[12][13], " ");
	makestring(txt[12][14], "BOTH TAKEN FROM OFFICIAL SOUNDTRACK");

	makestring(txt[13][0], "BRIEFING MUSIC");
	makestring(txt[13][1], "'HUMAN TARGET' BY UNKNOWN");
	makestring(txt[13][2], " ");
	makestring(txt[13][3], "PONG! MUSIC");
	makestring(txt[13][4], "'WASSERFAST' BY MAHONEY & KAKTUS");
	makestring(txt[13][5], " ");
	makestring(txt[13][6], " ");
	makestring(txt[13][7], "SOUND FX RIPPED FROM");
	makestring(txt[13][8], " ");
	makestring(txt[13][9], "THE ABYSS MOTION PICTURE");
	makestring(txt[13][10], "DESCENT 2");
	makestring(txt[13][11], "TOURING CAR CHALLENGE 2");
	makestring(txt[13][12], "VARIOUS AMIGA GAMES");

	makestring(txt[14][0], "SOME METAL TEXTURES");
	makestring(txt[14][1], " ");
	makestring(txt[14][2], "SOMEWHERE OFF THE INTERNET");

	makestring(txt[15][0], "ALL DIGITALLY REMASTERED BY EIKZILLA");
	
	makestring(txt[16][0], "PERSONAL CREDITS AND GREETINGS");
	makestring(txt[16][1], "FROM GERWIN");
	makestring(txt[16][2], "GO TO");

	makestring(txt[17][0], "MY GIRLFRIEND MONIKA");
	makestring(txt[17][1], "FOR USEFUL ADVICE AND");
	makestring(txt[17][2], "SOMETIMES HAVING TO WAIT A LITTLE BIT");
	makestring(txt[17][3], "LONGER FOR ME TO COME TO BED");
	makestring(txt[17][4], " ");
	makestring(txt[17][5], "EIK FOR THE GREAT WORK HE DID");

	makestring(txt[18][0], "PERSONAL CREDITS AND GREETINGS");
	makestring(txt[18][1], "FROM EIK");
	makestring(txt[18][2], "GO TO");

	makestring(txt[19][0], "CG INSTITUTE FOR OFFERING THIS LAB");
	makestring(txt[19][1], " ");
	makestring(txt[19][2], "CG STAFF FOR INSTANT EMAIL-REPLIES");
	makestring(txt[19][3], " ");
	makestring(txt[19][4], "ALL THE GUYS ON THE PROGRAMMING");
	makestring(txt[19][5], "DISCUSSION FORUMS OUT THERE");
	
	makestring(txt[20][0], "CHRIS HUELSBECK");
	makestring(txt[20][1], "FOR THE FANTASTIC MUSIC");
	makestring(txt[20][2], " ");
	makestring(txt[20][3], "RAREWARE LTD");
	makestring(txt[20][4], "BEST EGOSHOOTER EVER (PERFECT DARK)");
	makestring(txt[20][5], "SERVED AS A GREAT INSPIRATIONAL SOURCE");
	makestring(txt[20][6], " ");
	makestring(txt[20][7], "WWW.FMOD.ORG");
	makestring(txt[20][8], "THE MOST ADVANCED SOUND LIBRARY THERE IS");

	makestring(txt[21][0], "BLK FOR BETATESTING");
	makestring(txt[21][1], " ");
	makestring(txt[21][2], "RAY FOR THE 3DSTUDIO BOOK");
	makestring(txt[21][3], "AND HELP ON GETTING STARTED WITH IT");
	makestring(txt[21][4], " ");
	makestring(txt[21][5], "MARTY FOR ASSISTANCE");
	makestring(txt[21][6], "ON THE HALO-WORKAROUND");
	makestring(txt[21][7], " ");
	makestring(txt[21][8], "POMMES FOR THE 'HERO OF THE MONTH'");
	makestring(txt[21][9], " ");
	makestring(txt[21][10], "MY SPECIALIZED-FSR MOUNTAINBIKE");
	makestring(txt[21][11], "FOR MUCH-NEEDED DIVERSITY");

	makestring(txt[22][0], "JAN & TINTI");
	makestring(txt[22][1], "FOR THEIR HELP ON THE BOEING PAREIL");
	makestring(txt[22][2], "(AHEM... ;-)");
	makestring(txt[22][3], " ");
	makestring(txt[22][4], "K2 FOR DRAGGING ME INTO LOTS OF PUBS");
	makestring(txt[22][5], " ");
	makestring(txt[22][6], "GERWIN FOR THE GREAT COOPERATION");
	makestring(txt[22][7], " ");
	makestring(txt[22][8], "AND EVERYONE ELSE WHO SUPPORTED ME...");
	makestring(txt[22][9], "...YOU KNOW WHO YOU ARE!");

	makestring(txt[23][0], "FAVOURITE DEVELOPMENT SAYINGS:");
	makestring(txt[23][1], " ");
	makestring(txt[23][2], "\"IRGENDWIE HAUT DES NED GANZ HIN...\"");
	makestring(txt[23][3], " ");
	makestring(txt[23][4], "\"ES GIBT WIEDERMAL EIN UPDATE\"");
	makestring(txt[23][5], " ");
	makestring(txt[23][6], "\"DER KREMPEL REIBT MA DAUERND AB!!\"");
	
	makestring(txt[24][0], "VISIT");
	makestring(txt[24][1], "HTTP://POSSIBLEWORLDS.CJB.NET");
	makestring(txt[24][2], "FOR UPDATES,");
	makestring(txt[24][3], "A POSSIBLE SEQUEL ;)");
	makestring(txt[24][4], "OR JUST FOR FUN");

	makestring(txt[25][0], "THANK YOU FOR PLAYING");

	makestring(txt[26][0], "THE END");


	ft[0] = 5.0; ft[1] = 7.0; ft[2]=6.0; ft[3]=6.0; ft[4] =6.0;
	ft[5] = 5.0; ft[6] = 5.0; ft[7] = 5.0; ft[8] = 5.0; ft[9] = 5.0;
	ft[10] = 4.6; ft[11]=3.0; ft[12]=12.0; ft[13] = 11.0; ft[14]=4.0; ft[15]=4.0;
	ft[16] = 4.5; ft[17] = 8.0; ft[18] = 4.5; ft[19] = 8.0; ft[20] = 8.0;
	ft[21] = 10.0; ft[22] = 10.0; ft[23] = 8.0; ft[24] = 8.0; ft[25] = 5.0; ft[26] = 5000.0;
	numsteps=27;
	sprintf(buf,"sfx/chris_huelsbeck-apidya_credits.mp3");
	loadmp3(buf);
	 
	tm = ft[0];
	prevtime = tm;
	vol=CREDVOL;

	write_log("done!\n");
}


outro::~outro()
{
	textureObj* tmp;
	int i,j;

	for(i=0; i<40; i++)
	{
		for(j=0; j<15; j++)
		{
			if(txt[i][j]!=NULL)
			{
				char* ctmp = txt[i][j];
				delete ctmp;
				txt[i][j]=NULL;
			}
		}
	}
	delete logo;
	delete logo2;

	for(i=0; i<4; i++)
	{
		tmp = layer[i];
		delete tmp;
	}

	for(i=0; i<12; i++)
	{
		tmp = bg[i];
		delete tmp;
	}

	write_log("Credits quitted\n");
}


void outro::initsound()
{
	// reserve channels for music mod
	
	// DEPRECATED

}

void outro::control()
{
	// read keyboard commands, control time and fading
	GLfloat litepos[] = { 0.2, 1.0, 0.4, 0.0 };
	int i;
	char* keys = getkeys();
	GLfloat ticks = gettime();

	kbd();
	this->tm-=ticks;
	
	glLightfv(GL_LIGHT0, GL_POSITION,litepos);


	if(keys[SDLK_ESCAPE] && quit==0) // quit credits
	{
		keys[SDLK_ESCAPE]=0;
		quit=1;
		tm=1.0/CREDITS_FADESPEED; prevtime=1.0/CREDITS_FADESPEED;
	}
	if(keys[SDLK_RETURN] && this->tm>0.0) // next slide
	{
		keys[SDLK_RETURN]=0;
		//this->tm=-0.1;
		//prevtime=0.1;
	}

	// control bg
	bgwait-=ticks;

	if(bgx>1.0 && bgwait>0) {bgx-=ticks*0.8; bgal+=ticks*0.076; }
	if(bgx<=1.0 && bgwait>0){ bgx=1.0; bgal=0.95; }

	if(bgwait<-12.0) { bgy+=ticks*0.5; bgal-=ticks*0.2; }
	if(bgwait<-20.0) { bgwait=15.0; bgy=1.0; bgx=10.0; bgal=0.0;}


	// control logo
	lpos+=ticks*lspd;
	lpos2+=ticks*lspd;

	if(lpos<-60) lpos = 25;
	if(lpos2<-60) lpos2=25;

	// control layers
	for(i=0; i<4; i++)
	{
		lx[i]+=ticks*xd[i];
		ly[i]+=ticks*yd[i];
		lz[i]+=ticks*zd[i];
		rot[i]+=ticks*rsp[i];

		// handle transparency
		lc[i][3]=ta[i];
		if(lz[i]<L_REAR+5) lc[i][3]=ta[i]-ta[i]*(fabs(L_REAR+5-lz[i])/7);
		if(lz[i]>L_FRONT) lc[i][3]=ta[i]-ta[i]*(fabs(L_FRONT-lz[i])/4);

		// set new starting coords for layer
		if(lz[i]>-1.5)
		{
			lc[i][3]=0.0;
			lx[i]=-30.0; ly[i]=-30.0; lz[i]=L_REAR-2-rand()/(float)RAND_MAX*1.5;
			xd[i] = rand()/(float)RAND_MAX*3.0-1.5;
			yd[i] = rand()/(float)RAND_MAX*3.0-1.5;
			zd[i] = rand()/(float)RAND_MAX*3.6+1.6;

			rot[i] = rand()/(float)RAND_MAX*360.0;
			rsp[i] = rand()/(float)RAND_MAX*60.0-30.0;
	
			if((rand()/(float)RAND_MAX)>0.5)
			{
				xsc[i]=1.0+rand()/(float)RAND_MAX*3.0+0.3;
				zsc[i]=1.0;
			}
			else
			{
				zsc[i]=1.0+rand()/(float)RAND_MAX*3.0+0.3;
				xsc[i]=1.0;
			}
		}
	}



	if(tm>0.0 && quit==0) // fade in
	{
		switch(step%2) // switch odd / even steps
		{
		case 0:	// even
			if(al1<1.0) {al1+=ticks*CREDITS_FADESPEED; xsc[4]=1.0; zsc[4]-=ticks*ZSCALE_SPEED;}
			if(al2>0.0) {al2-=ticks*CREDITS_FADESPEED; zsc[5]=1.0; xsc[5]+=ticks*XSCALE_SPEED;}
			break;
		case 1:	// odd
			if(al2<1.0) {al2+=ticks*CREDITS_FADESPEED; xsc[5]=1.0; zsc[5]-=ticks*ZSCALE_SPEED;}
			if(al1>0.0) {al1-=ticks*CREDITS_FADESPEED; zsc[4]=1.0; xsc[4]+=ticks*XSCALE_SPEED;}
			break;
		}
		for(i=4; i<6; i++)
		{
			if(xsc[i]<INNERX) xsc[i]=INNERX;
			if(xsc[i]>OUTERX) xsc[i]=OUTERX;
			if(zsc[i]<INNERZ) zsc[i]=INNERZ;
			if(zsc[i]>OUTERZ) zsc[i]=OUTERZ;
		}
	}
	// the break!
	if(prevtime>0.0 && tm<0.0)
	{
		if(quit==1) killme=1;
		step++;
		switch(step%2)
		{
		case 0:
			zsc[4]=OUTERZ; xsc[4]=OUTERX;
			zsc[5]=INNERZ; xsc[5]=INNERX;
			break;
		case 1:
			zsc[5]=OUTERZ; xsc[5]=OUTERX;
			zsc[4]=INNERZ; xsc[4]=INNERX;
			break;
		}
		if(step<numsteps) tm=this->ft[step];
		else {tm = 1.0/CREDITS_FADESPEED; quit=1; }
	}

	
	if(quit==1)
	{
		allal+=ticks*CREDITS_FADESPEED;
		al1-=ticks*CREDITS_FADESPEED;
		al2-=ticks*CREDITS_FADESPEED;
		vol-=CREDVOL*ticks*CMOD_FADESPEED;
		setmp3vol(vol);
	}
	else
	{
		if(allal>0.0)
		{
			allal-=ticks*CREDITS_FADESPEED;
		}
		else
			allal=0.0;
	}

	prevtime=tm;


}



void outro::draw()
{
	// display all text and gfx
	int thisstep, j;
	GLfloat al;
	int maxy=0;
	int ind, i,ucoord, vcoord, bgtx=0;
	
	// very simple gfx pipeline!
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	// first, draw bg
	glScalef(bgx, bgy, 1.0);
	glColor4f(0.4, 0.5, 1.0, bgal-allal);
	glEnable(GL_TEXTURE_2D);
	for(j=0;j<3;j++)
	{
		for(i=0; i<4; i++)
		{
			if(j==2) vcoord = -1; else vcoord=0;
			if(i==3) ucoord = 5; else ucoord=1;
			glBindTexture(GL_TEXTURE_2D, bg[bgtx]->id); bgtx++;
			glBegin(GL_QUADS);
				glTexCoord2i(0,1); glVertex3f(-1.0+(i*256)*0.0025, 0.8-(j*256)*0.0027, -1.5000001);
				glTexCoord2i(0,vcoord); glVertex3f(-1.0+(i*256)*0.0025, 0.8-((j+1)*256)*0.0027, -1.5000001);
				glTexCoord2i(ucoord,vcoord); glVertex3f(-1.0+((i+1)*256)*0.0025, 0.8-((j+1)*256)*0.0027, -1.5000001);
				glTexCoord2i(ucoord,1); glVertex3f(-1.0+((i+1)*256)*0.0025, 0.8-(j*256)*0.0027, -1.5000001);
			glEnd();
		}
	}
	glPopMatrix();


	// draw logo
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glPushMatrix();
		glScalef(1.0,1.0,1.0);
		glPushMatrix();
			glTranslatef(lpos,0.0,-7.0);
			glScalef(-1.0,1.0,-1.0);
			logo->draw();
		glPopMatrix();
		glTranslatef(lpos2,0.0,-7.0);
		glScalef(-1.0,1.0,-1.0);
		logo2->draw();
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


	// now draw layers
	for(i=0; i<4; i++)
	{
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glColor4f(lc[i][0], lc[i][1], lc[i][2], lc[i][3]-allal);
			glScalef(xsc[i], zsc[i], 1.0);
			glRotatef(rot[i],0.0, 0.0, 1.0);
			glTranslatef(lx[i], ly[i], lz[i]);
			glBindTexture(GL_TEXTURE_2D, layer[i]->id);
			glBegin(GL_QUADS);
				glTexCoord2i(0,0); glVertex3f(0.0, 0.0, -2.0);
				glTexCoord2i(0,30*ts[i]); glVertex3f(0.0, 60.0, -2.0);
				glTexCoord2i(30*ts[i],30*ts[i]); glVertex3f(60.0, 60.0, -2.0);
				glTexCoord2i(30*ts[i],0); glVertex3f(60.0, 0.0, -2.0);
			glEnd();
		glPopMatrix();
	}


	for(i=0; i<2; i++)
	{
		thisstep = step-1+i;
		if(thisstep>=0 && thisstep<numsteps)
		{
			// select alpha
			if(thisstep%2==0) { al = al1; ind=4;} else {al=al2; ind=5;}

			// check amount of strings to draw
			maxy=0;
			while(txt[thisstep][maxy]!=NULL && maxy<15)
				maxy++;

			// then the text
			for(j=0; j<maxy; j++)
			{
				if(txt[thisstep][j]!=NULL)
				{
					print(strlen(txt[thisstep][j])/2*-CHARLEN+400, 300, txt[thisstep][j], 1, 0.7, 0.83, 1.0, al);
			
					glPushMatrix();
						glTranslatef(0.0, 0.0+((maxy*15)-j*30)*0.0027, 0.0);
						glScalef(xsc[ind],zsc[ind],1.0);
						glTranslatef(0.0, -14*0.0027, 0.0);
						drawstrings();
					glPopMatrix();

				}
			}
		}
	}
	
	// the usual suspects
	glFlush();
	SDL_GL_SwapBuffers();


}


void outro::run()
{
	// the sequence main loop
	timing();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_FOG);
	playmp3(vol);

	while(killme==0)
	{
		timing();
		
		control();
		draw();
	}

	stopmp3();
	glEnable(GL_FOG);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
}

