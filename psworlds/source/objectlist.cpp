// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "objectlist.h"
#include "psworlds.h"

// creates an empty list
objectlist::objectlist ()
{
  int i;
  for (i=0;i<8;i++)
  {
    first[i]=NULL;
    last[i]=NULL;
  }
  first_obj=NULL;

  sortme = 0;
}

// release all allocated memory, keep the objects
objectlist::~objectlist ()
{
  clear_all_entrys ();
}

// add object to the list
void objectlist::add_object (object* obj)
{
  listobj* prev=NULL;
  listobj* tmp = (listobj*) malloc (sizeof (listobj));
  tmp->obj = obj;
  int typ=obj->Typ ();
  if (typ==WAYPOINT||typ==PARTICLE||typ==COMPASS||typ==TRAIL||typ==HEADLIGHT||typ==ALIENLIGHT)
  {
    typ=BITMAP;
  }
  if (typ==TORPEDO)
  {
    typ=DIRECT;
  }
  switch (typ)
  {
    case PLAYER:
      prev=NULL;
      break;
    case COMPUTER:
      prev=last[PLAYER-1];
      break;
    case OTHER:
      prev=last[COMPUTER-1];
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
    case TURRET:
      prev=last[OTHER-1];
      if (prev==NULL)
      {
        prev=last[COMPUTER-1];
      }
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
    case BUILDING:
      prev=last[TURRET-1];
      if (prev==NULL)
      {
        prev=last[OTHER-1];
      }
      if (prev==NULL)
      {
        prev=last[COMPUTER-1];
      }
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
    case HOMING:
      prev=last[BUILDING-1];
      if (prev==NULL)
      {
        prev=last[TURRET-1];
      }
      if (prev==NULL)
      {
        prev=last[OTHER-1];
      }
      if (prev==NULL)
      {
        prev=last[COMPUTER-1];
      }
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
    case DIRECT:
      prev=last[HOMING-1];
      if (prev==NULL)
      {
        prev=last[BUILDING-1];
      }
      if (prev==NULL)
      {
        prev=last[TURRET-1];
      }
      if (prev==NULL)
      {
        prev=last[OTHER-1];
      }
      if (prev==NULL)
      {
        prev=last[COMPUTER-1];
      }
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
    case BITMAP:
      prev=last[DIRECT-1];
      if (prev==NULL)
      {
        prev=last[HOMING-1];
      }
      if (prev==NULL)
      {
        prev=last[BUILDING-1];
      }
      if (prev==NULL)
      {
        prev=last[TURRET-1];
      }
      if (prev==NULL)
      {
        prev=last[OTHER-1];
      }
      if (prev==NULL)
      {
        prev=last[COMPUTER-1];
      }
      if (prev==NULL)
      {
        prev=last[PLAYER-1];
      }
      break;
  }
  if (first_obj==NULL)
  {
    tmp->next=NULL;
    tmp->prev=NULL;
    first_obj=tmp;
    first[typ-1]=tmp;
    last[typ-1]=tmp;
  }
  else if (prev==NULL)
  {
    first_obj->prev=tmp;
    tmp->next=first_obj;
    if (tmp->next!=NULL)
    {
      (tmp->next)->prev=tmp;
    }
    tmp->prev=NULL;
    first_obj=tmp;
    first[typ-1]=tmp;
    if (last[typ-1]==NULL)
    {
      last[typ-1]=tmp;
    }
  }
  else
  {
    tmp->next=prev->next;
    prev->next=tmp;
    tmp->prev=prev;
    if (tmp->next!=NULL)
    {
      (tmp->next)->prev=tmp;
    }
    first[typ-1]=tmp;
    if (last[typ-1]==NULL)
    {
      last[typ-1]=tmp;
    }    
  }
}

// delete object from the list, destroy object
void objectlist::delete_object (listobj* obj)
{
  object* tmpobj=obj->obj;
  delete_entry (obj);
  delete tmpobj;
}

// delete object from the list, keep object
void objectlist::delete_entry (listobj* obj)
{
  for (int type=0;type<8;type++)
  {
    if (obj==first[type])
    {
      first[type]=obj->next;
      if (obj==last[type])
      {
        first[type]=NULL;
        last[type]=NULL;
      }
      break;
    }
    else if (obj==last[type])
    {
      last[type]=obj->prev;
      break;
    }
  }
  if (obj==first_obj)
  {
    first_obj=obj->next;
  }
  else
  {
    (obj->prev)->next=obj->next;
    if ((obj->next)!=NULL)
    {
      (obj->next)->prev=obj->prev;
    }
  }
  free (obj);
}

// returns the first object of the specified type
listobj* objectlist::get_first (int typ)
{
  if (typ==WAYPOINT||typ==PARTICLE||typ==COMPASS||typ==TRAIL||typ==HEADLIGHT||typ==ALIENLIGHT)
  {
    typ=BITMAP;
  }
  if (typ==TORPEDO)
  {
    typ=DIRECT;
  }
  return first[typ-1];
}

// returns the first object in the list
listobj* objectlist::get_first ()
{
  return first_obj;
}

// draws all objects
void objectlist::draw (float x, float y, float z)
{
  listobj* tmp = first_obj;
  while (tmp!=NULL)
  {
      float ox=tmp->obj->getx ();
      float oy=tmp->obj->gety ();
      float oz=tmp->obj->getz ();

      if (fastdist3(x-ox,y-oy,z-oz)<1000)
      {
        tmp->obj->draw ();
      }
    tmp = tmp->next;
  }
}

// move all objects
void objectlist::control ()
{
  listobj* tmp = first_obj;
  while (tmp!=NULL)
  {
    tmp->obj->control ();
    tmp = tmp->next;
  }
}

// clear all entrys, keep objects
void objectlist::clear_all_entrys ()
{
  while (first_obj!=NULL)
  {
    delete_entry (first_obj);
  }
}

// clear all entrys, destroy objects
void objectlist::clear_all_objects ()
{
  while (first_obj!=NULL)
  {
    delete_object (first_obj);
  }
}

// destroys object if object.destroyed==1
void objectlist::destroy_all_objects ()
{
  listobj* tmp=first_obj;
  listobj* tmp1;
  while (tmp!=NULL)
  {
    if (tmp->obj->getdestroy ()==1)
    {
      tmp1 = tmp;
      tmp = tmp->next;
      delete_object (tmp1);
    }
    else
    {
      tmp = tmp->next;
    }
  }
}

// checks if the object is in the list and returns corresponding listobj
// returns NULL if object not found
listobj* objectlist::in_list (object* obj)
{
  listobj* tmp=first_obj;
  while (tmp!=NULL)
  {
    if (tmp->obj==obj)
    {
      return tmp;
    }
    tmp=tmp->next;
  }
  return NULL;
}

// checks if the object with ID is in the list and returns corresponding listobj
// returns NULL if object not found
listobj* objectlist::in_list (int ID)
{
  listobj* tmp=first_obj;
  while (tmp!=NULL)
  {
    if (tmp->obj->getID ()==ID)
    {
      return tmp;
    }
    tmp=tmp->next;
  }
  return NULL;
}


void objectlist::setsort (int sort)
{
  sortme = sort;
}


void objectlist::resort ()
{
  // resort the list using quicksort. order in descending camdistance
  if (sortme==1)
  {
    // implement sorter, not longer needed
  }

}

// returns nearest object to obj
listobj* objectlist::nearest (object* obj)
{
  listobj* tmp = first_obj;
  listobj* nearest_obj = NULL;
  float d=0;
  float d1;
  while (tmp!=NULL)
  {
    d1 = distance (obj,tmp->obj);
    if (d1<d||d==0)
    {
      nearest_obj = tmp;
      d = d1;
    }
    tmp = tmp->next;
  }
  return nearest_obj;
}

