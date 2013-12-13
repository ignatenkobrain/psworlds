// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#ifndef __FONTS_H_
#define __FONTS_H_

#include "psworlds.h"
#include "tgaloader.h"

#define PIXWIDTH 0.0025
#define PIXHEIGHT 0.0027


class font
{
private:
  void makelists ();  // create the display lists

public:
  textureObj *chars;  // the texture
  GLuint displ;    // display lists for the characters

  font (char *fname);
  virtual ~font ();
  
  void drawstring (char *string);  // renders a string with this font
};




// this class is a list of all strings that shall be printed in one frame
class stringlist
{
private:
  GLfloat px, py;    // position to print
  GLfloat color[4];  // color of the string
  char text[120];// text to be printed
  

public:
  stringlist *next;  // pointer to next
  GLuint fontnum;    // number of font string is to be printed in

  stringlist (GLfloat px, GLfloat py, char *txt, GLuint fontnum, GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca);
  virtual ~stringlist ();

  void add (stringlist* next);  // add a new string as next pointer
  void draw ();
};



// prototypes

void loadfontset ();
void print (GLfloat px, GLfloat py, char *txt, GLuint fontnum, GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca);
void drawstrings ();




#endif

