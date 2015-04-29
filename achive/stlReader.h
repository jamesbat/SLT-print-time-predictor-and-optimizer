#ifndef STLREADER_H
#define STLREADER_H

//work on fixing volume
//todo rotation  reset readerer
//save output change stats to use get surface 
//translation and scaling 
//check surface normal

#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
class StlStats{
public:
	uint32_t numbsurface;
	float extrema[6]; // xmin xmax  y  z
	float avenorm[3];
	void print(void);
};

typedef float sur[4][3];

//struct sur{
//	float verts[4][3];
//};

struct Objstats{
	float surfaceArea;
	float volume;
	float dim[3];
};

struct StlTransform{
	float trans[3];
	float scale[3];
	bool active;
	
	bool rotate; 
	float crossing[6];
	//x- y   x - z   	y - z
	
};



class StlReader{
	std::ifstream source;
public: 
	bool open;
	bool writeable;
	bool isbinary;
	StlStats stats;
	StlTransform transform;

	
	std::string filename;

	StlReader(void){
		this->transform.active = false;
		this->open = false;
	}

	void hello(void);
	
	int getstats (void);
	int openFile (std::string newname, bool overwrite );
	//open file check if is binary or text 

	int setRotation (float spin[3]);
	//input inyaw round z pitch roundy roll roundx   

	int store (std::string newname);
	//store at this file name

	int getsurface(sur * out);
	//returns a surfacce will all vertexes at 0 if fail 
	Objstats getfeatures(void);
};  



#endif