// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "menu.h"
#include "fonts.h"
#include "mission.h"
#include "psworlds.h"
#include "sound.h"
#include "outro.h"
#include "intro.h"

entry::entry (int nr, const char *text, int x, int y, GLfloat *cols, int selectable, void* men)
{
  // create a menu entry
  num = nr;
  col[0] = cols[0]; col[1] = cols[1]; col[2] = cols[2]; col[3] = cols[3]; 
  this->x = x; this->y = y;
  this->selectable = selectable;
  host = (menu *) men;

  txt = (char *) malloc (strlen (text) + 1);
  strcpy (this->txt, text);
}

entry::~entry ()
{
  if (txt!=NULL) free (txt);
}



void entry::draw ()
{
  // draw a menu entry
  GLfloat* cl;

  if (((menu*)host)->selected == this->num) cl = ((menu*)host)->selcol;
  else cl = this->col;

  print (x,y,txt,1,cl[0], cl[1], cl[2], cl[3]);
}


GLfloat entry::gety ()
{
  return y;
}


menu::menu (int type, menu *parent, const char *caption)
{
  // constructor
  int i=0;
  int j=0;
  int l;
  GLfloat mc[4] = { 0.2, 1.0, 0.3, 1.0 };  
  numentries=0;

  killme=0;
  new_gs = 0;
  blink = 1.3;
  child = NULL;
  this->type = type;
  this->par = parent;
  FILE* fpeek;

  _FBUF

  if (strlen (caption)!=0) numentries++;

  switch (type)
  {
  case MENU_TITLE: // create the title screen
    // title caption
    playmusic ();
    entries[numentries] = new entry (numentries, "POSSIBLE WORLDS", strlen ("POSSIBLE WORLDS")/2*-CHARLEN+400, 140, mc, 0, (void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "NEW GAME", strlen ("NEW GAME")/2*-CHARLEN+400, 220,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "LOAD GAME", strlen ("LOAD GAME")/2*-CHARLEN+400, 260,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "WATCH INTRO", strlen ("WATCH INTRO")/2*-CHARLEN+400, 300,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "CREDITS", strlen ("CREDITS")/2*-CHARLEN+400, 340,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "QUIT", strlen ("QUIT")/2*-CHARLEN+400, 380,mc,1,(void*)this);
    numentries++;
    break;
  case MENU_GAME: // create the game screen
    // title caption
    playmusic ();
    l=180;
    entries[numentries] = new entry (numentries, "GAME MENU", strlen ("GAME MENU")/2*-CHARLEN+400, 140, mc, 0, (void*)this);
    numentries++;
    if (getMissionAdvance ()==getNumMission ())
    {
      entries[numentries] = new entry (numentries, "WATCH OUTRO", strlen ("WATCH OUTRO")/2*-CHARLEN+400, l+=40,mc,1,(void*)this);
    }
    else
    {
      entries[numentries] = new entry (numentries, "", strlen ("")/2*-CHARLEN+400, l,mc,0,(void*)this);
    }
    numentries++;
    entries[numentries] = new entry (numentries, "START MISSION", strlen ("START MISSION")/2*-CHARLEN+400, l+=40,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "SELECT MISSION", strlen ("SELECT MISSION")/2*-CHARLEN+400, l+=40,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "SAVE GAME", strlen ("SAVE GAME")/2*-CHARLEN+400, l+=40,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "QUIT GAME", strlen ("QUIT GAME")/2*-CHARLEN+400, l+=40,mc,1,(void*)this);
    numentries++;
    break;
  case MENU_INGAME: // create the game screen
    // title caption
    playmusic ();
    sprintf (buf,"missions/mission%d.txt",getMissionNum ());
    xpath (buf);
    fpeek = fopen (buf,"r");
    fgets (buf, 100, fpeek);
    fclose (fpeek);
    char buf2[255];
    sprintf (buf2,"MISSION %d: %s",getMissionNum (),buf);
    entries[numentries] = new entry (numentries, buf2, strlen (buf2)/2*-CHARLEN+400, 140, mc, 0, (void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "RETURN TO GAME", strlen ("RETURN TO GAME")/2*-CHARLEN+400, 200,mc,1,(void*)this);
    numentries++;
    entries[numentries] = new entry (numentries, "ABORT MISSION", strlen ("ABORT MISSION")/2*-CHARLEN+400, 240,mc,1,(void*)this);
    numentries++;
    // easteregg!!!
    if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
      if (((mission*)getmission ())->getCockpit ()->getpongable ()==1)
      {
        entries[numentries] = new entry (numentries, "PONG !", strlen ("PONG !")/2*-CHARLEN+400, 280,mc,1,(void*)this);
        numentries++;
      }
    break;
  case MENU_YESNO: // create a simple "yes / no" menu
    playmusic ();
    if (numentries!=0)
    {
      // create caption
      entries[numentries-1] = new entry (numentries-1, caption, strlen (caption)/2*-CHARLEN+400, 250, mc, 0, (void*)this);
    }
    // create yes
    numentries++;
    entries[numentries-1] = new entry (numentries-1, "YES", -3/2*CHARLEN+400, 285, mc, 1, (void*)this);
    // create noooo
    numentries++;
    entries[numentries-1] = new entry (numentries-1, "NO", -CHARLEN+400, 315, mc, 1, (void*)this);
    break;
  case MENU_SELMISSION: // create the mission selector
    if (numentries!=0)
    {
      // create caption
      entries[numentries-1] = new entry (numentries-1, caption, strlen (caption)/2*-CHARLEN+400, 180, mc, 0, (void*)this);
    }
    // create mission entries
    for (i=0; (i<getMissionAdvance ()+1)&&(i<getNumMission ()); i++)
    {
      numentries++;
      sprintf (buf,"MISSION %d:",i+1);
      entries[numentries-1] = new entry (numentries-1, buf, strlen ("MISSION X:")/2*-CHARLEN+400, 230+i*50, mc, 1, (void*)this);
      
      sprintf (buf,"missions/mission%d.txt",i+1);
      xpath (buf);
      fpeek = fopen (buf,"r");
      fgets (buf, 100, fpeek);
      numentries++;
      entries[numentries-1] = new entry (numentries-1, buf, strlen (buf)/2*-CHARLEN+400, 250+i*50, mc, 0, (void*)this);
      fclose (fpeek);
    }
    selected = 1+((getMissionNum ()-1)*2);
    break;
  case MENU_LOAD: case MENU_SAVE: // create the load/save menu
    if (numentries!=0)
    {
      // create caption
      entries[numentries-1] = new entry (numentries-1, caption, strlen (caption)/2*-CHARLEN+400, 180, mc, 0, (void*)this);
    }
    // create savegame entries
    for (i=0; i<5; i++)
    {
      numentries++;
      sprintf (buf,"save/%d.sav",i+1);
      xpath (buf);
      fpeek = fopen (buf,"r");
      fscanf (fpeek,"%d",&j);
      fclose (fpeek);
      if (j==0)
      {
        sprintf (buf,"GAME %d: NO MISSION",i+1);
      }
      else if (j==1)
      {
        sprintf (buf,"GAME %d: MISSION 1",i+1);
      }
      else
      {
        sprintf (buf,"GAME %d: MISSION 1-%d",i+1,j);
      }
      entries[numentries-1] = new entry (numentries-1, buf, 300, 230+i*50, mc, 1, (void*)this);
    }
    break;
  }

  selcol[0] = 0.3;
  selcol[1] = 0.65;
  selcol[2] = 0.95;
  selcol[3] = 1.0;

  i=0;
  // set first element selected
  if (type != MENU_SELMISSION)
  {
    while (i<numentries)
    {
      if (entries[i]->selectable==1)
      {
        selected = i; i=numentries;
      }
      i++;
    }
  }
  write_log ("Menu created!\n");
}

menu::~menu ()
{
  // delete entries
  /*int i=0;
  entry* temp;
  while (i<numentries)
  {
    temp = entries[i];
    //delete temp;
  }*/
}




void menu::handle ()
{
  // controls the navigation through the menu
  outro* otr;
  intro* itr;
  FILE* fpeek;
  _FBUF
  int j;

  if (child==NULL)
  {
    char* keys = getkeys ();
    int i, found=0,adj;

    adj=1;
    if (keys[SDLK_UP])
    {
      // search for next selectable upper item
      keys[SDLK_UP]=0;
      if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
        if (((mission*)getmission ())->getCockpit ()->getPongmode ()==1)
        {keys[SDLK_UP]=1; adj =0;}

      i = selected;
      while (found==0)
      {
        i-=adj;
        if (i<0) i=numentries-1;
        if (entries[i]->selectable) {selected = i; found=1;}
      }
    }
    adj=1;
    if (keys[SDLK_DOWN])
    {
      // search for next selectable lower item
      keys[SDLK_DOWN]=0;
      if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
        if (((mission*)getmission ())->getCockpit ()->getPongmode ()==1)
        {keys[SDLK_DOWN]=1; adj=0;}

      i = selected;
      while (found==0)
      {
        i+=adj;
        if (i>=numentries) i=0;
        if (entries[i]->selectable) {selected = i; found=1;}
      }
    }
    if (keys[SDLK_ESCAPE])
    {
      // close menu
      keys[SDLK_ESCAPE]=0;
      killme=1;
      // care for EASTEREGG
      if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
      {
        ((mission*)getmission ())->getCockpit ()->setPongmode (0);
      }
      if (par!=NULL)
        par->waitstate=0;
      else
      {
        if (!((mission*)getmission ())->get_demo ())
        {
          stopmusic ();
        }
      }
    }
    if (keys[SDLK_RETURN])
    {
      // selection complete!
      keys[SDLK_RETURN]=0;
      switch (this->type)
      {
      case MENU_TITLE:
        switch (selected)
        {
        case 1: // start new game
          killme=1;
          this->new_gs = GS_NEWGAME;

          break;
        case 2: // select mission
          child = new menu (MENU_LOAD, this, "LOAD GAME");
          waitstate = WAIT_LOAD;
          break;
        case 3: // view intro
          fademusic (1.5);
          itr = new intro (INTRO);
          itr->run ();
          delete itr;
          sprintf (buf,"sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
          playmusic ();
          break;
        case 4:  // view credits
          fademusic (1.5);
          otr = new outro ();
          otr->run ();
          delete otr;
          sprintf (buf,"sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
          playmusic ();
          break;
        case 5: // quit game
          child = new menu (MENU_YESNO, this, "QUIT TO OS?");
          waitstate = WAIT_QUIT;
          break;
        }
        break;
      case MENU_YESNO:
        this->par->returnvalue (selected);
        killme=1;  // mark for destruction
        break;
      case MENU_INGAME:
        switch (selected)
        {
        case 1:
          killme=1;
          stopmusic ();
          if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
          {
            ((mission*)getmission ())->getCockpit ()->setPongmode (0);
          }
          break;
        case 2: // spawn quit-menu
          child = new menu (MENU_YESNO, this, "ABORT MISSION?");
          waitstate = WAIT_ABORT;
          if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
          {
            ((mission*)getmission ())->getCockpit ()->setPongmode (0);
            playmusic ();
          }
          break;
        case 3: // EASTEREGG
          if (((Camera*)getcam ())->getCamstate ()==CAM_COCKPIT)
          {
            ((mission*)getmission ())->getCockpit ()->setPongmode (1);
          }
          break;
        }
        break;
      case MENU_GAME:
        switch (selected)
        {
        case 1: // view outro
          fademusic (1.5);
          itr = new intro (OUTRO);
          itr->run ();
          delete itr;
          sprintf (buf,"sfx/utah-saints.mod"); loadmusic (buf); setmusicvolume (TITLEVOL);
          playmusic ();
          break;
        case 2: // start the game
          killme=1;
          this->new_gs = GS_INGAME;
          setCheated (0);
          // get mission number
          loadmission ();
          break;
        case 3: // select mission
          child = new menu (MENU_SELMISSION, this, "SELECT MISSION");
          waitstate = WAIT_SELMISSION;
          break;
        case 4:  // view credits
          child = new menu (MENU_SAVE, this, "SAVE GAME");
          break;
        case 5: // quit game
          child = new menu (MENU_YESNO, this, "QUIT GAME?");
          waitstate = WAIT_QUITGAME;
          break;
        }
        break;
      case MENU_LOAD:
        sprintf (buf,"save/%d.sav",selected);
        xpath (buf);
        fpeek = fopen (buf,"r");
        fscanf (fpeek,"%d",&j);
        fclose (fpeek);
        this->par->returnvalue (j);
        killme=1;  // mark for destruction
        break;
      case MENU_SAVE:
        sprintf (buf,"save/%d.sav",selected);
        xpath (buf);
        fpeek = fopen (buf,"w");
        j=getMissionAdvance ();
        fprintf (fpeek,"%d",j);
        fclose (fpeek);
        killme=1;  // mark for destruction
        break;
      case MENU_SELMISSION:
        setMission (floor (selected/2)+1);
        killme=1;
        break;
      }
    }
  }
  else
    child->handle ();

  // handle destruction of child menus
  if (child!=NULL)
  {
    if (child->killme==1)
    {
      delete child;
      child=NULL;
    }
  }
}


void menu::returnvalue (int val)
{
  // handle the possible return values by all posible menus
  switch (this->child->type)
  {
  case MENU_YESNO:
    if (val==1) // that´s a yes!
    {
      switch (waitstate)
      {
      case WAIT_QUIT:
        new_gs = GS_QUIT;
        this->killme=1;
        break;
      case WAIT_ABORT:
        new_gs = GS_ABORT;
        this->killme=1;
        break;
      case WAIT_QUITGAME:
        new_gs = GS_QUITGAME;
        this->killme=1;
        break;
      }
    }
    //nothing to do for a NO
    break;
  case MENU_LOAD:
    switch (waitstate)
    {
    case WAIT_LOAD:
      new_gs = GS_LOADGAME;
      setMissionAdvance (val);
      this->killme=1;
      break;
    }
  }
}


void menu::draw ()
{
  // draw a menu
  GLfloat ticks = gettime ();

  glColor4f (0.0, 0.0, 0.25, 0.75);

  glBegin (GL_QUADS);
  glVertex3f (-1.0, abs (600-entries[0]->gety ())*0.0027-0.7, -1.5000001);
  glVertex3f (-1.0, abs (600-entries[numentries-1]->gety ())*0.0027-0.9, -1.5000001);
  glVertex3f (1.0, abs (600-entries[numentries-1]->gety ())*0.0027-0.9, -1.5000001);
  glVertex3f (1.0, abs (600-entries[0]->gety ())*0.0027-0.7, -1.5000001);
  glEnd ();

  for (int i=0; i<numentries; i++)
    entries[i]->draw ();

  // select-color cycle
  if (child==NULL)
  {
    selcol[0]+=blink*ticks*0.8;
    selcol[1]+=blink*ticks*0.5;
    selcol[2]+=blink*ticks;

    if (selcol[2]>1.0) {selcol[2]=1.0;blink=-blink;}
    if (selcol[2]<0.4) {selcol[2]=0.4;blink=-blink;}
  }

  drawstrings ();

  if (child!=NULL) child->draw ();
}




