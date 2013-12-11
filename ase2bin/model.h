// model.h
// Header for the class which does all the work

#ifndef __MODEL_H_
#define __MODEL_H_

#include <stdio.h>

// strings needed for parsing the ase-file
#define NUM_VERTEX			"*MESH_NUMVERTEX"
#define NUM_FACES			"*MESH_NUMFACES"
#define NUM_TVERTEX			"*MESH_NUMTVERTEX"
#define NUM_TFACES			"*MESH_NUMTVFACES"
#define NUM_MAT				"*MATERIAL_COUNT"
#define NUM_SUBMAT			"*NUMSUBMTLS"
#define VERTEX_LIST			"*MESH_VERTEX_LIST"
#define VERTEX				"*MESH_VERTEX"
#define FACE_LIST			"*MESH_FACE_LIST"
#define FACE				"*MESH_FACE"
#define MTLID				"*MESH_MTLID"
#define NORMALS				"*MESH_NORMALS"
#define FACE_NORMAL			"*MESH_FACENORMAL"
#define NVERTEX				"*MESH_VERTEXNORMAL"
#define TVERTEX				"*MESH_TVERT"
#define TFACE				"*MESH_TFACE"
#define MATERIAL			"*MATERIAL"
#define SUBMATERIAL			"*SUBMATERIAL"
#define MAT_REF				"*MATERIAL_REF"
#define AMBIENT				"*MATERIAL_AMBIENT"
#define DIFFUSE				"*MATERIAL_DIFFUSE"
#define SPECULAR			"*MATERIAL_SPECULAR"
#define SHINE				"*MATERIAL_SHINE"
#define TEXTURE				"*BITMAP"
#define MAP_CLASS			"*MAP_CLASS"
#define UTILE				"*UVW_U_TILING"
#define VTILE				"*UVW_V_TILING"
#define UOFFSET				"*UVW_U_OFFSET"
#define VOFFSET				"*UVW_V_OFFSET"

// vectors of dimension 1,2,3
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];

// a triangle in the model
typedef struct {
	int vertIndex[3];		// indicies for the verts that make up this triangle
	int coordIndex[3];		// indicies for the tex coords to texture this face
	vec3_t vertNormals[3];	// vertex normals
	vec3_t normal;			// face normal
	int material;			// index for the (sub)material of this face
} triangle_t;


// subtexture information for the model
typedef struct {
	char* texName;		// texture name
	int texmap;			// is there a texture
	int env;			// envirement mapping
	vec_t uTile;		// u tiling of texture
	vec_t vTile;		// v tiling of texture
	vec_t uOffset;		// u offset of texture
	vec_t vOffset;		// v offset of texture
	vec3_t ambient;
	vec3_t diffuse;
	vec3_t specular;
	vec_t shine;
} subtex_t;

// texture information for the model
typedef struct {
	char* texName;		// texture name
	int texmap;			// is there a texture
	int env;			// envirement mapping
	int numSubMat;		// number of submaterials
	vec_t uTile;		// u tiling of texture
	vec_t vTile;		// v tiling of texture
	vec_t uOffset;		// u offset of texture
	vec_t vOffset;		// v offset of texture
	vec3_t ambient;
	vec3_t diffuse;
	vec3_t specular;
	vec_t shine;
	subtex_t *submaterials;	// submaterials of the material
} texture_t;


class model {
private:
	int numVertex;				// number of verts in the model
	int numFaces;				// number of faces in the model
	int numTexFaces;			// number of textured faces in model
	int numTexVertex;			// number of texture coords
	int numMaterials;			// number of materials
	int material;				// material ID
	vec3_t *verts;				// model verts
	vec2_t *texVerts;			// texture coords
	triangle_t *faces;			// faces of model
	texture_t *texture;			// texture/material information

	vec_t sphere_rad;			// radius of the bounding sphere

	vec_t minx;					// values for the bounding box
	vec_t maxx;					// ...
	vec_t miny;					// ...
	vec_t maxy;					// ...
	vec_t minz;					// ...
	vec_t maxz;					// ...

	
	char *outputfile;			// filename for output
	FILE *s;					// inputfilestream
	
	void getCounts();			// reads the numbers of verts/faces/... from the ase file
	void getVertex();			// reads a vertex from the ase file
	void getTVertex();			// reads a texture coord from the ase file.  
	void getNVertex(int facei, int i);			// reads a vertex normal from the ase file
	void getTextureName(int mati, int subi, int mat);	// reads in a texture name from the ase file
	void getAmbient(int mati, int subi, int mat);		// get ambient values of material
	void getDiffuse(int mati, int subi, int mat);		// get diffuse values of material
	void getSpecular(int mati, int subi, int mat);		// get specular values of material
	void getMaterials();		// gets material and texture informations from the ase file
	void calculateTexCoords();	// changes the texture coords depending on the material
	void getFaceNormal();		// reads a face normal from the ase file
	void getFace();				// reads a face from the ase file
	void getTFace();			// reads the indicies for a texture mapped face
	vec_t getFloatVal();		// reads a float value from the file, expects data to be like this SOMETHING 4.2323
	void getData();				// gets all information needed from the ase file
	
	void allocate();			// allocate memory
	void freemem();				// releases all memory
	void model::Init();			// initializes all values
	
	void write_int(FILE* f,int value);		// write a binary int
	void write_sizet(FILE* f,size_t value);	// write a binary unsigned int
	void write_float(FILE* f,float value);	// write a binary float
	void write_char(FILE* f,char value);	// write a binary char

	void calculate_boundings();					// calculates bounding sphere and box

	float distance(float x, float y, float z);	// calculates length of vector (x,y,z);

public:
	model(char* filename, char* output);
	virtual ~model();
    void write();
	void Info();				// returns values of model
};

#endif
