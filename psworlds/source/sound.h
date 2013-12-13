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
int loadmod (char *fname);
int playmod (void);
void stopmod ();
void stopAudio (int mode);
void killAudio (void);
void playengine (int samnum, int freq, int volume);
void playsam (int samnum, int freq, int volume, int priority);
void updateAudio (void);
void loadsampleset (void);
void stoplooping ();
void destroyvoices ();
void initnormalvoices ();
void initintrovoices ();
void setmodvol (int vl);
void fademusic (float fsp);
int loadmp3(char *fname);
void playmp3(int vol);
void stopmp3();
void setmp3vol (int vol);

#endif

