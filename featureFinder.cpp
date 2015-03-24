#include <limits>
#include "stlReader.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
 
#define angle 30
#define next(a) (((a+1)%3)+1)

//need to add tetrahedron top for the volume calculation



void cross(float * A, float * B, float * C){
	//assume 3x3
	C[0] = A[ 1]* B[ 2] - A[ 2]* B[ 1];
 	C[1] = A[ 2]* B[ 0] - A[ 0]* B[ 2];
  C[2] = A[ 0]* B[ 1] - A[ 1]* B[ 0];
}

//build un ordered map of 2 veretieces + int 012
// for each surface insert 3 pairs 1 for each edge 
//if colide take corrner 

/*double Area(sur cur, int dim){
float A[3], B[3], crossProd[3], partial;
  for(int i =0 ; i < 3 ; i++){
        A[i] = cur[2*3 + i] - cur[1*3 + i];
        B[i] = cur[3*3 + i] - cur[1*3 + i];
      }
      if(dim == 2) {
        A[3] = 0.0;
        B[3] =0.0;
      }
      //get cross product 
      cross(A, B , crossProd); 
      //square componants add each take sqrt 
      for(int i = 0 ; i < 3; i++)
        crossProd[i] = abs(crossProd[i]);
      partial = 0;
      for(int i = 0 ; i < 3; i++)
        partial += crossProd[i];
      //devide by 2 
    std::cout << partial/2 << " +" ;
        return partial/2;
}*/


int FeatureFinder ::getFeatures(Objstats * stats, StlReader * reader){
	Objstats mystats;
	float crossProd[3];
	float A[3];
  	float B[3];
	double VolTotal = 0;
	double areaTotal = 0.0;
	double partial = 0;
  double suportVol = 0.0;
  double suportthreshold = -1 * cos(angle);
	sur cur;
  reader->restReading();
 // printf("numb surfaces:%d\n",(reader)->stats.numbsurface );
	for(uint32_t surfnumb = 0; surfnumb < (reader)->stats.numbsurface; surfnumb++){
		(reader)->getSurface(cur);

	
  		cross(&cur[2*3], &cur[3*3], crossProd);
	
  		partial = crossProd[0] * cur[1*3 + 0] + crossProd[1] * cur[1*3 + 1] + crossProd[2] * cur[1*3 + 2];

  		partial = abs(partial) / 6.0;

  		float normsum = 0;

      // want dot of nromal and vecotr from origin to center
      float centroid[3]; 
      for(int dim =0; dim< 3; dim++){
        for(int vert = 0; vert < 3 ; vert++ )
          centroid[dim] += cur[(vert +1)*3 + dim];
        centroid[dim] /= 6.0;
        normsum += cur[dim] *  centroid[dim];
      }

  		//normal points awway from object
  		
  		//see if dot product of surface normal is positive
  		if(normsum < 0) {
  			//negtive faceing toward subtract
  			
  			VolTotal -= partial;
  		}else VolTotal += partial;

      //suport volume
      //if angle betwen the xy and surface < angle 
      double dot = 0.0;
      for( int i =0; i < 3; i++)
          dot += pow(cur[i], 2);
      dot = sqrt(dot);
      dot = cur[2] / dot;
      if(dot < suportthreshold){
        //needs suport 
        //upper tetrahedron
        
        //body trangulat solid 
        double area = 0.0;
        for(int j = 0; j < 3; j++){
          partial = cur[next(j) *3 + 1 ] - cur[next(j+1) * 3 +1];
          area += partial * cur[(j +1) * 3];
        }
        area = abs(area)/2;
        double minZ = cur[1*3 +2 ];
        for(int i =1 ; i< 4; i++)
          if(cur[i*3 +2] < minZ) minZ = cur[i*3 +2];
     //   if(abs(area * minZ) > 0 ) std::cout << " +"<< area * minZ;
        suportVol += area * minZ;
      }

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
  			crossProd[i] = abs(crossProd[i]);
  		partial = 0;
  		for(int i = 0 ; i < 3; i++)
  			partial += crossProd[i];
  		//devide by 2 
  			areaTotal += partial/2;

        //add surface to the two lists 

	}

  //for each slice add intersecting lines //find corrners 

  //for each floor that intersects adds the triangle or trapazod to suport set

  //put object into 2d tree

  //use tree to prune support 
    //where lines intersect 

  //get support volume



  //at very end get raft volume


  mystats.data[0] = areaTotal;
//  std::cout << "  area :"<< areaTotal ;
  mystats.data[1] = VolTotal;
  //std::cout << "  volume:"<< VolTotal  << std::endl;

	mystats.data[2] = reader->stats.extrema[5]/ this->layerThickness;
	//std::cout << " layers:"<< mystats.layers ;

mystats.data[3] = suportVol;
std::cout << " support vol:"<< suportVol ;
  // mystats.data[4] = 1;
	* stats = mystats;
	return 0;
}
 