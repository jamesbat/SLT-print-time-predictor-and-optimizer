#ifndef STLREADER_H
#define STLREADER_H
//corrners 
//raft area 
//more some functions to utils 
//scaling check
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
//each surface
typedef float sur[12];//   //norm then 3 vertexes


struct Objstats{
	// surface area
	// obj volume
	// #layers
	// support volume
	// sharp corrners
	// # islands 
	double data[6];

	/*double surfaceArea; //mm^2
	double volume; // mm^3
	//double raftArea;// mm^2
	double layers; // # layers 
	double constant = 1; 
	double */
};

struct StlTransform{
	//objects are rotated first 
	//then scaled then translated
	float R[9];
	//x- y   x - z   	y - z
	float scale[3];
	float trans[3];
	bool active;
	
	bool rotate; 
	
	
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
		for(int i = 0 ; i < 3; i++)
			this->transform.scale[i] = 1.0;
	}

	void hello(void);

	int restReading(void);
	//restart reading at first surface
	int getSurface(sur out);
	//read out next surface 

	int openFile (std::string newname, bool overwrite );
	//open file check if is binary or text 
	int saveObject(std::string newname );
	// save object in current orrientation and possiton 

	int getStats (void);
	//read the extrema of the object in current config
	
	int close(void);
	void clear(void);

	int setRotation (float spin[3]);
	//input   roll roundx  pitch roundy     inyaw round z 
	//a rtated object should be shrinked and set down befor being printed 

	int shrinkToFit(int dim[3], bool curstats );
	//will shrink to fit in a x y z box 
	
	int setDown(bool curstats);
	//moves object so its min z is 0 and it is centered in x and y 
	//curstats mark true if object has not been rotated or moved since last get stats 




	
};  
struct surextrema{
  
  sur body;
  float max;
  float min;
};



class FeatureFinder{
	void Slice( surextrema surfaces[], int numbsurface, Objstats * stats, float top );
public:
	float layerThickness = .2;
	int getFeatures(Objstats * stats, StlReader * reader);
};



typedef float rot[3];


void matrixmult (float * A , float * B, float * C, int n, int p, int m );

class Optimizer{
public:
	void ping(void){return;}
	void bestRotate(StlReader * reader, rot * bestRot);
	float alpha = .01;
	int numbSteps = 100;
	int numbTries =5 ;
	float oldTime;
	float bestTime;

	std::string filename = "currentModel";
	Optimizer(void){
		this-> alpha = .05;
	this-> numbSteps = 100;
	this->numbTries =5 ;
	this-> filename = "currentModel";
	}

};


#endif
