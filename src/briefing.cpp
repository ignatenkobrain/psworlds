// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "briefing.h"
#include "sound.h"
#include "fonts.h"

#define BRIEFVOL 155

briefing::briefing (int mnr)
{
  // load briefing
  _FBUF
  char line[255];
  int i=0;

  for (i=0; i<50; i++)
  {
    txt[i]=NULL;
  }
  this->mnr = mnr;
  killme=0;
  vol=BRIEFVOL; voladjust=0.0;


  // load bg image
  sprintf (buf,"gfx/credlayer4.tga");this->bg = new textureObj (buf,1);

  // set vars
  this->txtalpha = 0.0;
  this->txtalphac = 0.6;
  this->tc = 0.0;

  xscaler = 3.0;
  zscaler = 4.0;
  xsc = -1.5;
  zsc = -1.5*4/3;


  // read briefingfile
  sprintf (buf,"missions/briefing%d.brf",mnr);
  xpath (buf);
  FILE* fp = fopen (buf,"r");

  i=0;
  while ((line == fgets (line, sizeof (line), fp))!=NULL)
  {
    // read file
    if (i==0) // special: Mission type
    {
      xl[0] = 20; yl[0]=40; makestring (txt[0],"MISSION TYPE:");
      xl[1] = 200; yl[1]=40; makestring (txt[1],line);
    }
    else if (i==1) // special: location
    {
      xl[2] = 20; yl[2]=55; makestring (txt[2],"LOCATION:");
      xl[3] = 200; yl[3]=55; makestring (txt[3],line);
    }
    else if (i==2) // special: depth
    {
      xl[4] = 20; yl[4]=70; makestring (txt[4],"DEPTH:");
      xl[5] = 200; yl[5]=70; makestring (txt[5],line);
    }
    else
    {
      xl[i+3] = 20; yl[i+3] = 95+(i-3)*14; makestring (txt[i+3], line);
    }
    i++;
  }
  fclose (fp);

  sprintf (buf,"sfx/briefingmod.mod"); loadmusic (buf); 

}

briefing::~briefing ()
{
  _FBUF
  char* tmp;
  for (int i=0; i<50; i++)
  {
    if (txt[i]!=NULL)
    {
      tmp = txt[i]; delete tmp;
      txt[i]=NULL;
    }
  }
  delete bg;

  sprintf (buf,"sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
}


void briefing::control ()
{
  // control the stuff
  kbd ();
  char* keys = getkeys ();

  // textures first
  tc+=gettime ()*0.7;
  vol+=gettime ()*voladjust;
  
  txtalpha+=gettime ()*this->txtalphac;
  xscaler+=gettime ()*this->xsc;
  zscaler+=gettime ()*this->zsc;

  if (txtalpha>1.0)
  {
    txtalpha = 1.0;
    txtalphac=0.0;
  }
  if (xscaler<1.0)
  {
    xscaler=1.0;
    xsc=0.0;
  }
  if (zscaler<1.0)
  {
    zscaler=1.0;
    zsc=0.0;
  }


  if (txtalpha<-0.5) killme=1;

  // quit !
  if (((keys[SDLK_ESCAPE] || keys[SDLK_RETURN])) && (txtalpha==1.0))
  {
    txtalphac=-0.6; voladjust=txtalphac*vol;
    xsc = 1.5; zsc=1.5*4/3;
    keys[SDLK_RETURN] = 0;
    keys[SDLK_ESCAPE] = 0;
  }

  setmusicvolume (vol);
}

void briefing::draw ()
{
  int i;
  
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glClearColor (0, 0, 0, 0);
  glClear (GL_COLOR_BUFFER_BIT);
  glPushMatrix ();
  glEnable (GL_TEXTURE_2D);

  glPushMatrix ();
    glScalef (xscaler, 1.0, zscaler);
    glTranslatef (0.0, 0.0, -48.4);
    // draw tunnel
    glBindTexture (GL_TEXTURE_2D, bg->id);
    glBegin (GL_QUADS);
      
    glColor4f (0.9,0.4,0.0,txtalpha/1.9);
    glTexCoord2f (-5.0, 0.0+tc+0.4);
    glVertex3f (-20.0, 2.0, 100);
    glTexCoord2f (-5.0, 50.0+tc+0.4);
    glVertex3f (-20.0, 2.0, -100.0);
    glTexCoord2f (5.0, 50.0+tc+0.4);
    glVertex3f (20.0, 2.0, -100.0);
    glTexCoord2f (5.0, 0.0+tc+0.4);
    glVertex3f (20.0, 2.0, 100);
  
    glColor4f (0.1, 0.7, 0.1, txtalpha/1.9);
    glTexCoord2f (-5.0, 0.0+tc+0.4);
    glVertex3f (-20.0, -2.0, 100.0);
    glTexCoord2f (-5.0, 50.0+tc+0.4);
    glVertex3f (-20.0, -2.0, -100.0);
    glTexCoord2f (5.0, 50.0+tc+0.4);
    glVertex3f (20.0, -2.0, -100.0);
    glTexCoord2f (5.0, 0.0+tc+0.4);
    glVertex3f (20.0, -2.0, 100.0);
    glEnd ();

  glPopMatrix ();
  glPushMatrix ();
    glScalef (1.0, 1.0, zscaler);
    glTranslatef (0.0, 0.0, -48.4);

    glBegin (GL_QUADS);
    glColor4f (0.4,0.6,0.9,txtalpha/1.0);
    glTexCoord2f (-5.0, 0.0+tc);
    glVertex3f (-20.0, 1.0, 100);
    glTexCoord2f (-5.0, 50.0+tc);
    glVertex3f (-20.0, 1.0, -100.0);
    glTexCoord2f (5.0, 50.0+tc);
    glVertex3f (20.0, 1.0, -100.0);
    glTexCoord2f (5.0, 0.0+tc);
    glVertex3f (20.0, 1.0, 100);

    glColor4f (0.1, 0.3, 0.9, txtalpha/1.0);
    glTexCoord2f (-5.0, 0.0+tc);
    glVertex3f (-20.0, -1.0, 100);
    glTexCoord2f (-5.0, 50.0+tc);
    glVertex3f (-20.0, -1.0, -100.0);
    glTexCoord2f (5.0, 50.0+tc);
    glVertex3f (20.0, -1.0, -100.0);
    glTexCoord2f (5.0, 0.0+tc);
    glVertex3f (20.0, -1.0, 100);
    glEnd ();

  glPopMatrix ();

  glPushMatrix ();
    glScalef (xscaler, 1.0, 1.0);
    glDisable (GL_TEXTURE_2D);
    glColor4f (0.1, 1.0, 0.0, txtalpha/1.1);
    glBegin (GL_LINES);
      glVertex3f (-0.96,0.615,-1.5000001);
      glVertex3f (0.96,0.615,-1.5000001);
    glEnd ();

    for (i=0; i<50; i++)
    {
      if (txt[i]!=NULL)
      {
        print (xl[i], yl[i],txt[i],0,0.1,1.0,0.0,txtalpha/1.1);
      }
    }

    drawstrings ();
  glPopMatrix ();

  glPopMatrix ();

  // the usual suspects
  glFlush ();
  SDL_GL_SwapWindow (getWindow ());
}


void briefing::run ()
{
  // the sequence main loop
  GLfloat fogcol[] = { 0.0, 0.0, 0.0, 1.0 };

  timing ();
  timing ();
  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_SRC_ALPHA,GL_ONE);
  glEnable (GL_TEXTURE_2D);
  glDisable (GL_COLOR_MATERIAL);
  
  glEnable (GL_FOG);
  glFogfv (GL_FOG_COLOR, fogcol);
  glFogf (GL_FOG_DENSITY, 0.1);
  glHint (GL_FOG_HINT, GL_NICEST);
  glFogf (GL_FOG_START, 50.0);
  glFogf (GL_FOG_END, 200.0);
  
  setmusicvolume (BRIEFVOL);
  playmusic ();

  while (killme==0)
  {
    timing ();
    
    control ();
    draw ();
  }

  stopmusic ();
  glFogf (GL_FOG_DENSITY, 0.1);
  glHint (GL_FOG_HINT, GL_NICEST);
  glFogf (GL_FOG_START, 250.0);
  glFogf (GL_FOG_END, 720.0);
  glEnable (GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable (GL_TEXTURE_2D);
  setmusicvolume (255);
  
}

