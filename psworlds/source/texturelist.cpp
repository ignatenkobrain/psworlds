// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "texturelist.h"

texturelist::texturelist()
{
	first = NULL;
}

texturelist::~texturelist()
{
	// delete all entrys
	texlist* tmp=first;
	texlist* tmp1=NULL;
	while(tmp!=NULL)
	{
		tmp1 = tmp->next;
		delete tmp->texobj;
		free(tmp);
		tmp = tmp1;
	}
}

// returns textureobj if name in list
textureObj* texturelist::inlist(char* name)
{
	texlist* tmp=first;
	while(tmp!=NULL)
	{
		if(!strcmp(tmp->name,name))
		{
			return tmp->texobj;
		}
		tmp=tmp->next;
	}
	return NULL;
}
	
// adds entry to list
void texturelist::add(char* name, textureObj* newobj)
{
	texlist* newtex = (texlist*)malloc(sizeof(texlist));
	newtex->name = (char*)malloc(strlen(name)+1);
	strcpy(newtex->name,name);
	newtex->texobj=newobj;
	newtex->next=first;
	first=newtex;
}

