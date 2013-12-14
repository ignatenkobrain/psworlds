// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "sound.h"
#include "psworlds.h"
#include <SDL_mixer.h>

#define NUMSAMPLES 32

Mix_Chunk* samples[NUMSAMPLES];
Mix_Music* module = NULL;
Mix_Music* mp3 = NULL;

int enginechannel = -1;
int engineused = 0;
int floodchannel = -1;
int mp3channel = -1;


int initAudio (void)
{
  write_log ("Initializing Audio Device.. ");
  if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 4096)) {
    write_log ("Sound initalization failed!\n");
    return -1;
  }

  write_log ("done!\n");
  return 0;
}

int loadmusic (char *fname)
{
  // load a module (S3M, MOD, etc.) file
  _FBUF

  xpath (fname);
  Mix_FreeMusic (module);
  module = Mix_LoadMUS (fname);
  if (module == NULL)
    return -1;

  sprintf (buf, "Module %s loaded\n", fname);
  write_log (buf);

  return 0;
}

int playmusic (int volume)
{
  // start playing the module
  if (Mix_PlayingMusic ())
    return 0;

  write_log ("Starting mod\n");

  setmusicvolume (volume);
  Mix_PlayMusic (module, 0);

  return 0;
}

int playmusic ()
{
    playmusic (255);
}


void setmusicvolume (int volume)
{
  Mix_VolumeMusic (volume);
}


void fademusic (float fsp)
{
  float vl=Mix_VolumeMusic (-1);
  float faderate = vl/fsp;
  float dur;
  double ticks;

  while (vl > 0) {
    timing ();
    ticks = gettime ();
    vl -= ticks * faderate;
    setmusicvolume (vl);
    // HACK! keep the damn CPU busy
    for (int i = 0; i < 10000; i++)
      dur = rand ();
  }
  setmusicvolume (0);
}


void stopmusic ()
{
  // stop playing
  write_log ("Stopping mod\n");
  Mix_HaltMusic ();
}


void stopAudio (int mode)
{
  // stop all sound playing
  if (mode == 0)
    stopmusic ();

  Mix_HaltChannel (-1);
  floodchannel =- 1;
}


void killAudio (void)
{
  // shutdown audio device
  write_log ("Shutting down Audio Device.. ");

  stopAudio (0);

//  FMUSIC_FreeSong (module);
//  FSOUND_Close ();

    write_log ("done!\n");
}


void playsam (int samnum, int freq, int volume, int priority)
{
  int chann = Mix_PlayChannel (-1, samples[samnum], 0);
  if (chann >= 0) {
    Mix_Volume (chann, volume);
    if (samnum == 19) // if flood
      floodchannel = chann;
  }
}


void playengine (int samnum, int freq, int volume)
{
  // play the engine (looping sound)

  if (enginechannel < 0)
    enginechannel = Mix_PlayChannel (-1, samples[samnum], -1);
  engineused = 1;

  Mix_Volume (enginechannel, volume);
}


void updateAudio (void)
{
  // kill the engine sample if it´s necessary
  if (engineused == 0) // if the engine was not called this frame
    if (enginechannel > -1) { // but is still playing, 
      Mix_HaltChannel (enginechannel);
      enginechannel = -1;  // signal it´s not playing anymore
    }

  engineused = 0;
}


void loadsam (char* buf, int n)
{
  samples[n] = Mix_LoadWAV (buf);
}


void loadsampleset (void)
{
  // loads all the samples required by Possible worlds
  _FBUF
  write_log ("Loading Sound FX...");

  sprintf (buf, "sfx/engine1.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[0], 0);
  loadsam (buf, 0);
//  FSOUND_Sample_SetLoopMode (samples[0], FSOUND_LOOP_NORMAL);
//  FSOUND_Sample_SetLoopPoints (samples[0], 0, FSOUND_Sample_GetLength (samples[0]));
  sprintf (buf, "sfx/shipcrash16k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[1], 0);
  loadsam (buf, 1);
  sprintf (buf, "sfx/floorcrash116k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[2], 0);
  loadsam (buf, 2);
  sprintf (buf, "sfx/Subshot.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[3], 0);
  loadsam (buf, 3);
  sprintf (buf, "sfx/abyssexplo16k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[4], 0);
  loadsam (buf, 4);
  sprintf (buf, "sfx/bonkplusblub216k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[5], 0);
  loadsam (buf, 5);
  sprintf (buf, "sfx/bonkplusblub116k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[6], 0);
  loadsam (buf, 6);
  sprintf (buf, "sfx/bonkplusblub316k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[7], 0);
  loadsam (buf, 7);
  sprintf (buf, "sfx/bleep.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[8], 0);
  loadsam (buf, 8);
  sprintf (buf, "sfx/whale_cry.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[9], 0);
  loadsam (buf, 9);
  sprintf (buf, "sfx/whale_feeding.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[10], 0);
  loadsam (buf, 10);
  sprintf (buf, "sfx/whale_howl.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[11], 0);
  loadsam (buf, 11);
  sprintf (buf, "sfx/whale_short1.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[12], 0);
  loadsam (buf, 12);
  sprintf (buf, "sfx/whale_short2.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[13], 0);
  loadsam (buf, 13);
  sprintf (buf, "sfx/sonar.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[14], 0);
  loadsam (buf, 14);
  sprintf (buf, "sfx/weapchange.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[15], 0);
  loadsam (buf, 15);
  sprintf (buf, "sfx/malfunction.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[16], 0);
  loadsam (buf, 16);
  sprintf (buf, "sfx/crack116k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[17], 0);
  loadsam (buf, 17);
  sprintf (buf, "sfx/crack216k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[18], 0);
  loadsam (buf, 18);
  sprintf (buf, "sfx/crackandwater16k.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[19], 0);
  loadsam (buf, 19);
//  FSOUND_Sample_SetLoopMode (samples[19], FSOUND_LOOP_NORMAL);
//  FSOUND_Sample_SetLoopPoints (samples[19], 10562, 21981);
  sprintf (buf, "sfx/shred.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[20], 0);
  loadsam (buf, 20);
  sprintf (buf, "sfx/swoosh1.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[21], 0);
  loadsam (buf, 21);
  sprintf (buf, "sfx/swoosh2.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[22], 0);
  loadsam (buf, 22);
  sprintf (buf, "sfx/swoosh3.wav"); xpath (buf); //ALoadWaveFile (buf, &sample[23], 0);
  loadsam (buf, 23);
  sprintf (buf, "sfx/accomplish.wav"); xpath (buf); 
  loadsam (buf, 24);
  sprintf (buf, "sfx/missile.wav"); xpath (buf);
  loadsam (buf, 25);
  sprintf (buf, "sfx/reloadmissile.wav"); xpath (buf); 
  loadsam (buf, 26);

  write_log ("done!\n");
}


void stoplooping ()
{
  // stop looping sound, e.g. engine and flood
  Mix_HaltChannel (enginechannel);
  if (floodchannel > 0) {Mix_HaltChannel (floodchannel); floodchannel = -1; }
}

