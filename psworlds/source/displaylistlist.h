// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __DISPLAYLISTLIST_H_
#define __DISPLAYLISTLIST_H_

// class to manage displaylists
// for each possible model there is only one displaylist
// models of the same type use only one displaylist

// structure for list management
//typedef struct dllistentry;

typedef struct dllist
{
  char* name;
  int ID;
  float sphere_rad;  // radius for bounding sphere
  float minx;      // values for bounding box
  float maxx;      // ...
  float miny;      // ...
  float maxy;      // ...
  float minz;      // ...
  float maxz;      // ...
  dllist* next;
} dllist;

class displaylistlist  
{
private:
  dllist* first;
public:
  displaylistlist ();
  virtual ~displaylistlist ();
  dllist* inlist (char* name);    // returns displaylistitem if name in list
  void add (char* name, int ID, float sphere_rad, float minx, float maxx, float miny, float maxy, float minz, float maxz);  // adds entry to list
};

#endif
