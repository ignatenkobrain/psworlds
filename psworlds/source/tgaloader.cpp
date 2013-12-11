// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "tgaloader.h"


textureObj::textureObj(int w, int h, int cc, GLubyte* dat, int makemipmap)
{
	// make a texture out of an all ready image
	GLuint type;
	
    _FBUF

	if(cc==3) type = GL_RGB; else type = GL_RGBA;

	// register texture and build mipmaps
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

	if(makemipmap)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, cc, w, h, type, GL_UNSIGNED_BYTE, dat);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, cc, w, h, 0, type, GL_UNSIGNED_BYTE, dat);
	}
    sprintf(buf, "Generated inline texture. ID is %d\n", id); write_log(buf);
}


textureObj::textureObj(char *fname, int makemipmap)
{
	// load a tga image and pack it into openGL-usable format

	GLubyte TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};	// uncompressed header
	GLubyte TGAcompare[12];
	GLubyte header[6];
	GLuint bypp;	// bytes per pixel
	GLuint size;	// image size in bytes
	GLuint temp;
	GLuint type = GL_RGBA;	// default image type
	GLuint read;			// amount of bytes read
	short cc = 4;			// default color components count
	FILE *fp;
	GLuint i;
	
    _FBUF

	xpath(fname);

	// start
	fp = fopen(fname, "rb");

	// check if file exists and header is valid
	if(fp==NULL || fread(TGAcompare,sizeof(GLubyte),sizeof(TGAcompare),fp)!=sizeof(TGAcompare)
		|| memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0
		|| fread(header,sizeof(GLubyte),sizeof(header),fp)!=sizeof(header))
	{
		if(fp==NULL) exit(1);
		fclose(fp);
		exit(1);
	}

	// set up texture size
	w = header[0] + (header[1]<<8);
	h = header[2] + (header[3]<<8);

	// set up color depth
	bpp = header[4];
	bypp = header[4]>>3;

	// check size and color depth
	if(w <= 0 || h <=0 || (bpp !=24 && bpp != 32))
	{
		fclose(fp);
		exit(1);
	}

	if(bpp == 24) {type = GL_RGB; cc=3;}	// set texture type
	size = w * h * bypp;				// calc image size
	img = (GLubyte *)malloc(size);			// reserve some ram	
	read = fread(img, sizeof(GLubyte), size, fp);		// read pixels

	if(img==NULL ||  read != size) // read image data
	{
		if(img!=NULL) free(img);
		fclose(fp);
		exit(1);
	}

	// swap R and B bytes
	for(i=0; i<size; i+=bypp)
	{
		temp = img[i];
		img[i] = img[i+2];
		img[i+2] = temp;
	}
	fclose(fp);
	
	// register texture and build mipmaps
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

	if(makemipmap)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, cc, w, h, type, GL_UNSIGNED_BYTE, img);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, cc, w, h, 0, type, GL_UNSIGNED_BYTE, img);
	}

	sprintf(buf, "Loaded texture %s ID is %d\n", fname, id); write_log(buf);
}



textureObj::~textureObj()
{
	glDeleteTextures(1, &this->id);
}


GLubyte *textureObj::getPixels()
{
	return img;
}


