// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __OBJECT_H_
#define __OBJECT_H_

#define BITMAP    1
#define OTHER    2
#define HOMING    3
#define DIRECT    4
#define BUILDING  5
#define TURRET    6
#define PLAYER    7
#define COMPUTER  8
#define WAYPOINT  9
#define PARTICLE  10
#define TRAIL    11
#define COMPASS    12
#define TORPEDO    13
#define HEADLIGHT  14
#define ALIENLIGHT  15

// movement state
#define MV_NORMAL  0
#define MV_COLLIDE  1
#define MV_COLLSOON 2
#define MV_TARGET  3
#define MV_DESTROY  4
#define MV_START  5

// the abstract superclass for all objects

class object  
{
protected:
  int ID;      // used to identify an object when reading mission file

  float x;    // geometric position of the object
  float y;    //.......
  float z;    //.......
  float rotx;  //.......
  float roty;  //.......
  float rotz;  //.......
  float dx;    //.......
  float dy;    //.......
  float dz;    //.......
  float drotx;  //.......
  float droty;  //.......
  float drotz;  //.......

  char* name;

  int movement_state;    // collision recovery, collision soon, normal, ...
  float time_to_live;    // time (in seconds) before object is finally destroyed
  int destroy;      // object is ready for final destruction
  void update_lifetime ();  // decreases time_to_live and sets destroy if necessary

public:
  object (float X,float Y,float Z,float rotX, float rotY, float rotZ);
  virtual ~object ();
  virtual int Typ ()=0;    // returns type of object
  virtual void draw ()=0;    // draws the object
  virtual void move ()=0;    // moves the object
  virtual void control ()=0;  // controls the object

  virtual void clear_collision_info ()=0;  // clears collision info
  
  // access functions to private variables
  void setx (float value);
  void sety (float value);
  void setz (float value);
  void setrotx (float value);
  void setroty (float value);
  void setrotz (float value);
  float getx ();
  float gety ();
  float getz ();
  float getdx ();
  float getdy ();
  float getdz ();
  float getrotx ();
  float getroty ();
  float getrotz ();
  int getID ();
  void setID (int value);
  int getdestroy ();
  void set_lifetime (float value);
  int getmvstate ();
  void setName (char* value);
  char* getName ();

  virtual void set_weaponcount (int c)=0;
  virtual void add_weapon (char* filename, char* name, float sx, float sy, float sz, float sx2, float sy2, float sz2,float time, int power, int amount, int type, int dbl, float mvspd, float rotspd, float rate)=0;

  virtual void add_object (int target,object* obj)=0;

  virtual void set_enemystatus (int value);

};

#endif

