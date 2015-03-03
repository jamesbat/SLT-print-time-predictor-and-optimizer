#include <limits>
#include "stlReader.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
 
void cross(float * A, float * B, float * C){
	//assume 3x3
	C[0] = A[ 1]* B[ 2]-A[ 2]* B[ 1];
 	C[1] = A[ 2]* B[ 0]-A[ 0]* B[ 2];
  	C[2] = A[ 0]* B[ 1]-A[ 1]* B[ 0];
}

//build un ordered map of 2 veretieces + int 012
// for each surface insert 3 pairs 1 for each edge 
//if colide take corrner 



int FeatureFinder ::getFeatures(Objstats * stats, StlReader * reader){
	Objstats mystats;
	float crossProd[3];
	float A[3];
  	float B[3];
	float VolTotal = 0;
	float areaTotal = 0.0;
	float partial = 0;
	sur cur;
	for(uint32_t i = 0; i < (reader)->stats.numbsurface; i++){
		(reader)->getSurface(cur);
		
	
  		cross(&cur[2*3], &cur[3*3], crossProd);
	
  		partial = crossProd[0] * cur[1*3 + 0] + crossProd[1] * cur[1*3 + 1] + crossProd[2] * cur[1*3 + 2];

  		partial = abs(partial) / 6.0;

  		float normsum = 0;
  		//normal points awway from object
  		for( int j = 0; j < 3; j++)
  			normsum += cur[0*3 + i];
  		//see if dot product of surface normal is positive
  		if(normsum < 0) {
  			//negtive faceing toward subtract
  			
  			VolTotal -= partial;
  		}else VolTotal += partial;

  //surface area 
  		//get two border vectors 
  		for(int i =0 ; i < 3 ; i++){
  			A[i] = cur[2*3 + i] - cur[1*3 + i];
  			B[i] = cur[3*3 + i] - cur[1*3 + i];
  		}
  		//get cross product 
  		cross(A, B , crossProd); 
  		//square componants add each take sqrt 
  		for(int i = 0 ; i < 3; i++)
  			crossProd[i] = abs(crossProd[i] * crossProd[i]);
  		partial = 0;
  		for(int i = 0 ; i < 3; i++)
  			partial += sqrt(crossProd[i]);
  		//devide by 2 
  			areaTotal += partial/2;


	}
	mystats.layers = reader->stats.extrema[5]/ this->layerThickness;
	std::cout << "total layers were:"<< mystats.layers  << std::endl;
	mystats.surfaceArea = areaTotal;
	std::cout << "total area  was:"<< areaTotal  << std::endl;
	mystats.volume = VolTotal;
	std::cout << "total volume  was:"<< VolTotal  << std::endl;
	* stats = mystats;
	return 0;
}
