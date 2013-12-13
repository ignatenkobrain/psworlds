// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __STARTSCREEN_H_
#define __STARTSCREEN_H_

class startscreen
{
private:
  void initsound ();  // reserve channels for LARGE mods
  void bootpic ();    // display boot picture
public:
  startscreen ();
  virtual ~startscreen ();
  void run ();
};

#endif 
