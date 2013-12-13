// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __TEXTURELIST_H_
#define __TEXTURELIST_H_

#include "tgaloader.h"

// class to manage textures
// each possible texture is only stored once

// structure for list management

typedef struct texlist
{
  char* name;      // texture name
  textureObj* texobj; // texture object
  texlist* next;    // next in list
} texlist;

class texturelist  
{
private:
  texlist* first;    // first in list
public:
  // create empty list
  texturelist ();
  // delete all entries and delete list
  virtual ~texturelist ();
  // returns textureitem if name in list
  textureObj* inlist (char* name);
  // adds entry to list
  void add (char* name, textureObj* newobj);
};

#endif
