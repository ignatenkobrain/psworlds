// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "displaylistlist.h"
#include "psworlds.h"

displaylistlist::displaylistlist ()
{
  first = NULL;
}

displaylistlist::~displaylistlist ()
{
  // delete all entrys
  dllist* tmp=first;
  dllist* tmp1=NULL;
  while (tmp!=NULL)
  {
    tmp1 = tmp->next;
    free (tmp);
    tmp = tmp1;
  }
}

// returns displaylistid if name in list
dllist* displaylistlist::inlist (char* name)
{
  dllist* tmp=first;
  while (tmp!=NULL)
  {
    if (!strcmp (tmp->name,name))
    {
      return tmp;
    }
    tmp=tmp->next;
  }
  return NULL;
}
  
// adds entry to list
void displaylistlist::add (char* name, int ID, float sphere_rad, float minx, float maxx, float miny, float maxy, float minz, float maxz)
{
  _FBUF
  dllist* newdl = (dllist*)malloc (sizeof (dllist));
  newdl->name = (char*)malloc (strlen (name)+1);
  strcpy (newdl->name,name);
  newdl->ID=ID;
  newdl->sphere_rad = sphere_rad;
  newdl->minx = minx;
  newdl->maxx = maxx;
  newdl->miny = miny;
  newdl->maxy = maxy;
  newdl->minz = minz;
  newdl->maxz = maxz;
  newdl->next=first;
  first=newdl;
  sprintf (buf,"Created Displaylist for: %s\n",name);
  write_log (buf);
}

