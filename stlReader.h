#ifndef STLREADER_H
#define STLREADER_H


#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <sys/time.h>

//used to get exictuion time acurate to .01 seconds
class timer{
public:
		double t1 = 0;
		double t2;
	 timeval tim;
	void start(void){
		gettimeofday(&tim, NULL);
		 t1=tim.tv_sec+(tim.tv_usec/1000000.0);

	}
	double long end(void){
		gettimeofday(&tim, NULL);
        t2=tim.tv_sec+(tim.tv_usec/1000000.0);
        printf("\t sec elap:%.6lf \t", t2-t1);
        return (t2 - t1) ;
	}

};



//the statistics of an stl object
class StlStats{
public:
	uint32_t numbsurface; //numbere of surfaces
	float extrema[6]; // xmin xmax  y  z
	float avenorm[3];
	void print(void);
};

//each surface
typedef float sur[12];//   //norm then 3 vertexes

//array of features to be passed from freaure finder to predictor
struct Objstats{
	// surface area
	// obj volume
	// #layers
	// support volume
	// sharp corrners
	// # islands 
	double data[7];

};

//the transform that the reader appiles to the object befor 
// passing it out
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
//stores a surface and its max and min z values 
struct surextrema{
  
  sur body;
  float max;
  float min;
};



class FeatureFinder{
	// slice object to get numb corners and numb islands
	void Slice( surextrema surfaces[], int numbsurface, Objstats * stats, float top );
public:
	float layerThickness = .2;
	//get all features 
	int getFeatures(Objstats * stats, StlReader * reader);
	//get only features that are rotation dependent
	int rotFeatures(Objstats * stats, StlReader * reader);
};


//3D rotation matrix 
typedef float rot[3];

//general perpose matrix multiper A * B = C
// A n*m  B m*p  C n *P
void matrixmult (float * A , float * B, float * C, int n, int p, int m );

class Optimizer{
public:
	// find the best rotation for an object store in bestRot
	void bestRotate(StlReader * reader, rot * bestRot);
	
	float alpha ; //size of steps
	int numbSteps; // number of steps per try
	int numbTries ; //number of tries 
	float oldTime;
	float bestTime;
	//where to find current predicotr model 
	std::string filename = "./currentModel";
	Optimizer(void){
		this-> alpha = .30;
	this-> numbSteps = 200;
	this->numbTries = 40 ;
	this-> filename = "./currentModel";
	}

};


#endif
