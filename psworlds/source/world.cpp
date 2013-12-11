// POSSIBLE WORLDS
// by Gerwin Sturm & Eike Umlauf
//
// This software is distributed under the terms of the GNU Public License
// as published by the Free Software Foundation.
//
// possibleworlds@eik.at
// http://possibleworlds.cjb.net
//

#include "world.h"
#include "fonts.h"
#include "camera.h"


#define RB 0.12
#define GB 0.21
#define BB 0.5


world::world(char *fname, float scale, float sealevel, float maxdepth, float mindepth, float wavheight, float wavspd)
{
	// load an 8 bit PCX file and create vertex data, normal data and 
	// texture coordinate data of it and store all data in arrays

	FILE *fp;
	unsigned char header[128];
	unsigned char *image;
	int xsize, ysize;
	int scanline, xpos;
	unsigned char inbyte;
	int length;
	_FBUF


	xpath(fname);

	// assign basic vars
	wavespeed = wavspd; waveheight = wavheight;
	this->setHaze(18);
	setdrawmode(1);
	wavecount=0.0;
	wave = 0.0;
	
	// load the PCX now
    sprintf(buf, "Attempting to load world file: %s ", fname); write_log(buf);

	// open fire
	fp = fopen(fname, "rb");
	if(fp == NULL)
	{
        sprintf(buf, "Could not open world file: %s\n", fname); write_log(buf);
		exit(1);
	}

	// read header
	fread(header, sizeof(char), 128, fp);

	// check if file is 256 colors
	if((header[3] != 8) || (header[68]!=2))
	{
        sprintf(buf, "World file %s is not a greyscale PCX\n", fname); write_log(buf);
		exit(1);
	}

	// get size
	xsize = (header[8]+(header[9]<<8)) - (header[4]+(header[5]<<8)) + 1;
	ysize = (header[10]+(header[11]<<8)) - (header[6]+(header[7]<<8)) + 1;
	image = (unsigned char *)malloc(xsize*ysize*sizeof(unsigned char));
	
	sprintf(buf, "...done! Image size is %d x %d\n", xsize, ysize); write_log(buf);


	// read pixels
	for(scanline=0; scanline<ysize; scanline++)
	{
		xpos=0; 
		while(xpos<xsize)
		{
			fread(&inbyte, 1, 1, fp); 
			if(feof(fp)) 
			{
				write_log("Unexpected EOF!\n"); exit(1);
			}
			
			// normal pixel data
			if(inbyte<=192) {image[xpos+scanline*xsize] = inbyte; xpos++;}
			else // RLE data
			{
				length = inbyte-192;
				fread(&inbyte, 1, 1, fp);
				if(feof(fp)) 
				{
					write_log("Unexpected EOF!\n"); exit(1);
				}
				memset(image+xpos+scanline*xsize, inbyte, length);
				xpos+=length;
			}
		}
	}

	// no need for palette read, palette is aligned ascendingly
	fclose(fp);

	// invoke terrain generator
	if(generateworld(xsize, ysize, scale, image)<0) exit(1);
	
	// generate the miniature map
	makeminimap(image);

	// now calculate the world limits
	this->sealev = sealevel;
	base = sealev * 0.27;
	this->maxdepth = maxdepth;
	this->mindepth = mindepth;
	
	// load the textures
	sprintf(buf, "gfx/seafloor2.tga"); texture = new textureObj(buf, 1);
	sprintf(buf, "gfx/water1.tga"); water1 = new textureObj(buf, 1);
	sprintf(buf, "gfx/water2.tga"); water2 = new textureObj(buf, 1);
	
	free(image);
}



world::~world()
{
	delete(texture);
	free(vertices);
	free(normals);
	free(texcoords);
}



int world::generateworld(int xsize, int ysize, float scale, unsigned char *img)
{
	// generate vertex-, normals- and texturecoordinates from greyscale PCX,
	// taking into account the height scale factor

	int x,y;
	GLfloat normal[6][3];	// for normal calculating
	GLfloat v1[3], v2[3], v3[3], res[3];	// help vectors
	//GLfloat *hprevec;		// all horizontal mesh vectors	mapping: V->XYZ
	//GLfloat *vprevec;		// all vertical mesh vectors	mapping: V->XYZ
	//GLfloat *prenorm;		// precalculated normals for each traingle face	mapping: F->XYZ
	GLfloat d;				// vector length
	int ind;

	write_log("Generating vertices.. ");

	xquads = xsize; zquads = ysize;

	// allocate vertex array memory and calculate vertices

	vertices = (GLfloat *)malloc((xsize)*(ysize)*3*sizeof(GLfloat));

	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			vertices[(y*xsize+x)*3] = x * MESHSIZE;
			vertices[(y*xsize+x)*3+2] = y * MESHSIZE;
			vertices[(y*xsize+x)*3+1] = img[y*xsize+x] * scale;
		}
	}

	// generate face list
	write_log("done!\nGenerating faces.. ");
	xsize-=1; ysize-=1;

	faces = (GLuint *)malloc(xsize*ysize*2*3*sizeof(GLuint));	// 2 triangles make one quad

	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			// define counterclockwise
			// left triangle
			faces[(y*xsize+x)*6] = y*(xsize+1)+x;
			faces[(y*xsize+x)*6+1] = (y+1)*(xsize+1)+x;
			faces[(y*xsize+x)*6+2] = (y+1)*(xsize+1)+x+1;

			// right triangle
			faces[(y*xsize+x)*6+3] = y*(xsize+1)+x;
			faces[(y*xsize+x)*6+4] = (y+1)*(xsize+1)+x+1;
			faces[(y*xsize+x)*6+5] = y*(xsize+1)+x+1;			
		}
	}

	// generate normals
	write_log("done!\nGenerating normals.. ");
	//hprevec = (GLfloat *)malloc(xsize*(ysize+1)*3*sizeof(GLfloat));
	//vprevec = (GLfloat *)malloc((xsize+1)*ysize*3*sizeof(GLfloat));
	//prenorm = (GLfloat *)malloc(xsize*ysize*2*3*sizeof(GLfloat));

	normals = (GLfloat *)malloc((xsize+1)*(ysize+1)*3*sizeof(GLfloat));

	xsize++; ysize++;

	// X-PERIMANTAL!!!! direct normal calculation!
	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			// left and up left triangle
			if((x>0) && (y>0))
			{
				// left vector
				v1[0] = vertices[y*xsize*3+(x-1)*3] - vertices[y*xsize*3+x*3];
				v1[1] = vertices[y*xsize*3+(x-1)*3+1] - vertices[y*xsize*3+x*3+1];
				v1[2] = vertices[y*xsize*3+(x-1)*3+2] - vertices[y*xsize*3+x*3+2];

				// up left vector
				v2[0] = vertices[(y-1)*xsize*3+(x-1)*3] - vertices[y*xsize*3+x*3];
				v2[1] = vertices[(y-1)*xsize*3+(x-1)*3+1] - vertices[y*xsize*3+x*3+1];
				v2[2] = vertices[(y-1)*xsize*3+(x-1)*3+2] - vertices[y*xsize*3+x*3+2];

				// up vector
				v3[0] = vertices[(y-1)*xsize*3+x*3] - vertices[y*xsize*3+x*3];
				v3[1] = vertices[(y-1)*xsize*3+x*3+1] - vertices[y*xsize*3+x*3+1];
				v3[2] = vertices[(y-1)*xsize*3+x*3+2] - vertices[y*xsize*3+x*3+2];

				// calc normal for upper left triangle
				normcrossprod(v1,v2,res);
				normal[0][0] = res[0]; normal[0][1] = res[1]; normal[0][2] = res[2];

				normcrossprod(v2,v3,res);
				normal[1][0] = res[0]; normal[1][1] = res[1]; normal[1][2] = res[2];
			}
			else
			{
				normal[0][0] = 0.0; normal[0][1] = 1.0; normal[0][2] = 0.0;
				normal[1][0] = 0.0; normal[1][1] = 1.0; normal[1][2] = 0.0;
			}

			// upper right triangle
			if((y>0) && (x<xsize-1))
			{
				// up vector
				v1[0] = vertices[(y-1)*xsize*3+x*3] - vertices[y*xsize*3+x*3];
				v1[1] = vertices[(y-1)*xsize*3+x*3+1] - vertices[y*xsize*3+x*3+1];
				v1[2] = vertices[(y-1)*xsize*3+x*3+2] - vertices[y*xsize*3+x*3+2];

				// right vector
				v2[0] = vertices[y*xsize*3+(x+1)*3] - vertices[y*xsize*3+x*3];
				v2[1] = vertices[y*xsize*3+(x+1)*3+1] - vertices[y*xsize*3+x*3+1];
				v2[2] = vertices[y*xsize*3+(x+1)*3+2] - vertices[y*xsize*3+x*3+2];

				normcrossprod(v1,v2,res);
				
				normal[2][0] = res[0]; normal[2][1] = res[1]; normal[2][2] = res[2];
			}
			else
			{
				normal[2][0] = 0.0; normal[2][1] = 1.0; normal[2][2] = 0.0;
			}

			// lower left triangle
			if((y<ysize-1) && (x>0))
			{
				// left vector
				v1[0] = vertices[y*xsize*3+(x-1)*3] - vertices[y*xsize*3+x*3];
				v1[1] = vertices[y*xsize*3+(x-1)*3+1] - vertices[y*xsize*3+x*3+1];
				v1[2] = vertices[y*xsize*3+(x-1)*3+2] - vertices[y*xsize*3+x*3+2];

				// down vector
				v2[0] = vertices[(y+1)*xsize*3+x*3] - vertices[y*xsize*3+x*3];
				v2[1] = vertices[(y+1)*xsize*3+x*3+1] - vertices[y*xsize*3+x*3+1];
				v2[2] = vertices[(y+1)*xsize*3+x*3+2] - vertices[y*xsize*3+x*3+2];

				normcrossprod(v1,v2,res);

				normal[3][0] = res[0]; normal[3][1] = res[1]; normal[3][2] = res[2];
			}
			else
			{
				normal[3][0] = 0.0; normal[3][1] = 1.0; normal[3][2] = 0.0;
			}

			// lower middle and right triangle
			if((y<ysize-1) && (x<xsize-1))
			{
				// down vector
				v1[0] = vertices[(y+1)*xsize*3+x*3] - vertices[y*xsize*3+x*3];
				v1[1] = vertices[(y+1)*xsize*3+x*3+1] - vertices[y*xsize*3+x*3+1];
				v1[2] = vertices[(y+1)*xsize*3+x*3+2] - vertices[y*xsize*3+x*3+2];

				// down right vector
				v2[0] = vertices[(y+1)*xsize*3+(x+1)*3] - vertices[y*xsize*3+x*3];
				v2[1] = vertices[(y+1)*xsize*3+(x+1)*3+1] - vertices[y*xsize*3+x*3+1];
				v2[2] = vertices[(y+1)*xsize*3+(x+1)*3+2] - vertices[y*xsize*3+x*3+2];

				// right vector
				v3[0] = vertices[y*xsize*3+(x+1)*3] - vertices[y*xsize*3+x*3];
				v3[1] = vertices[y*xsize*3+(x+1)*3+1] - vertices[y*xsize*3+x*3+1];
				v3[2] = vertices[y*xsize*3+(x+1)*3+2] - vertices[y*xsize*3+x*3+2];

				normcrossprod(v1,v2,res);
				normal[4][0] = res[0]; normal[4][1] = res[1]; normal[4][2] = res[2];
				
				normcrossprod(v2,v3,res);
				normal[5][0] = res[0]; normal[5][1] = res[1]; normal[5][2] = res[2]; 
			}
			else
			{
				normal[4][0] = 0.0; normal[4][1] = 1.0; normal[4][2] = 0.0;
				normal[5][0] = 0.0; normal[5][1] = 1.0; normal[5][2] = 0.0;
			}


			// now that we´ve collected all 6 adjacent normal vectors, interpolate them
			ind = (y*xsize*3+x*3);
			normals[ind] = normal[0][0]+normal[1][0]+normal[2][0]+normal[3][0]+normal[4][0]+normal[5][0];
			normals[ind+1] = normal[0][1]+normal[1][1]+normal[2][1]+normal[3][1]+normal[4][1]+normal[5][1];
			normals[ind+2] = normal[0][2]+normal[1][2]+normal[2][2]+normal[3][2]+normal[4][1]+normal[5][1];

			d = sqrt(normals[ind]*normals[ind] + normals[ind+1]*normals[ind+1] + normals[ind+2]*normals[ind+2]);
			normals[ind]/=d;
			normals[ind+1]/=d;
			normals[ind+2]/=d;
		}
	}
/*
	xsize--; ysize--;


	// precalc all mesh vectors to speed up normal generating
	for(y=0; y<=ysize; y++)
	{
		for(x=0; x<=xsize; x++)
		{
			// to do smooth normal generating, we need the vectors of all 
			// adjacent edges

			if(y<ysize) // south
			{
				vprevec[(y*(xsize+1)+x)*3] = vertices[(y*(xsize+1)+x)*3] - vertices[((y+1)*(xsize+1)+x)*3];
				vprevec[(y*(xsize+1)+x)*3+1] = vertices[(y*(xsize+1)+x)*3+1] - vertices[((y+1)*(xsize+1)+x)*3+1];
				vprevec[(y*(xsize+1)+x)*3+2] = vertices[(y*(xsize+1)+x)*3+2] - vertices[((y+1)*(xsize+1)+x)*3+2];
			}
			
			if(x<xsize) // east
			{
				hprevec[(y*(xsize)+x)*3] = vertices[(y*(xsize+1)+x)*3] - vertices[(y*(xsize+1)+x+1)*3];
				hprevec[(y*(xsize)+x)*3+1] = vertices[(y*(xsize+1)+x)*3+1] - vertices[(y*(xsize+1)+x+1)*3+1];
				hprevec[(y*(xsize)+x)*3+2] = vertices[(y*(xsize+1)+x)*3+2] - vertices[(y*(xsize+1)+x+1)*3+2];
			}
	
		}
	}


	// now calculate the normal vectors for each traingle
	fprintf(logfile, "Mesh-vector precalc done.. ");

	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			// left triangle
			v1[0] = vprevec[(y*(xsize+1)+x)*3]; v1[1] = vprevec[(y*(xsize+1)+x)*3+1]; v1[2] = vprevec[(y*(xsize+1)+x)*3+2];
			v2[0] = hprevec[((y+1)*xsize+x)*3]; v2[1] = hprevec[((y+1)*xsize+x)*3+1]; v2[2] = hprevec[((y+1)*xsize+x)*3+2];
			normcrossprod(v1, v2, res); // apply corkscrew-law
			prenorm[(y*xsize+x)*6] = res[0]; prenorm[(y*xsize+x)*6+1] = res[1]; prenorm[(y*xsize+x)*6+2] = res[2];
			
			// right triangle
			v1[0] = vprevec[(y*(xsize+1)+x+1)*3]; v1[1] = vprevec[(y*(xsize+1)+x+1)*3+1]; v1[2] = vprevec[(y*(xsize+1)+x+1)*3+2];
			v2[0] = hprevec[(y*xsize+x)*3]; v2[1] = hprevec[(y*xsize+x)*3+1]; v2[2] = hprevec[(y*xsize+x)*3+2];
			normcrossprod(v1, v2, res);
			prenorm[(y*xsize+x)*6+3] = res[0]; prenorm[(y*xsize+x)*6+4] = res[1]; prenorm[(y*xsize+x)*6+5] = res[2];
		}
	}

	// now interpolate the normals to one normal per vertex
	fprintf(logfile, "Quad (triangle) normals precalc done.. ");
	xsize++; ysize++;

	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			// upper left and middle  vector
			if((y>0) && (x>0))
			{
				normal[0][0] = prenorm[((y-1)+xsize+x-1)*6];
				normal[0][1] = prenorm[((y-1)+xsize+x-1)*6+1];
				normal[0][2] = prenorm[((y-1)+xsize+x-1)*6+2];
				normal[1][0] = prenorm[((y-1)+xsize+x-1)*6+3];
				normal[1][1] = prenorm[((y-1)+xsize+x-1)*6+4];
				normal[1][2] = prenorm[((y-1)+xsize+x-1)*6+5];
			}
			else
			{
				normal[0][0] = 0.0; normal[0][1] = 1.0; normal[0][2] = 0.0;
				normal[1][0] = 0.0; normal[1][1] = 1.0; normal[1][2] = 0.0;
			}

			// upper right right vector
			if((y>0) && (x<xsize-1))
			{
				normal[2][0] = prenorm[((y-1)+xsize+x)*6];
				normal[2][1] = prenorm[((y-1)+xsize+x)*6+1];
				normal[2][2] = prenorm[((y-1)+xsize+x)*6+2];
			}
			else
			{
				normal[2][0] = 0.0; normal[2][1] = 1.0; normal[2][2] = 0.0;
			}

			// lower left vector
			if((y<ysize-1) && (x>0))
			{
				normal[3][0] = prenorm[(y+xsize+x-1)*6+3];
				normal[3][1] = prenorm[(y+xsize+x-1)*6+4];
				normal[3][2] = prenorm[(y+xsize+x-1)*6+5];
			}
			else
			{
				normal[3][0] = 0.0; normal[3][1] = 1.0; normal[3][2] = 0.0;
			}

			// lower middle and right vector
			if((y<ysize-1) && (x<xsize-1))
			{
				normal[4][0] = prenorm[(y+xsize+x)*6];
				normal[4][1] = prenorm[(y+xsize+x)*6+1];
				normal[4][2] = prenorm[(y+xsize+x)*6+2];
				normal[5][0] = prenorm[(y+xsize+x)*6+3];
				normal[5][1] = prenorm[(y+xsize+x)*6+4];
				normal[5][2] = prenorm[(y+xsize+x)*6+5];
			}
			else
			{
				normal[4][0] = 0.0; normal[4][1] = 1.0; normal[4][2] = 0.0;
				normal[5][0] = 0.0; normal[5][1] = 1.0; normal[5][2] = 0.0;
			}


			// now that we´ve collected all 6 adjacent normal vectors, interpolate them
			ind = (y*xsize*3+x*3);
			normals[ind] = normal[0][0]+normal[1][0]+normal[2][0]+normal[3][0]+normal[4][0]+normal[5][0];
			normals[ind+1] = normal[0][1]+normal[1][1]+normal[2][1]+normal[3][1]+normal[4][1]+normal[5][1];
			normals[ind+2] = normal[0][2]+normal[1][2]+normal[2][2]+normal[3][2]+normal[4][1]+normal[5][1];

			d = sqrt(normals[ind]*normals[ind] + normals[ind+1]*normals[ind+1] + normals[ind+2]*normals[ind+2]);
			normals[ind]/=d;
			normals[ind+1]/=d;
			normals[ind+2]/=d;
		}
	}

	// generate texture coordinates
//	fprintf(logfile, "Calculating Vertex normals done!\nCalculating Texture Coordinates..");
*/
	write_log("Calculating Vertex normals done!\nCalculating Texture Coordinates..");
	xsize--; ysize--;
	texcoords = (GLfloat *)malloc(xsize*ysize*2*3*3*sizeof(GLfloat));

	for(y=0; y<ysize; y++)
	{
		for(x=0; x<xsize; x++)
		{
			ind = (y*xsize+x)*12;
			// simple static mapping for now
			// left triangle
			texcoords[ind+0+0+0] = 0.0; texcoords[ind+0+0+1] = 0.0;
			texcoords[ind+0+2+0] = 0.0; texcoords[ind+0+2+1] = 1.0;
			texcoords[ind+0+4+0] = 1.0; texcoords[ind+0+4+1] = 0.0;
			
			// right triangle
			texcoords[ind+6+0+0] = 0.0; texcoords[ind+6+0+1] = 0.0;
			texcoords[ind+6+2+0] = 0.0; texcoords[ind+6+2+1] = 1.0;
			texcoords[ind+6+4+0] = 1.0; texcoords[ind+6+4+1] = 0.0;
		}
	}

	// calculate limits

	xwidth = xsize*MESHSIZE;
	zwidth = ysize*MESHSIZE;

	sizex = xsize*6; sizez = ysize;
	// depth settings stated in missionfile

	// now prepare openGL for array drawing. which will be scrapped soon
	write_log("Preparing OpenGL for VertexArrays.. ");

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0,  normals);
	//glTexCoordPointer(2, GL_FLOAT, 0, texcoords);



    write_log("done! Setting up Terrain complete!\n");
	

	return 0;
}


GLuint world::getHaze()
{
	return haze;
}

void world::setHaze(GLuint distance)
{
	haze = distance;
}



int world::draw(GLfloat x, GLfloat y, GLfloat z, GLfloat xrot, GLfloat yrot, GLfloat zrot, int pass)
{
	// draw the seafloor
	// TODO: clean up the whole thing a bit

	int gridx, gridz;
	int ax, az;
	int xmin, xmax, zmin, zmax, linesize;
	int iz, ix;
	
	// material for seafloor
	GLfloat fdiffuse[] = {0.6, 0.48, 0.2, 1.0};
	GLfloat fmspec[] = {0.6, 0.48, 0.2, 1.0 };
	GLfloat fmshiny[] = {50.0};
	GLfloat famb[] = { 0.1, 0.1, 0.03, 1.0};
	
	// material for water
	GLfloat wdiffuse[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat wmspec[] = {0.8, 0.8, 0.8, 1.0 };
	GLfloat wmshiny[] = {40.0};
	GLfloat wamb[] = { 0.5, 0.5, 0.5, 1.0};
	
	// time and texture id for water
	GLfloat ticks = gettime();
	GLuint waterid;
	GLfloat wh;	// wave height

	// position light source
	litepos[0] = 1.0;
	litepos[1] = 1.0;
	litepos[2] = 0.0;
	litepos[3] = 0.0;

	// map calculations
	gridx = x/MESHSIZE;
	gridz = z/MESHSIZE;
	
	ax=gridx*6;
	az=gridz;
	
	// TODO: filter dependent on axis rotation

	// first of all, position the light
	glLightfv(GL_LIGHT0, GL_POSITION, litepos);

	// filter faces we don´t see
	xmin = (int)(ax-haze*6); xmax = (int)(ax+haze*6);
	zmin = (int)(az-haze); zmax = (int)(az+haze);

	// check the limits
	if(xmin<0) xmin=0;
	if(xmax<0) xmax=0;
	if(xmax>(int)sizex) xmax=sizex;
	if(xmin>(int)sizex) xmin=sizex;
	if(zmin<0) zmin=0;
	if(zmax<0) zmax=0;
	if(zmax>(int)sizez) zmax=sizez;
	if(zmin>(int)sizez) zmin=sizez;
	linesize = xmax-xmin;

	// now acces the vertex, face, normals and texcoord arrays
	glPushMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	if(pass==1)
	{
		fmspec[3]=0.5; famb[3]=0.5; fdiffuse[3]=0.5;
		glColor4f(1.0, 1.0, 1.0, 0.5);
	}
	glMaterialfv(GL_FRONT, GL_SPECULAR, fmspec);
	glMaterialfv(GL_FRONT, GL_SHININESS, fmshiny);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fdiffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, famb);

	// bind the seafloor texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	if(pass==1)
	{
		glBindTexture(GL_TEXTURE_2D, this->water1->id);
	}


	// array drawing! fast!!!
	if(drawmode==0)
	{
		for(iz=zmin; iz<zmax; iz++)
		{
			glDrawElements(GL_TRIANGLES, linesize, GL_UNSIGNED_INT, faces+(iz*sizex)+xmin);
		}
	}
	else // vertex drawing! should be slower.... but it´s not!
	{
		int v1, v2, v3;
		GLfloat ve1[3], ve2[3], ve3[3];
		GLfloat n1[3], n2[3], n3[3];
		
		glBegin(GL_TRIANGLES);

		for(iz=zmin; iz<zmax; iz++)
		{
			
			for(ix=xmin; ix<xmax; ix+=3)
			{
				// get needed vertex indices
				
				v1 = faces[iz*sizex+ix];
				v2 = faces[iz*sizex+ix+1];
				v3 = faces[iz*sizex+ix+2];
				
				ve1[0] = vertices[v1*3]; ve1[1] = vertices[v1*3+1]; ve1[2] = vertices[v1*3+2];
				ve2[0] = vertices[v2*3]; ve2[1] = vertices[v2*3+1]; ve2[2] = vertices[v2*3+2];
				ve3[0] = vertices[v3*3]; ve3[1] = vertices[v3*3+1]; ve3[2] = vertices[v3*3+2];
				
				n1[0] = normals[v1*3]; n1[1] = normals[v1*3+1]; n1[2] = normals[v1*3+2];
				n2[0] = normals[v2*3]; n2[1] = normals[v2*3+1]; n2[2] = normals[v2*3+2];
				n3[0] = normals[v3*3]; n3[1] = normals[v3*3+1]; n3[2] = normals[v3*3+2];

				// draw that stuff!
				glNormal3fv(n1);
				glTexCoord2i(0,1); 
				glVertex3fv(ve1);
				glNormal3fv(n2);
				glTexCoord2i(0,0);
				glVertex3fv(ve2);
				glNormal3fv(n3);
				glTexCoord2i(1,1);
				glVertex3fv(ve3);
				
			}	
		}
		glEnd();
		
	}

	// now draw the water surface

	// texture swap counter
	wavecount+=ticks;
	if(wavecount<0.8) waterid = water1->id;
	else if(wavecount<1.6) waterid = water2->id;
	else { while(wavecount>1.6) wavecount-=1.6; waterid = water1->id; }

	// wave height
	wave+=ticks*wavespeed;
	while(wave>360.0) wave-=360.0;
	wh = waveheight*sin(wave*GRAD_PI)+sealev+waveheight;


	// bind texture and draw one huge quad
	GLfloat tmult = MESHSIZE*2;
	glColor4f(0.7, 0.7, 0.7, 0.7);
	glMaterialfv(GL_FRONT, GL_SPECULAR, wmspec);
	glMaterialfv(GL_FRONT, GL_SHININESS, wmshiny);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wdiffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, wamb);

	// the water can be viewed from above and below
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, waterid);
	glBegin(GL_QUADS);
		glNormal3f(0.0,1.0,0.0);
		glTexCoord2f((x-1000.0)/tmult, (z-1000.0)/tmult);
		glVertex3f(x-1000.0, wh, z-1000.0);
		glTexCoord2f((x)/tmult, (z-1000.0)/tmult);
		glVertex3f(x, wh, z-1000.0);
		glTexCoord2f((x)/tmult, (z)/tmult);
		glVertex3f(x, wh, z);
		glTexCoord2f((x-1000.0)/tmult, (z)/tmult);
		glVertex3f(x-1000.0, wh, z);

		glNormal3f(0.0,1.0,0.0);
		glTexCoord2f((x)/tmult, (z-1000.0)/tmult);
		glVertex3f(x, wh, z-1000.0);
		glTexCoord2f((x+1000.0)/tmult, (z-1000.0)/tmult);
		glVertex3f(x+1000.0, wh, z-1000.0);
		glTexCoord2f((x+1000.0)/tmult, (z)/tmult);
		glVertex3f(x+1000.0, wh, z);
		glTexCoord2f((x)/tmult, (z)/tmult);
		glVertex3f(x, wh, z);

		glNormal3f(0.0,1.0,0.0);
		glTexCoord2f((x)/tmult, (z)/tmult);
		glVertex3f(x, wh, z);
		glTexCoord2f((x+1000.0)/tmult, (z)/tmult);
		glVertex3f(x+1000.0, wh, z);
		glTexCoord2f((x+1000.0)/tmult, (z+1000.0)/tmult);
		glVertex3f(x+1000.0, wh, z+1000.0);
		glTexCoord2f((x)/tmult, (z+1000.0)/tmult);
		glVertex3f(x, wh, z+1000.0);

		glNormal3f(0.0,1.0,0.0);
		glTexCoord2f((x-1000.0)/tmult, (z)/tmult);
		glVertex3f(x-1000.0, wh, z);
		glTexCoord2f((x)/tmult, (z)/tmult);
		glVertex3f(x, wh, z);
		glTexCoord2f((x)/tmult, (z+1000.0)/tmult);
		glVertex3f(x, wh, z+1000.0);
		glTexCoord2f((x-1000.0)/tmult, (z+1000.0)/tmult);
		glVertex3f(x-1000.0, wh, z+1000.0);
	glEnd();

	glEnable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
	return 0;
}


void world::makeminimap(GLubyte* data)
{
	// generate a mini map of the seafloor file
	GLfloat xstep, ystep;
	GLfloat xpos, ypos;
	int i,j;

	write_log("Generating minimap..");

	// calc sampling steps
	xstep = xquads/256.0;
	ystep = zquads/256.0;

	// allocate some RAM for the minimap
	minimap = (GLubyte*)malloc(256*256*3);

	// probe!
	ypos = 0.0;
	for(j=0; j<256; j++)
	{
		xpos = 0.0;
		for(i=0; i<256; i++)
		{
			minimap[j*3*256+i*3] = data[(int)((int)(ypos)*xquads+(int)xpos)]*0.2;
			minimap[j*3*256+i*3+1] = data[(int)((int)(ypos)*xquads+(int)xpos)]*0.9;
			minimap[j*3*256+i*3+2] = data[(int)((int)(ypos)*xquads+(int)xpos)]*0.2;
			xpos+=xstep;
		}
		ypos+=ystep;
	}

    write_log("done!\n");
}


void world::adjustlight()
{
	// adjust the fog, clear color and lighting according
	// to the camera´s depth and the world water attributes (TODO)
	GLfloat depth;
	GLfloat fcol[4];
	GLfloat litediff[] = { 0.42, 0.42, 1.0, 1.0 },
			litespec[] = { 0.65, 0.65, 1.0, 1.0 },
			liteamb[] = { 0.2, 0.2, 0.9, 1.0 };
	Camera* cam = (Camera*)getcam();
	

	depth = cam->getdepth();

	// calc fog and clear col
	fcol[0] = RB-depth/5000.0;
	fcol[1] = GB-depth/7500.0;
	fcol[2] = BB-depth/9000.0;
	fcol[3] = 1.0;
	if(fcol[2]<0.20) fcol[2]=0.20;

	// set fog & clear color
	glFogfv(GL_FOG_COLOR, fcol);
	glClearColor(fcol[0], fcol[1], fcol[2], fcol[3]);


	// calc lighting
	
	// diffuse first
	litediff[0]-=depth/3200.0;
	litediff[1]-=depth/3200.0;
	litediff[2]-=depth/2160.0;
	if(litediff[0]<0.05) litediff[0]=0.05;
	if(litediff[1]<0.05) litediff[1]=0.05;
	if(litediff[2]<0.3) litediff[2]=0.3;

	// specular lighting
	litespec[0]-=depth/3060.0;
	litespec[1]-=depth/3060.0;
	litespec[2]-=depth/2600.0;
	if(litespec[0]<litediff[0]) litespec[0] = litediff[0];
	if(litespec[1]<litediff[1]) litespec[1] = litediff[1];
	if(litespec[2]<litediff[2]) litespec[2] = litediff[2];

	// ambient
	liteamb[0]-=depth/7500.0;
	liteamb[1]-=depth/7500.0;
	liteamb[2]-=depth/3200.0;
	if(liteamb[2]<0.2) liteamb[2] = 0.2;

	// set light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, litediff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, litespec);
	glLightfv(GL_LIGHT0, GL_AMBIENT, liteamb);

}




// providers

int world::getdrawmode()
{
	return drawmode;
}

void world::setdrawmode(int mode)
{
	drawmode = mode;
}


int world::getsizex()
{
	return sizex;
}


int world::getsizez()
{
	return sizez;
}

GLfloat* world::getvertices()
{
	return vertices;
}

GLuint* world::getfaces()
{
	return faces;
}

GLuint world::getbase()
{
	return base;
}

GLfloat world::getxwidth()
{
	return xwidth;
}

GLfloat world::getzwidth()
{
	return zwidth;
}

GLfloat world::getmaxdepth()
{
	return maxdepth;
}

GLfloat world::getmindepth()
{
	return mindepth;
}

GLfloat world::getlevel()
{
	return sealev;
}

GLubyte* world::getminimap()
{
	return minimap;
}

