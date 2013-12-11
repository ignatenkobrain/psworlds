// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "computer.h"
#include "mission.h"

computer::computer(char* filename, float X, float Y, float Z, float rotX, float rotY, float rotZ,
				   float Rotspd, float Maxspd, int Mass, int Energy,GLfloat *norms,int *sw)
					:fighter(filename,X,Y,Z,rotX,rotY,rotZ,Rotspd,Maxspd,Mass,Energy,norms,sw)
{
	mvspd=0;
	retreat=0;
	r_left=0;
	r_up=0;
	retreat_time=0;
}

computer::~computer()
{

}

int computer::Typ() {
	return COMPUTER;
}

void computer::draw() {
	model::draw();
}

// controls computer controlled fighters
void computer::control()
{
	float secs=gettime();
	float destx,desty,destz,d,rx,ry,rz;
	int collsoon=0;

	cleanup_targetlist();

	// first, get active waypoint
	
	if(pathlist->get_first()!=NULL)
	{
		// if no waypt available, select first one!
		if(waypt==NULL)
		{
			if(!((waypoint*)pathlist->get_first()->obj)->get_reached()) waypt = pathlist->get_first();
		}
		
		// if waypoint is reached set waypoint to reached
		if(waypt!=NULL)
		{
			if(distance(this,waypt->obj)<WAYPT_DIST)
			{
				((waypoint*)waypt->obj)->set_reached();
				// if waypoint = endpoint set all waypoints to reached
				if(((waypoint*)waypt->obj)->get_end())
				{
					// fighters usually don´t encounter endpoints, just transports
					// or other very large ships
					write_log("endpoint reached\n");
					waypt = NULL;
				}
				// otherwise just select next waypoint
				else
				{
					waypt = waypt->next;
					
					if(waypt == NULL)
					{
						listobj* tmp = pathlist->get_first();
						while(tmp!=NULL)
						{
							((waypoint*)tmp->obj)->reset_reached();
							tmp = tmp->next;
						}
						waypt = pathlist->get_first();
					}
				}
			}
		}
	}

	//	nearobject = search nearest object in targetlist
	listobj* near_obj = targetlist->nearest(this);

	// if target still exists
	if(target!=NULL)
	{
		if(targetlist->in_list(target->obj)!=NULL)
		{
			// if target is far away
			if(distance(this,target->obj)>1000)
			{
				if(near_obj!=NULL)
				{
					// and nearest target is near
					if(distance(this,near_obj->obj)<1000)
					{
						// choose new target
						target = near_obj;
					}
					else
					{
						// no target near
						target = NULL;
					}
				}
				else
				{
					// no target near
					target = NULL;
				}
			}
		}
	}
	else
	{
		if(near_obj!=NULL)
		{
			// if nearest target is not to far away
			if(distance(near_obj->obj,this)<1000)
			{
				// choose target
				target = near_obj;
			}
			else
			{
				// no target near
				target = NULL;
			}
		}
		else
		{
			// no target near
			target = NULL;
		}
	}

	// try to prohibit collisions with ground
	if(sonar(minz*3)<20)
	{
		down();
		down();
	}
	if(collision_soon_object!=NULL)
	{
		// try to prohibit collisions with objects
		if(collision_soon_distance<(this->bounding_rad()+((model*)collision_soon_object)->bounding_rad())*6*mvspd/maxspd)
		{
			collsoon=1;
			destx=collision_soon_object->getx()-x;
			desty=collision_soon_object->gety()-y;
			destz=collision_soon_object->getz()-z;
			rx=destx*invmat[0]+desty*invmat[4]+destz*invmat[8];
			ry=destx*invmat[1]+desty*invmat[5]+destz*invmat[9];
			rz=destx*invmat[2]+desty*invmat[6]+destz*invmat[10];
			d=collision_soon_distance;
			rx/=d;
			ry/=d;
			rz/=d;
			r_left=0;
			r_up=0;
			if(rx<0)
			{
				r_left=1;
				right();
				right();
				right();
			}
			else
			{
				r_left=-1;
				left();
				left();
				left();
			}
			if(ry<0)
			{
				r_up=1;
				down();
				down();
				down();
			}
			else
			{
				r_up=-1;
				up();
				up();
				up();
			}
			if(mvspd>0)
			{
				dec_speed();
				dec_speed();
			}
			if(target!=NULL)
			{
				if(collision_soon_object==target->obj&&!retreat)
				{
					retreat=1;
					retreat_time=0.0;
				}
			}
		}

		// 5th step, fire if the targetobject is collisionsoon_object
		if(((target!=NULL) && (collision_soon_object==target->obj)))
		{
			active_weapon=0;
			shoot();
		}
	}
	
	// select either target or waypoint to follow
	listobj* agressor = NULL;

	if(waypt!=NULL) agressor = waypt;
	if(target!=NULL) agressor = target;

	if(!retreat)
	{
		if(agressor!=NULL && (agressor->obj!=collision_soon_object || collsoon==0))
		{
			// follow the object
			destx=agressor->obj->getx();
			desty=agressor->obj->gety();
			destz=agressor->obj->getz();
		
			destx-=x;
			desty-=y;
			destz-=z;
	
			rx=destx*invmat[0]+desty*invmat[4]+destz*invmat[8];
			ry=destx*invmat[1]+desty*invmat[5]+destz*invmat[9];
			rz=destx*invmat[2]+desty*invmat[6]+destz*invmat[10];
			d=fastdist3(rx,ry,rz);
			rx/=d;
			ry/=d;
			rz/=d;
			if(rx<-0.05) left();
			else if(rx>0.05) right();
			if(ry<-0.05) up();
			else if(ry>0.05) down();
			if(mvspd<maxspd/2&&movement_state!=MV_COLLIDE) inc_speed();
			if(movement_state==MV_COLLIDE)
			{
				dec_speed();
				dec_speed();
			}
		}
	}
	else
	{
		retreat_time+=secs;
		if(retreat_time>=5.0+rand()/(float)RAND_MAX*3.0)
		{
			retreat = 0;
		}
		rotspd/=3;
		if(r_left==-1) left();	
		else if(r_left==1) right();
		if(r_up==-1) up();
		else if(r_up==1) up();
		rotspd*=3;
		if(mvspd<maxspd/2&&movement_state!=MV_COLLIDE) inc_speed();
	}

	// if no target stop
	if(agressor==NULL)
	{
		if(mvspd>0) dec_speed();
		if(mvspd<0) inc_speed();
		if(fabs(mvspd)<1.0) mvspd=0;
	}

	// align with ground (roll)
	float rolly=mat[1];
	if(rolly<-0.1)
	{
		rollleft();
	}
	else if(rolly>0.1)
	{
		rollright();
	}
	

	// update time since last shot
	lastshot+=secs;

	move();
	update_lifetime();
}


// delete all entrys in targetlist from destroyed objects
void computer::cleanup_targetlist()
{
	objectlist* objlist = ((mission*)getmission())->get_objlist();
	listobj* tmp = targetlist->get_first();
	listobj* tmp1 = NULL;
	while(tmp!=NULL)
	{
		if(objlist->in_list(tmp->obj)==NULL)
		{
			tmp1 =  tmp->next;
			targetlist->delete_entry(tmp);
			tmp = tmp1;
		}
		else if(tmp->obj->getmvstate()==MV_DESTROY)
		{
			tmp1 =  tmp->next;
			targetlist->delete_entry(tmp);
			tmp = tmp1;
		}
		else
		{
			tmp = tmp->next;
		}
	}
	if(target!=NULL)
	{
		if(targetlist->in_list(target->obj)==NULL)
			target = NULL;
	}
}

