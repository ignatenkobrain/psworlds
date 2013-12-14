// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#ifndef __SOUND_H_
#define __SOUND_H_

#define NUMVOICES 32


int initAudio (void);
int loadmusic (char *fname);
int playmusic (int volume);
int playmusic (void);
void stopmusic ();
void stopAudio (int mode);
void killAudio (void);
void playengine (int samnum, int freq, int volume);
void playsam (int samnum, int freq, int volume, int priority);
void updateAudio (void);
void loadsampleset (void);
void stoplooping ();
void setmusicvolume (int volume);
void fademusic (float fsp);

#endif

