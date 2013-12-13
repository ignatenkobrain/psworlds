// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __OBJECTLIST_H_
#define __OBJECTLIST_H_

#include "object.h"

// structure for list management
typedef struct listobj
{
  object* obj;
  listobj* next;
  listobj* prev;
} listobj;

// this class handels a list of objects
class objectlist  
{
private:
  listobj* first[8];  // pointer to the first object for each object typ
  listobj* last[8];  // pointer to the last object for each object typ
  listobj* first_obj;  // pointer to the first object
  int sortme;      // should the list be resorted this frame?


public:
  objectlist ();            // create an empty objectlist
  virtual ~objectlist ();        // release all allocated resources, to not destroy objects!
  void add_object (object* obj);    // add an object to the list
  void delete_object (listobj* obj);  // delete the object from the list, destroy the object
  void delete_entry (listobj* obj);  // delete the object from the list, keep the object
  listobj* get_first (int typ);    // returns the first object of the specified typ
  listobj* get_first ();        // returns the first object

  // checks if the object is in the list and returns corresponding listobj
  // returns NULL if object not found
  listobj* in_list (object* obj);

  // checks if the object with ID is in the list and returns corresponding listobj
  // returns NULL if object not found
  listobj* in_list (int ID);

  // draws all objects dependent on their position relative to x,y,z
  void draw (float x, float y, float z);

  void control ();        // moves all objects
  void clear_all_entrys ();  // clear all entrys, keep objects
  void clear_all_objects ();  // clear all entrys, destroy objects
  void destroy_all_objects ();  // destroys object if object.destroyed==1

  void setsort (int sort);    // set indicator whether we need to sort
  void resort ();        // quicksort the list by descending distance to cam

  listobj* nearest (object* obj);  // returns nearest object to obj

};

#endif

