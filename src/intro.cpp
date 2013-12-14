// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "intro.h"
#include "psworlds.h"
#include "sound.h"
#include "menu.h"
#include "fonts.h"
#include "utility.h"

#define INTRO_FADESPEED 0.75
#define MOD_FADESPEED INTRO_FADESPEED

#define INTROVOL 136
#define MP3VOL 255

intro::intro (int mode)
{
  _FBUF

  write_log ("Creating intro...");

  for (int i=0;i<20;i++){ img[i]=NULL; for (int j=0; j<10; j++) { fo[i][j]=2; txt[i][j]=NULL; }}
  al1=0.0;
  al2=0.0;
  this->timeleft=0.0;
  step=0;
  numsteps=0;
  killme=0;
  quit=0;
  vol=INTROVOL;
  this->mode = mode;
  chl[1] = CHARLEN;
  chl[2] = CHARLEN/4*3;

  // script intro and outro sequences
  switch (mode)
  {
  case INTRO:
    sprintf (buf,"gfx/intro/bush-intro.tga"); img[2]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/bill-intro.tga"); img[3]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/bill-intro.tga"); img[4]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/lawandorder.tga"); img[5]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/guilty.tga"); img[6]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/attack.tga"); img[7]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/nato.tga"); img[8]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/nato.tga"); img[9]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/ues.tga"); img[10]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/economy.tga"); img[11]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/news.tga"); img[12]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/news.tga"); img[13]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/scientists.tga"); img[14]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/spy.tga"); img[15]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/raceison.tga"); img[16]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/raceison.tga"); img[17]=new textureObj (buf,1);



    makestring (txt[0][0], "PROLOGUE"); fo[0][0]=1;

    makestring (txt[1][1], "2000 A.D."); fo[1][1]=1;

    makestring (txt[2][0], "GEORGE W. BUSH ACQUIRES");
    makestring (txt[2][1], "THE U.S. PRESIDENT'S CHAIR");

    makestring (txt[3][0], "JUNE 2004"); fo[3][0] = 1;
    makestring (txt[3][2], "THE MICROSOFT TRIAL, FURTHER FORCED BY THE AMERICAN");
    makestring (txt[3][3], "LAW & ORDER LOBBY AND THE FEDERAL LAWYER ASSOCIATION,");
    makestring (txt[3][4], "COMES TO AN END.");

    makestring (txt[4][1], "AS A RESULT, MICROSOFT IS JUDGED TO OPEN-SOURCE ITS");
    makestring (txt[4][2], "WHOLE WINDOWS PRODUCT LINE.");

    makestring (txt[5][0], "JULY 2004"); fo[5][0]=1;
    makestring (txt[5][2], "NETSCAPE / AOL, SUN MICROSYSTEMS, APPLE AND MANY OTHER");
    makestring (txt[5][3], "COMPANIES SUE MICROSOFT BECAUSE OF VARIOUS PATENT");
    makestring (txt[5][4], "PENDING VIOLATIONS, UNFAIR COMPETITION, AND 104 OTHER");
    makestring (txt[5][5], "POINTS OF INDICTMENT.");

    makestring (txt[6][0], "SEPTEMBER 2005"); fo[6][0]=1;
    makestring (txt[6][2], "U.S. COURTS JUDGE MICROSOFT GUILTY.");
    makestring (txt[6][3], "THE AMERICAN ECONOMY, COMPLETELY GEARED TOWARDS I.T. AND");
    makestring (txt[6][4], "SOMEWHAT DIMINISHED BY GEORGE W. BUSH'S LEADERSHIP, COLLAPSES.");

    makestring (txt[7][0], "IN A DESPERATE MOVE TO TURN ATTENTION AWAY FROM");
    makestring (txt[7][1], "INTERNAL PROBLEMS, THE U.S. MILITARY ATTACKS TARGETS");
    makestring (txt[7][2], "IN EX-YUGOSLAWIA IN ORDER TO \"PREVENT TERRORISTIC ACTIONS");
    makestring (txt[7][3], "OF ISLAMIC FANATICS THAT ARE A SERIOUS THREAT TO NATIONAL");
    makestring (txt[7][4], "SECURITY\", A RATHER CLEVER TECHNIQUE THAT WAS");
    makestring (txt[7][5], "SUCCESSFULLY DEMONSTRATED BY PREDECESSOR BILL CLINTON.");
    
    makestring (txt[8][0], "THE RATHER CLEVER TECHNIQUE DOES NOT DO THE TRICK,");
    makestring (txt[8][1], "THE RELATIONSHIP USA - EUROPE DETERIORATES.");

    makestring (txt[9][0], "OCTOBER 2006"); fo[9][0]=1;
    makestring (txt[9][2], "NATO IS DISBANDED DUE TO INTERNAL DISCREPANCIES.");

    makestring (txt[10][0], "APRIL 2007"); fo[10][0]=1;
    makestring (txt[10][2], "ONGOING ECONOMY CRISIS IN THE U.S.A., MUTUAL MILITARY");
    makestring (txt[10][3], "INTERESTS INTEGRATED INTO THE E.U., WHICH IS EXPANDED AND");
    makestring (txt[10][4], "RENAMED TO \"UNION OF EUROPEAN STATES\" (U.E.S.)");

    makestring (txt[11][0], "MID - 2009"); fo[11][0]=1;
    makestring (txt[11][2], "CURRENT STATE OF THE WORLD:");
    makestring (txt[11][3], "UES AND USA APPROXIMATELY EQUALLY STRONG,");
    makestring (txt[11][4], "ECONOMICALLY-WISE.");

    makestring (txt[12][0], "MARCH 12TH, 2011"); fo[12][0]=1;
    makestring (txt[12][2], "AN UNIDENTIFIED FLYING OBJECT CRASHES INTO THE ATLANTIC");
    makestring (txt[12][3], "OCEAN. BRITISH AND AMERICAN NAVY FORCES RACE TO");
    makestring (txt[12][4], "RECOVER THE REMAINS OF THE UFO.");

    makestring (txt[13][0], "BOTH SIDES OBTAIN PARTS OF THE EXTRATERRESTRIAN WRECK.");
    makestring (txt[13][1], "IT IS THE FIRST TIME IN HISTORY THAT A DISCOVERY");
    makestring (txt[13][2], "LIKE THIS IS ADMITTED BY GOVERNMENTS AND OFFICIAL");
    makestring (txt[13][3], "FOOTAGE IS MADE AVAILABLE.");

    makestring (txt[14][0], "AUGUST 2011"); fo[14][0]=1;
    makestring (txt[14][2], "SCIENTISTS MANGAGE TO DECIPHER SOME OF THE UFO'S");
    makestring (txt[14][3], "STORED DATA. RECORDINGS DESCRIBE 4 LOCATIONS OF");
    makestring (txt[14][4], "UNMANNED EXTRATERRESTRIAN OBSERVATORY STATIONS");
    makestring (txt[14][5], "LOCATED ON THE SEAFLOOR. (EXTRATERRESTRIAN SEAFLOOR");
    makestring (txt[14][6], "STATIONS, E.S.S)");

    makestring (txt[15][0], "DUE TO AN UNPREDICTABLE ESPIONAGE INCIDENT");
    makestring (txt[15][1], "THAT INVOLVES A MINIVAN, AN ELECTRIC TOOTHBRUSH");
    makestring (txt[15][2], "AND A PROTOTYPE OF A NUCLEAR MICROWAVE OVEN,");
    makestring (txt[15][3], "THE USA AND THE UES BOTH GET HOLD OF THIS");
    makestring (txt[15][4], "INFORMATION.");

    makestring (txt[16][0], "SEPTEMBER 2011"); fo[16][0]=1;
    makestring (txt[16][2], "EXPERTS AND SECRET SERVICE PERSONNEL CONCLUDE:");
    makestring (txt[16][3], "THE ALIEN TECHNOLOGY THAT IS SUSPECTED IN THE ESS'S");
    makestring (txt[16][4], "WILL GUARANTEE WHOEVER DISCOVERS AND EVALUATES IT");
    makestring (txt[16][5], "IN THE FIRST PLACE ECONOMICAL AND MILITARY");
    makestring (txt[16][6], "LEADERSHIP FOR YEARS TO COME.");

    makestring (txt[17][0], "THE RACE FOR WHICH SIDE WILL"); fo[17][0]=1;
    makestring (txt[17][1], "BECOME THE NEXT SUPERPOWER IS ON!"); fo[17][1]=1;



    tm[0] = 5.0; tm[1] = 5.0; tm[2] = 8.0; tm[3]=11; tm[4] = 7; tm[5]=14.0; tm[6]=14.0;
    tm[7] = 19.0; tm[8] = 7.0; tm[9] = 8.0; tm[10] = 12.0; tm[11] = 11.0;
    tm[12] = 13.0; tm[13] = 13.0; tm[14] = 18.0; tm[15] = 15.0; tm[16] = 17.0; tm[17]=9.0;
    numsteps=18;

    beginvol = INTROVOL;
    musicfadespeed = MOD_FADESPEED;
    sprintf (buf,"sfx/turrican2_(eik_remix).mod");
    loadmusic (buf); setmusicvolume (beginvol);
    break;
  case OUTRO:
    sprintf (buf,"gfx/intro/openess.tga"); img[2]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/amazed.tga"); img[3]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/amazed.tga"); img[4]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/ontheweb.tga"); img[5]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/newspaper1.tga"); img[7]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/news2.tga"); img[8]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/news3.tga"); img[6]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/believe.tga"); img[9]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/futurespace.tga"); img[10]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/alienplanet.tga"); img[11]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/theend.tga"); img[12]=new textureObj (buf,1);
    sprintf (buf,"gfx/intro/theend2.tga"); img[13]=new textureObj (buf,1);
    

    makestring (txt[0][0], "EPILOGUE"); fo[0][0]=1;

    makestring (txt[1][1], "FEBRUARY 23RD, 2012"); fo[1][1]=1;

    makestring (txt[2][0], "U.E.S SCIENTISTS OPEN THE CAPTURED ESS TO");
    makestring (txt[2][1], "GET ACCESS TO THE ALIEN TECHNOLOGY INSIDE.");

    makestring (txt[3][0], "THE RESULTS ARE FAR BEYOND EXPECTATION:");
    makestring (txt[3][1], "THE ESS SEEMS TO BE MORE A WAY FOR DELIVERING");
    makestring (txt[3][2], "A MESSAGE FROM THE ALIEN RACE TO MANKIND THAN");
    makestring (txt[3][3], "AN OBSERVATORY STATION.");

    makestring (txt[4][0], "NOT ONLY DOES IT INCLUDE BLUEPRINTS OF THE");
    makestring (txt[4][1], "ALIEN ANATOMY, BUT ALSO INFORMATION ABOUT THE");
    makestring (txt[4][2], "LOCATION OF THE DISTANT SOLAR SYSTEM INCLUDING");
    makestring (txt[4][3], "ECONOMICAL, ENVIRONMENTAL AND SOCIAL ISSUES, AS");
    makestring (txt[4][4], "WELL AS A WEALTH OF DOCUMENTATION AND EXAMPLES");
    makestring (txt[4][5], "OF THEIR TECHNICAL SUPREMACY.");

    makestring (txt[5][0], "MARCH 4TH, 2012"); fo[5][0]=1;
    makestring (txt[5][2], "THE RESPONSIBLE U.E.S SCIENTISTS, AMAZED BY THE");
    makestring (txt[5][3], "SHEER SUPERIORITY OF THE ALIEN RACE, PUBLISH ALL");
    makestring (txt[5][4], "THE GATHERED INFORMATION ON THE INTERNET.");
    
    makestring (txt[6][0], "ALTHOUGH SECRET SERVICE AND THE GOVERNMENT");
    makestring (txt[6][1], "DESPERATELY ATTEMPT TO WITHHOLD THE VALUABLE");
    makestring (txt[6][2], "DOCUMENTS, IT'S TOO LATE FOR THEM.");
    makestring (txt[6][3], "THE WEBSERVERS PROVIDING THE ALIEN INFORMATION ARE");
    makestring (txt[6][4], "CONSTANTLY HEAVILY OVERLOADED, THE MEDIA ARE");
    makestring (txt[6][5], "FULL OF RELATED CONTENT.");

    makestring (txt[7][0], "ALTHOUGH SECRET SERVICE AND THE GOVERNMENT");
    makestring (txt[7][1], "DESPERATELY ATTEMPT TO WITHHOLD THE VALUABLE");
    makestring (txt[7][2], "DOCUMENTS, IT'S TOO LATE FOR THEM.");
    makestring (txt[7][3], "THE WEBSERVERS PROVIDING THE ALIEN INFORMATION ARE");
    makestring (txt[7][4], "CONSTANTLY HEAVILY OVERLOADED, THE MEDIA ARE");
    makestring (txt[7][5], "FULL OF RELATED CONTENT.");

    makestring (txt[8][0], "ALTHOUGH SECRET SERVICE AND THE GOVERNMENT");
    makestring (txt[8][1], "DESPERATELY ATTEMPT TO WITHHOLD THE VALUABLE");
    makestring (txt[8][2], "DOCUMENTS, IT'S TOO LATE FOR THEM.");
    makestring (txt[8][3], "THE WEBSERVERS PROVIDING THE ALIEN INFORMATION ARE");
    makestring (txt[8][4], "CONSTANTLY HEAVILY OVERLOADED, THE MEDIA ARE");
    makestring (txt[8][5], "FULL OF RELATED CONTENT.");

    makestring (txt[9][0], "THE CONFLICT HAS BECOME OBSOLETE, THE WHOLE WORLD");
    makestring (txt[9][1], "NOW HAS ACCESS TO THE TOP SECRET MATERIAL.");
    makestring (txt[9][2], "MANKIND IS OVERWHELMED BY THE ADVANCED TECHNOLOGY");
    makestring (txt[9][3], "AND THE DETAILED DESCRIPTIONS OF THE ALIEN CULTURE.");
    makestring (txt[9][4], "EVERYTHING HAS A NEW MEANING, PEOPLE ARE EMBRACED");
    makestring (txt[9][5], "BY A NEW, OPEN-MINDED SPIRIT.");

    makestring (txt[10][0], "MAY 12TH, 2012"); fo[10][0]=1;
    makestring (txt[10][2], "AFTER WEEKS OF DEBATE, THE U.S.A AND THE U.E.S");
    makestring (txt[10][3], "FINALLY SET AN END TO THE CONFLICT AND LAUNCH");
    makestring (txt[10][4], "AN IMPRESSIVELY AMBITIOUS JOINT SPACE PROGRAM.");
    
    makestring (txt[11][0], "THE GOAL IS TO REPRODUCE PARTS OF THE ALIEN");
    makestring (txt[11][1], "TECHNOLOGY TO BE ABLE TO MAKE CONTACT WITH");
    makestring (txt[11][2], "THE EXTRATERRESTRIAN RACE SOMEDAY.");

    makestring (txt[12][0], "IS THIS JUST A PLATFORM FOR A SEQUEL TO");
    makestring (txt[12][1], "THIS GAME, OR COULD THIS FINALLY MEAN TRUE");
    makestring (txt[12][2], "PEACE AND AN END TO ALL CONFLICTS ON EARTH?");

    makestring (txt[13][0], "WHO KNOWS..");
    makestring (txt[13][1], "THERE'S MORE THAN ONE");
    makestring (txt[13][3], "POSSIBLE WORLD"); fo[13][3]=1;
  

    tm[0] = 5.0; tm[1] = 5.0; tm[2] = 8.0; tm[3]=12; tm[4] = 15.0; tm[5]=12.0; tm[6]=11.0;
    tm[7] = 10.0; tm[8] = 9.0;
    tm[9] = 17.0; tm[10] = 13.0; tm[11] = 11.0; tm[12] = 10.0; tm[13] = 10.0;
    numsteps=14;

    beginvol = MP3VOL;
    musicfadespeed = MOD_FADESPEED;
    sprintf (buf,"sfx/chris_huelsbeck-turrican_credits.mp3");
    loadmusic (buf);
    break;
  }

  timeleft = tm[0];
  prevtime = timeleft;

  initsound ();
  vol = beginvol;
  write_log ("done!\n");
}

intro::~intro ()
{
  for (int i=0; i<20; i++)
  {
    for (int j=0; j<10; j++)
    {
      if (txt[i][j]!=NULL)
      {
        char* ctmp = txt[i][j];
        delete ctmp;
        txt[i][j]=NULL;
      }
    }
    if (img[i]!=NULL)
    {
      textureObj* tmp = img[i];
      delete tmp;
      img[i]=NULL;
    }
  }
  write_log ("Intro quitted\n");
}


void intro::initsound ()
{

}


void intro::control ()
{
  // read keyboard commands, control time and fading
  kbd ();
  char* keys = getkeys ();
  GLfloat ticks = gettime ();

  this->timeleft-=ticks;
  

  if (keys[SDLK_ESCAPE] && quit==0) // quit intro
  {
    keys[SDLK_ESCAPE]=0;
    quit=1;
    timeleft=1.0/INTRO_FADESPEED; prevtime=1.0/INTRO_FADESPEED;
  }
  if (keys[SDLK_RETURN] && this->timeleft>0.0) // next slide
  {
    keys[SDLK_RETURN]=0;
    this->timeleft=-0.1;
    prevtime=0.1;
  }

  if (timeleft>0.0 && quit==0) // fade in pic!
  {
    switch (step%2) // switch odd / even steps
    {
    case 0:  // even
      if (al1<1.0) al1+=ticks*INTRO_FADESPEED;
      if (al2>0.0) al2-=ticks*INTRO_FADESPEED;
      break;
    case 1:  // odd
      if (al2<1.0) al2+=ticks*INTRO_FADESPEED;
      if (al1>0.0) al1-=ticks*INTRO_FADESPEED;
      break;
    }
  }
  // the break!
  if (prevtime>0.0 && timeleft<0.0)
  {
    if (quit==1) killme=1;
    step++;
    if (step<numsteps) timeleft=this->tm[step];
    else {timeleft = 1.0/INTRO_FADESPEED; quit=1; }
  }

  if (timeleft<0.0 && quit==0) // fade out pic!
  {
    switch (step%2) // switch odd / even steps
    {
    case 0:  // even
      if (al2<1.0) al2+=ticks*INTRO_FADESPEED;
      if (al1>0.0) al1-=ticks*INTRO_FADESPEED;
      break;
    case 1:  // odd
      if (al1<1.0) al1+=ticks*INTRO_FADESPEED;
      if (al2>0.0) al2-=ticks*INTRO_FADESPEED;
      break;
    }
  }
  if (quit==1)
  {
    al1-=ticks*INTRO_FADESPEED;
    al2-=ticks*INTRO_FADESPEED;
    vol-=beginvol*ticks*musicfadespeed;
    setmusicvolume (vol);
  }

  prevtime=timeleft;

}


void intro::display ()
{
  // display all text and gfx
  int thisstep, j;
  GLfloat alpha;

  // very simple gfx pipeline!
  glEnable (GL_TEXTURE_2D);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glClearColor (0, 0, 0, 0);
  glClear (GL_COLOR_BUFFER_BIT);
  glPushMatrix ();

  for (int i=0; i<2; i++)
  {
    thisstep = step-1+i;
    if (thisstep>=0 && thisstep<numsteps)
    {
      // draw the picture
      if (thisstep%2==0) alpha = al1; else alpha=al2;
      if (img[thisstep]!=NULL)
      {
        glBindTexture (GL_TEXTURE_2D, img[thisstep]->id);
        glColor4f (1.0, 1.0, 1.0, alpha);
        glBegin (GL_QUADS);
          glTexCoord2f (0.0,1.0); glVertex3f (-128*0.0025, 266*0.0027, -1.5000001);
          glTexCoord2i (0.0,0.0); glVertex3f (-128*0.0025, 10*0.0027, -1.5000001);
          glTexCoord2i (1.0,0.0); glVertex3f (128*0.0025, 10*0.0027, -1.5000001);
          glTexCoord2i (1.0,1.0); glVertex3f (128*0.0025, 266*0.0027, -1.5000001);
        glEnd ();
      }
      // then the text
      for (j=0; j<10; j++)
      {
        if (txt[thisstep][j]!=NULL)
          print (strlen (txt[thisstep][j])/2*-chl[fo[thisstep][j]]+400, 340+j*30, txt[thisstep][j], fo[thisstep][j], 0.5, 0.63, 1.0, alpha);
      }
    }
  }
  glPopMatrix ();
  drawstrings ();

  // the usual suspects
  glFlush ();
  SDL_GL_SwapBuffers ();
}


void intro::run ()
{
  // the sequence main loop
  timing ();
  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_SRC_ALPHA,GL_ONE);
  glEnable (GL_TEXTURE_2D);
  glDisable (GL_COLOR_MATERIAL);
  playmusic ();
  
  while (killme==0)
  {
    timing ();
    
    control ();
    display ();
  }

  stopmusic ();

  glEnable (GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable (GL_TEXTURE_2D);
  
}


