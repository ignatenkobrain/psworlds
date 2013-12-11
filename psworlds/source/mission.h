// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __MISSION_H_
#define __MISSION_H_

#include "psworlds.h"
#include "objectlist.h"
#include "models.h"
#include "camera.h"
#include "world.h"
#include "tgaloader.h"
#include "cockpit.h"
#include "trail.h"

typedef struct
{
	char text[100];		     // short description of goal
	objectlist *destroylist; // objects to be destroyed
	objectlist *savelist;	 // objects to be saved
	objectlist *reachlist;	 // waypoints to be reached by reachobj
	object *reachobj;
	int failed;
	int success;
} goal_t;


// this class includes all informations and procedures to play a mission
class mission  
{
private:
	FILE *f;				// pointer to mission file for easy access
	objectlist *objlist;	// list of all objects used in the mission
	objectlist *bmplist;	// list of bitmap objects, wil be reordered often
	objectlist *partlist;	// list of particles
	objectlist *traillist;	// list of trails
	objectlist *litelist;	// list of headlights / flares
	objectlist *alienlist;	// list of headlights / flares
	player* plr;			// the player object
	world* sea;				// the world (sea ground)
	Camera* cam;			// the camera

	Cockpit* cockpit;		// the cockpit

	int pausemenu;			// indicate whether the game is paused while menu activity
	int demo;				// if demo = 1 game is not playable

	int mis_success;
	int mis_failure;
	int player_dead;

	int active_goal;
	int num_goals;	// number of goals
	goal_t* goals;	// mission goals

	void read_file();				// reads the mission file and creates objectlists
	void read_map();				// reads map information
	void read_model();				// reads model information
	void read_goal();				// reads goal information
	void read_alienlight(int gll);	// reads alienlight information
	objectlist* read_goal_list(object*); // reads destroylist,... information
	object* read_ship(char* shp, int typ);	// reads ship information
	void read_targetlist();			// reads targetlist information
	void read_target(object* obj);	// reads target information for object obj
	void read_pathlist();			// reads pathlist information
	void read_waypoint(object* obj, int num);// reads waypoint information for object obj
	void read_weapon(object* obj);	// reads one weapon info for object obj
	void read_light(object* obj);	// reads a headlight

	// performs a boundingsphere test for two objects
	int boundingsphere_collide(model* obj1, model* obj2, GLfloat d);

	// checks obj for collision with other objects
	// typ ... collsion with 0 all objects, 1 missiles only
	// coll_soon ... 1 check for soon_collision with other objects
	void check_collision(listobj* lobj, int typ, int coll_soon);

	// deletes the collision infos of all objects;
	void clear_collision_info();

	// checks if obj1 will soon collide with obj2
	int collisionsoon_detection(model* obj1, model* obj2, GLfloat d);
	
public:
	mission(char* filename);	// constructs a mission out of a mission file
	virtual ~mission();			// frees all allocated resources
	void run();					// starts the mission
	void camcontrol();			// changes the cameraview dependend on keyboard input

	void move_objects();		// moves all objects
	void draw_objects();		// draws all objects
	void collisiondetection();	// checks collision and collision soon for all objects
	void destroy_objects();		// removes all destroyed objects
	void add_particles();		// adds some particles (plancton) for more fun

	int drawcockpit();			// draws the cockpit
	
	void add_object(object* obj);	// adds an object to the objectlist
	void add_bmp(bitmap *bmp);		// add a bitmap
	void add_trail(trail *tr);		// add a trail

	world* getworld();			// returns the world (to be drawn)

	objectlist* get_objlist();
	objectlist* get_bmplist();
	objectlist* get_traillist();
	objectlist* get_litelist();
	objectlist* get_alienlist();

	goal_t* get_goal(int i);
	int get_numgoals();

	Cockpit* getCockpit();	// returns the cockpit

	int getpausemode();		// return the pausemenu activity
	int get_demo();

	void check_goals();		// checks the goals for success or failure

	int get_failure();
	int get_success();
	int get_player_dead();

	void set_failure();
	void set_success();
};

#endif
