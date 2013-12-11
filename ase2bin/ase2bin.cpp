// Reads an 3DS ase file and converts it to a binary format only
// containing the relevant informations.
// Also calculates the bounding sphere and the bounding box for
// collision detection and writes it to the binary file.

#include "model.h"
#include <stdio.h>

int main(int argc, char **argv) {
	if(argc>3||argc<2) {
		printf("Usage: ase2bin <inputfile> [<outputfile>]");
		return 1;
	}
	char* filename=argv[1];
	char* output=NULL;
	if(argc==3) {
		output=argv[2];
	}
	FILE *asefile;
	if (!(asefile = fopen (filename, "r")))
	{
		printf ("# %s not found!\n", filename);
		return 1;
	}
	fclose (asefile);
	model *obj=new model(filename,output);
	obj->write();
	return 0;
}
