// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//


#include "fonts.h"


// some vars we need for the stringlist

stringlist	*lalista = NULL;	// the list itself - la lista :)
stringlist	*liststart = NULL;	// pointer to first list element
font		*fonts[4];			// 4 types of fonts should be enough for now


font::font(char *fname)
{
	// create 128 display lists out of a tga file containing the characters

	GLuint startline[10], endline[10], numlines=0, lin=0, i=0, j, k, alphas=0, inlineflag=0, incharflag=0;
	GLuint startx[63], starty[63], endx[63], endy[63];
	GLubyte *pix;
	GLfloat spacewidth, unitpixw, unitpixh, unittexw, unittexh;

    _FBUF

    sprintf(buf, "Loading %s..", fname); write_log(buf);

	// first, load the TGA
	// AAARGHH!! YOU NEED MIPMAPS OTHERWISE NOTHING WILL WORK!!
	// IT TOOK ME A WHOLE DAY TO FIGURE THAT OUT!!
	chars = new textureObj(fname, 1); 
	
	write_log("Scanning for lines..");

	// now get the start and end lines
	pix = chars->getPixels();
	while(i<chars->h)
	{
		// scan this line for any non-zero alphas
		j=0; alphas=0;
		while((j<chars->w) && (alphas==0))
		{
			if(pix[(i*chars->w+j)*4+3]>5) // some tolerance might be handy
				alphas=1;
			j++;
		}

		// now check what we´ve found out about this line
		if(inlineflag==0)
		{
			if(alphas==1) // we´re now on a new fontline
			{
				inlineflag=1;
				startline[lin] = i;
			}
		}
		else // we´re currently on a fontline
		{
			if(alphas==0) // but no more
			{
				inlineflag=0;
				endline[lin] = i;
				lin++;
			}
		}
		i++;
	}
	numlines = lin;
	lin = 0; k=0;

    write_log("Scanning for chars..");

	// now build the display lists by stepping thru our deduced fontlines
	// from left to right and cutting whenever we find a char separation (a vertical blank)

	incharflag=0;
	for(i=0; i<numlines; i++)
	{
		for(j=0; j<chars->w; j++)
		{
			// make vertical check
			k=startline[i]; alphas=0;
			while((k<endline[i]) && (alphas==0))
			{
				if(pix[(k*chars->w+j)*4+3]>5) // tolerance again
					alphas=1;
				k++;
			}

			// now decide on the results
			if(incharflag==0)
			{
				if(alphas==1) // new char found
				{
					incharflag=1;
					startx[lin]=j; starty[lin]=startline[i];
				}
			}
			else
			{
				if(alphas==0) // char ends here
				{
					incharflag=0;
					endx[lin]=j+1; endy[lin]=endline[i]; // add 1 pixel of padding
					lin++;
				}
			}
		}
	}


    write_log("Creating display lists..");

	// now that we´ve collected all the start and end coords for each character, map
	// each to a quad and store this letter as display list

	// first, find the appropriate width for a space char. derive it from char "A"/2
	spacewidth = endx['A'-33] - startx['A'-33];
	spacewidth/=2.0;

	// calc how much one pixel is in texture coords

	unitpixw = 2.0 / 640; // our fonts are optimized for 640x480 but will be equally 
	unitpixh = 1.6 / 480; // sized at ANY resolution
	unittexw = 1.0 / chars->w;
	unittexh = 1.0 / chars->h;

	displ = glGenLists(128);
	glListBase(displ);
	
	// generate display lists. all will be empty except chars 33 - 95, which are 
	// capital letters, numbers and signs
	for(i=0; i<128; i++)
	{
		glNewList(displ+i, GL_COMPILE);
			if((i>=33) && (i<=95))
			{
				// draw the real chars here
				j=i-33;
				glBegin(GL_QUADS);
					
					glTexCoord2f(startx[j]*unittexw, starty[j]*unittexh);
					glVertex3f(0.0, 0.0, -1.500001);
					
					glTexCoord2f(startx[j]*unittexw, endy[j]*unittexh);
					glVertex3f(0.0, (endy[j]-starty[j])*unitpixh, -1.500001);
					
					glTexCoord2f(endx[j]*unittexw, endy[j]*unittexh);
					glVertex3f((endx[j]-startx[j])*unitpixw, (endy[j]-starty[j])*unitpixh, -1.500001);
					
					glTexCoord2f(endx[j]*unittexw, starty[j]*unittexh);
					glVertex3f((endx[j]-startx[j])*unitpixw, 0.0, -1.500001);
				glEnd();
				glTranslatef((endx[j]-startx[j])*unitpixw, 0.0, 0.0);
			}
			else // just space
				glTranslatef(spacewidth*unitpixw, 0.0, 0.0);

		glEndList();
	}
    write_log("Done!\n");

}


font::~font()
{
	// free the display lists
	glListBase(displ);
	glDeleteLists(displ, 128);
}




stringlist::stringlist(GLfloat px, GLfloat py, char *txt, GLuint fontnum, GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca)
{
	// constructor
	this->px = px;
	this->py = py;
	strcpy(text,txt);
	this->fontnum = fontnum;
	color[0] = cr; color[1] = cg; color[2] = cb; color[3] = ca;
	this->next = NULL;
}


stringlist::~stringlist()
{
	// hmm
}

void stringlist::add(stringlist *next)
{
	this->next = next;
}

void stringlist::draw()
{
	// position matrix at starting coordinates, call color and the display lists
	glPushMatrix();
		glColor4fv(color);
		
		glTranslatef(px*PIXWIDTH-1.0, py*PIXHEIGHT-0.8, 0.0);
		glCallLists(strlen(text), GL_BYTE, text);
	glPopMatrix();
}


// font function wrappers

void loadfont(char *fname, int pos)
{
	// create the font object and pack it into the fontarray
	fonts[pos] = new font(fname);
}

void delfonts()
{
	// delete all them fonts
/*	delete (font[0])
	delete (font[1])
	delete (font[2])
	delete (font[3])*/

}


// string function wrappers

void print(GLfloat px, GLfloat py, char *txt, GLuint fontnum, GLfloat cr, GLfloat cg, GLfloat cb, GLfloat ca)
{
	stringlist *nu;
	py=abs(600-py);

	nu = new stringlist(px, py, txt, fontnum, cr, cg, cb, ca);
	
	// if this is the first string, set the listfirst pointer
	if(lalista == NULL)
	{
		liststart = nu;
		lalista = liststart;
	}
	else
	{
		// else just append this one
		lalista->add(nu);
		lalista = lalista->next;
	}
}


void drawstrings()
{
	// now´s the time to draw all the strings in the list
	stringlist *dummy;
	int fn, lastfn=-1;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	// run through the list of strings
	while(liststart!=NULL)
	{
		fn = liststart->fontnum;
		
		// check if we need to change the font texture
		if(fn!=lastfn)
		{
			glListBase(fonts[fn]->displ);
			glBindTexture(GL_TEXTURE_2D, fonts[fn]->chars->id);
			lastfn = fn;
		}
		lastfn = fn;

		liststart->draw();
		dummy = liststart->next;
		delete (liststart);	// clean up immediately
		liststart = dummy;
	}

	// for the next frame
	liststart = NULL;
	lalista = NULL;

	glDisable(GL_TEXTURE_2D);
}



void loadfontset()
{
	// loads all fonts used in Possible Worlds
	_FBUF
	
    write_log("Loading fonts..\n");

	sprintf(buf,"gfx/pssmallfont.tga"); fonts[0] = new font(buf);
	sprintf(buf,"gfx/samba_is_dead25.tga"); fonts[1] = new font(buf);
	sprintf(buf,"gfx/samba_is_dead15_narrow.tga"); fonts[2] = new font(buf);

    write_log("Fonts loaded!\n");
}
