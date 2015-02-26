#ifndef STLREADER_H
#define STLREADER_H
//test rotation and volume 
//work on fixing volume
//todo  reset readerer
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

typedef float sur[12]; //norm then 3 vertexes



//struct sur{
//	float verts[4][3];
//};

struct Objstats{
//	float surfaceArea;
	float volume;
//	float dim[3];
};

struct StlTransform{
	float trans[3];
	float scale[3];
	bool active;
	
	bool rotate; 
	float R[9];
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
		this->transform.rotate = false;
		this->open = false;
	}

	void hello(void);
	
	int getStats (void);
	int openFile (std::string newname, bool overwrite );
	//open file check if is binary or text 

	int setRotation (float spin[3]);
	//input   roll roundx  pitch roundy     inyaw round z 

	int store (std::string newname);
	//store at this file name

	int getSurface(sur out);
	
	//returns a surfacce will all vertexes at 0 if fail 
	int getFeatures(Objstats *stats);
	int restReading(void);
};  



#endif