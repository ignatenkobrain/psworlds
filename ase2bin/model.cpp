// model.cpp
// The class that does all the work

#include "model.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* parse_path(char* s)
{
	unsigned int i=strlen(s)-1;
	while(i>=0 && s[i]!='\\') i--;
	char *s1 = (char*)malloc(strlen(s));
	for(unsigned int j=i+1;j<=strlen(s);j++)
	{
		s1[j-i-1] = s[j];
	}
	return s1;
}

model::model(char* filename, char* output) {
	s = fopen (filename, "r");
	if(output==NULL) {
		outputfile = (char *)malloc(sizeof(filename));
		strncpy(outputfile, filename, strlen(filename)-4);
		outputfile[strlen(filename)-4]='\0';
		strncat(outputfile,".obj",4);
	}
	else {
		outputfile = output;
	}
	Init();
    getCounts();
	allocate();
	if(numMaterials>0) getMaterials();
	getData();
	if(numMaterials>0) calculateTexCoords();
	fclose (s);
	calculate_boundings();
}

model::~model() {
	freemem();
}

// write a binary int
void model::write_int(FILE* f,int value) {
	fwrite(&value,sizeof(int),1,f);
}

// write a binary unsigned int
void model::write_sizet(FILE* f,size_t value) {
	fwrite(&value,sizeof(size_t),1,f);
}

// write a binary float
void model::write_float(FILE* f,float value) {
	fwrite(&value,sizeof(float),1,f);
}

//write a binary char
void model::write_char(FILE* f,char value) {
	fwrite(&value,sizeof(char),1,f);
}

// write the file to the outputfile in binary format
void model::write() {
	int i;
	size_t j;
	// open output file
	FILE *o = fopen(outputfile, "wb");
	// write materials
	if(numMaterials>0) {
		if(texture[material].numSubMat) {
			write_int(o,texture[material].numSubMat);
			for(i=0;i<texture[material].numSubMat;i++) {
				write_float(o,texture[material].submaterials[i].ambient[0]);
				write_float(o,texture[material].submaterials[i].ambient[1]);
				write_float(o,texture[material].submaterials[i].ambient[2]);
				write_float(o,texture[material].submaterials[i].diffuse[0]);
				write_float(o,texture[material].submaterials[i].diffuse[1]);
				write_float(o,texture[material].submaterials[i].diffuse[2]);
				write_float(o,texture[material].submaterials[i].specular[0]);
				write_float(o,texture[material].submaterials[i].specular[1]);
				write_float(o,texture[material].submaterials[i].specular[2]);
				write_float(o,texture[material].submaterials[i].shine);
				write_float(o,texture[material].submaterials[i].uTile);
				write_float(o,texture[material].submaterials[i].vTile);
				write_float(o,texture[material].submaterials[i].uOffset);
				write_float(o,texture[material].submaterials[i].vOffset);
				write_int(o,texture[material].submaterials[i].env);
				write_int(o,texture[material].submaterials[i].texmap);
				write_sizet(o,strlen(texture[material].submaterials[i].texName));
				for(j=0;j<strlen(texture[material].submaterials[i].texName);j++)
				{
					write_char(o,texture[material].submaterials[i].texName[j]);
				}
			}
		}
		else {
			write_int(o,1);
			write_float(o,texture[material].ambient[0]);
			write_float(o,texture[material].ambient[1]);
			write_float(o,texture[material].ambient[2]);
			write_float(o,texture[material].diffuse[0]);
			write_float(o,texture[material].diffuse[1]);
			write_float(o,texture[material].diffuse[2]);
			write_float(o,texture[material].specular[0]);
			write_float(o,texture[material].specular[1]);
			write_float(o,texture[material].specular[2]);
			write_float(o,texture[material].shine);
			write_float(o,texture[material].uTile);
			write_float(o,texture[material].vTile);
			write_float(o,texture[material].uOffset);
			write_float(o,texture[material].vOffset);
			write_int(o,texture[material].env);
			write_int(o,texture[material].texmap);
			write_sizet(o,strlen(texture[material].texName));
			for(j=0;j<strlen(texture[material].texName);j++)
			{
				write_char(o,texture[material].texName[j]);
			}
		}
	}
	// write vertices
	write_int(o,numVertex);
	for(i=0;i<numVertex;i++) {
		write_float(o,verts[i][0]);
		write_float(o,verts[i][1]);
		write_float(o,verts[i][2]);
	}
	// write tex vertices
	write_int(o,numTexVertex);
	for(i=0;i<numTexVertex;i++) {
		write_float(o,texVerts[i][0]);
		write_float(o,texVerts[i][1]);
	}
	// write faces
	write_int(o,numFaces);
	for(i=0;i<numFaces;i++) {
		if(numMaterials>0) {
			if(texture[material].numSubMat) {
				write_int(o,faces[i].material%texture[material].numSubMat);
			}
			else {
				write_int(o,material);
			}
		}
		else {
			write_int(o,-1);
		}
		write_int(o,faces[i].vertIndex[0]);
		write_int(o,faces[i].vertIndex[1]);
		write_int(o,faces[i].vertIndex[2]);
		write_int(o,faces[i].coordIndex[0]);
		write_int(o,faces[i].coordIndex[1]);
		write_int(o,faces[i].coordIndex[2]);
		write_float(o,faces[i].normal[0]);
		write_float(o,faces[i].normal[1]);
		write_float(o,faces[i].normal[2]);
		write_float(o,faces[i].vertNormals[0][0]);
		write_float(o,faces[i].vertNormals[0][1]);
		write_float(o,faces[i].vertNormals[0][2]);
		write_float(o,faces[i].vertNormals[1][0]);
		write_float(o,faces[i].vertNormals[1][1]);
		write_float(o,faces[i].vertNormals[1][2]);
		write_float(o,faces[i].vertNormals[2][0]);
		write_float(o,faces[i].vertNormals[2][1]);
		write_float(o,faces[i].vertNormals[2][2]);
	}
	// write bounding sphere radius
	write_float(o,sphere_rad);
	// write bounding box values
	write_float(o,minx);
	write_float(o,maxx);
	write_float(o,miny);
	write_float(o,maxy);
	write_float(o,minz);
	write_float(o,maxz);
	// close output file
	fclose(o);
}

// establishes the number of verts/faces/...
void model::getCounts() {

	char data[255];
	int tmp=0;
	rewind (s);

	while (!feof (s))
	{
		fscanf (s, "%s", &data);
		if (!strcmp (data, NUM_VERTEX))
			fscanf (s, "%d", &numVertex);
		else if (!strcmp (data, NUM_FACES))
			fscanf (s, "%d", &numFaces);
		else if (!strcmp (data, NUM_TVERTEX))
			fscanf (s, "%d", &numTexVertex);
		else if (!strcmp (data, NUM_TFACES))
			fscanf (s, "%d", &numTexFaces);
		else if (!strcmp (data, NUM_MAT))
			fscanf (s, "%d", &numMaterials);
		else if (!strcmp (data, MAT_REF))
			fscanf (s, "%d", &material);
		else fgets (data, sizeof (data), s);	// skip this line
	}	
}

// reads a vertex from the ase file
void model::getVertex () {
	int index;
	fscanf (s, "%d", &index);
	// swap y and z to get them right
	fscanf (s, "%f %f %f",&verts[index][0],&verts[index][2],&verts[index][1]);
	// in 3dsm negative z goes out of the screen, we want it to go in
	verts[index][2] = -verts[index][2];
}


// reads a texture coord from the ase file.  
void model::getTVertex () {
	int index;
	fscanf (s, "%d", &index);
	fscanf (s, "%f %f", &texVerts[index][0], &texVerts[index][1]);
}

// reads a vertex normal from the ase file
void model::getNVertex (int facei, int i) {
	int index;
	fscanf (s, "%d", &index);
	// swap y and z to get them right
	fscanf (s, "%f %f %f",&faces[facei].vertNormals[i][0],&faces[facei].vertNormals[i][2],&faces[facei].vertNormals[i][1]);
	// in 3dsm negative z goes out of the screen, we want it to go in
	faces[facei].vertNormals[i][2] = -faces[facei].vertNormals[i][2];
}


// reads a face normal from the ase file
void model::getFaceNormal () {
	int index;
	int i=0;
	char data[255];
	fscanf (s, "%d", &index);
	// swap y and z to get them right
	fscanf (s, "%f %f %f",&faces[index].normal[0],&faces[index].normal[2],&faces[index].normal[1]);
	// in 3dsm negative z goes out of the screen, we want it to go in
	faces[index].normal[2] = -faces[index].normal[2];
	// get vertex normals
	while(i<3) {
		fscanf (s, "%s" ,&data);
		while(strcmp(data,NVERTEX)) {
			fscanf (s, "%s" ,&data);
		}
		getNVertex(index,i);
		i++;
	}
}

// reads a face from the ase file
void model::getFace () {
	int index;
	char data[255];
	fscanf (s, "%d:", &index);
	fscanf (s, "\tA:\t%d B:\t%d C:\t%d",&faces[index].vertIndex[0],&faces[index].vertIndex[1],&faces[index].vertIndex[2]);
    fscanf (s, "%s" ,&data);
    while(strcmp(data,MTLID)) {
		fscanf (s, "%s" ,&data);
	}
	fscanf (s, "%d:", &faces[index].material);
}

// reads the indicies for a texture mapped face
void model::getTFace () {
	int index;
	fscanf (s, "%d", &index);
	fscanf (s, "%d %d %d",&faces[index].coordIndex[0],&faces[index].coordIndex[1],&faces[index].coordIndex[2]); 
}

// get ambient values of material
void model::getAmbient(int mati, int subi, int mat) {
	if(mat) {
		fscanf (s, "%f %f %f",&texture[mati].ambient[0],&texture[mati].ambient[1],&texture[mati].ambient[2]);
	}
	else {
		fscanf (s, "%f %f %f",&texture[mati].submaterials[subi].ambient[0],
							  &texture[mati].submaterials[subi].ambient[1],
							  &texture[mati].submaterials[subi].ambient[2]);
	}
}

// get diffuse values of material
void model::getDiffuse(int mati, int subi, int mat) {
	if(mat) {
		fscanf (s, "%f %f %f",&texture[mati].diffuse[0],&texture[mati].diffuse[1],&texture[mati].diffuse[2]);
	}
	else {
		fscanf (s, "%f %f %f",&texture[mati].submaterials[subi].diffuse[0],
							  &texture[mati].submaterials[subi].diffuse[1],
							  &texture[mati].submaterials[subi].diffuse[2]);
	}
}

// get specular values of material
void model::getSpecular(int mati, int subi, int mat) {
	if(mat) {
		fscanf (s, "%f %f %f",&texture[mati].specular[0],&texture[mati].specular[1],&texture[mati].specular[2]);
	}
	else {
		fscanf (s, "%f %f %f",&texture[mati].submaterials[subi].specular[0],
							  &texture[mati].submaterials[subi].specular[1],
							  &texture[mati].submaterials[subi].specular[2]);
	}
}

// reads in a texture name from the ase file
void model::getTextureName (int mati, int subi, int mat)
{
	char tmp[255];
	fscanf (s, " \"%s", tmp);
	tmp[strlen(tmp)-1] = '\0';
	if(mat) {

		texture[mati].texName = parse_path(tmp);
		texture[mati].texmap=1;
	}
	else {
		texture[mati].submaterials[subi].texName = parse_path(tmp);
		texture[mati].submaterials[subi].texmap=1;
	}
}


// reads a float value from the file
vec_t model::getFloatVal () {
	vec_t v;
	fscanf (s, " %f", &v);
	return v;
}


// gets material and texture informations from the ase file
void model::getMaterials() {
	char data[255];
	int index1=0;
	int index2=0;
	int mat=1;

	rewind(s);

	while(!feof (s)) {
		fscanf(s, "%s", &data);
		if (!strcmp (data, MATERIAL)) {
			fscanf (s, "%d", &index1);
			mat=1;
		}
		else if (!strcmp(data,NUM_SUBMAT)) {
			fscanf (s, "%d", &index2);
			texture[index1].submaterials = (subtex_t*) malloc (sizeof(subtex_t)*index2);
			texture[index1].numSubMat=index2;
			for(int i=0;i<index2;i++) {
				texture[index1].submaterials[i].texmap=0;
				texture[index1].submaterials[i].texName="\0";
				texture[index1].submaterials[i].env=0;
				texture[index1].submaterials[i].uOffset=0.0;
				texture[index1].submaterials[i].vOffset=0.0;
				texture[index1].submaterials[i].uTile=0.0;
				texture[index1].submaterials[i].vTile=0.0;
			}
		}
		else if (!strcmp(data,SUBMATERIAL)) {
			fscanf (s, "%d", &index2);
			mat=0;
		}
		else if (!strcmp (data, AMBIENT))
			getAmbient(index1,index2,mat);
		else if (!strcmp (data, DIFFUSE))
			getDiffuse(index1,index2,mat);
		else if (!strcmp (data, SPECULAR))
			getSpecular(index1,index2,mat);
		else if (!strcmp (data, SHINE)) {
			if(mat) {
				texture[index1].shine = getFloatVal();
			}
			else {
				texture[index1].submaterials[index2].shine = getFloatVal();
			}
		}
		else if (!strcmp (data, TEXTURE))
			getTextureName(index1,index2,mat);
		else if (!strcmp (data, UTILE))
			if(mat) {
				texture[index1].uTile = getFloatVal();
			}
			else {
				texture[index1].submaterials[index2].uTile = getFloatVal();
			}
		else if (!strcmp (data, VTILE))
			if(mat) {
				texture[index1].vTile = getFloatVal();
			}
			else {
				texture[index1].submaterials[index2].vTile = getFloatVal();
			}
		else if (!strcmp (data, UOFFSET))
			if(mat) {
				texture[index1].uOffset = getFloatVal();
			}
			else {
				texture[index1].submaterials[index2].uOffset = getFloatVal();
			}
		else if (!strcmp (data, VOFFSET))
			if(mat) {
				texture[index1].vOffset = getFloatVal();
			}
			else {
				texture[index1].submaterials[index2].vOffset = getFloatVal();
			}
		else if(!strcmp (data, MAP_CLASS))
		{
			fscanf(s," %s",&data);
			if(!strcmp (data, "\"Reflect/Refract\""))
			{
				if(mat)
				{
					texture[index1].env=1;
				}
				else
				{
					texture[index1].submaterials[index2].env=1;
				}
			}
		}
		else fgets (data, sizeof (data), s);
	}
}

// changes the texture coords depending on the material
void model::calculateTexCoords() {
	int i,j;
	int *changed = new int[numTexVertex];
	for(i=0;i<numTexVertex;i++) changed[i]=0;
	for(i=0;i<numTexFaces;i++) {
		for(j=0;j<3;j++) {
			if(!changed[faces[i].coordIndex[j]]) {
				if(texture[material].numSubMat>0) {
    				texVerts[faces[i].coordIndex[j]][0] *= texture[material].submaterials[faces[i].material%texture[material].numSubMat].uTile;
    				texVerts[faces[i].coordIndex[j]][0] *= texture[material].submaterials[faces[i].material%texture[material].numSubMat].vTile;
				}
				else {
    				texVerts[faces[i].coordIndex[j]][0] *= texture[material].uTile;
    				texVerts[faces[i].coordIndex[j]][1] *= texture[material].vTile;
				}
				changed[faces[i].coordIndex[j]]=1;
			}
		}
	}
	free(changed);
}

// gets all information needed from the ase file
void model::getData () {
	char data[255];

	rewind (s);

	while (!feof (s))
	{
		fscanf (s, "%s", &data);

		if (!strcmp (data, VERTEX))
			getVertex();
		else if (!strcmp (data, TVERTEX))
			getTVertex();
		else if (!strcmp (data, FACE_NORMAL))
			getFaceNormal();
		else if (!strcmp (data, FACE))
			getFace();
		else if (!strcmp (data, TFACE))
			getTFace();
		else fgets (data, sizeof (data), s);
	}
}


// allocate memory
void model::allocate () {
	int i;
	verts = (float (*)[3])malloc (sizeof(vec3_t)*numVertex);
	faces = (triangle_t*)malloc (sizeof(triangle_t)*numFaces);
	if (numTexVertex != 0)
		texVerts = (float (*)[2])malloc (sizeof(vec2_t)*numTexVertex);
	if (numMaterials != 0) {
		texture = (texture_t*)malloc (sizeof(texture_t)*numMaterials);
		for(i=0;i<numMaterials;i++) {
			texture[i].numSubMat=0;
			texture[i].submaterials=NULL;
			texture[i].texmap=0;
			texture[i].texName="\0";
			texture[i].env=0;
			texture[i].uOffset=0.0;
			texture[i].vOffset=0.0;
			texture[i].uTile=0.0;
			texture[i].vTile=0.0;
		}
	}

}

// releases all memory
void model::freemem () {
	if (verts != NULL) free (verts);
	if (faces != NULL) free (faces);
	if (texVerts != NULL) free (texVerts);
	for(int i=0;i<numMaterials;i++) {
		if(texture[i].submaterials!=NULL) free (texture[i].submaterials);
	}
	if (texture != NULL) free (texture);
}

// returns values of model
void model::Info () {
	printf ("# Model Info\n");
	printf ("# ------------------------------\n");
	printf ("# Num Vertices %d\n", numVertex);
	printf ("# Num Tex Vertices %d\n", numTexVertex);
	printf ("# Num Faces %d\n", numFaces);
	printf ("# Num Tex Faces %d\n", numTexFaces);
	printf ("# Num Materials %d\n", numMaterials);
	printf ("# Material ID %d\n", material);
	for(int i=0;i<numMaterials;i++) {
		printf ("# Num Submaterials %d\n", texture[i].numSubMat);
	}
}

// initializes all values
void model::Init () {
	numVertex = 0;
	numFaces = 0;
	numTexFaces = 0;
	numTexVertex = 0;
	numMaterials = 0;
	material = 0;
	verts = NULL;
	faces = NULL;
	texVerts = NULL;
    texture=NULL;
}

float model::distance(float x, float y, float z) {
	return (float)sqrt(x*x+y*y+z*z);
}


// calculates bounding sphere and box
void model::calculate_boundings() {
	float dist;
	sphere_rad=0;
	minx=0;
	maxx=0;
	miny=0;
	maxy=0;
	minz=0;
	maxz=0;
	for(int i=0;i<numVertex;i++) {
		if(verts[i][0]>maxx) maxx=verts[i][0];
		if(verts[i][0]<minx) minx=verts[i][0];
		if(verts[i][1]>maxy) maxy=verts[i][1];
		if(verts[i][1]<miny) miny=verts[i][1];
		if(verts[i][2]>maxz) maxz=verts[i][2];
		if(verts[i][2]<minz) minz=verts[i][2];
		dist = distance(verts[i][0],verts[i][1],verts[i][2]);
		if(dist>sphere_rad) sphere_rad=dist;
	}
}


