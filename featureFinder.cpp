#include <limits>
#include "stlReader.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
 //floors with lower angles must be supported 
#define suportAngle (40* 3.141592 / 180)
#define next(a) (((a+1)%3)+1)
#define leaffill 4 //number of segment pointers that can be stored in a leaf node
#define layerWidth .1
//corners with lower angles must be slowed for 
#define cornerAngle (30* 3.141592 / 180)
#define invalidLoc -1000

//need to add tetrahedron top for the volume calculation

//surface with its max and min z 
struct surextrema{
  
  sur body;
  float max;
  float min;
};




//claculate cross product
void cross(float * A, float * B, float * C){
	//assume 3x3
	C[0] = A[ 1]* B[ 2] - A[ 2]* B[ 1];
 	C[1] = A[ 2]* B[ 0] - A[ 0]* B[ 2];
  C[2] = A[ 0]* B[ 1] - A[ 1]* B[ 0];
}

typedef float Seg[4];  // line segments 

//k2d tree node
struct node{
  node * left;
  node * right;
  int body[leaffill];
};
struct pairhash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};
typedef std::unordered_map< std::pair<float,float>, int, pairhash > edgeHash;


class k2dTree{
  node head;
public:
 k2dTree(int numbsegments, Seg segs[]);
 //tree stores indexes into the Segs array if that changes the tree will be useless 
 int * search(Seg cur);
 //will return the list of indexes for segments fall in the box defined by these endpoints 
 void clear(void);
};

float angle(float * v1, float * v2, int size){
  //returns the arccos of the angle of the two vectors 
  float n1=0, n2=0;
  for(int i = 0; i < size; i++){
    n1 += pow(v1[i], 2);
    n2 += pow(v2[i], 2);
  }
  n1 = sqrt(n1);
  n2 = sqrt(n2);
  n1 *=n2;
  float top = 0;
  for(int i = 0 ; i < size; i++)
    top += v1[i] * v2[i];
  return top / n1;
}

int insert(float * cur, float * corners, edgeHash slice, int numbCorner, int * sharpegde ){
  //return the number of added corrners
  //cur is lenght 4 first point is loc to be added second is other side
  std::pair<float, float> temp = std::make_pair(cur[0], cur[1]) ; 
  if(0 == slice.count(temp) ){
  //first time this point inserted 
     
    slice.emplace(temp,numbCorner);
    for(int i = 0 ; i < 2; i++){  
      corners[i + numbCorner *4] = cur[i + 2];
      corners[2+ i + numbCorner *4]= invalidLoc;
    }
   return 1; //new corner added 
  }else{
    int curCorner = slice[temp];
    if(corners[curCorner*4 +2 ] != invalidLoc ){
        //second point inserted 
        //check sharp angle 
      corners[curCorner*4 +2 ] = cur[0];
      corners[curCorner*4 +3 ] = cur[1];
      float v1[2];
      float v2[2];
      for(int i = 0; i < 2; i++){
        v1[i]= cur[i] - cur[i+2];
        v2[i]= cur[i] - corners[curCorner*4 + i];
      }
      if( abs(cos(angle(v1, v2, 2) )) > cornerAngle) * sharpegde = *sharpegde +1;

    }else //three lines at 1 corrner
      printf("ERROR \n");
  }
    return 0;
}


//build un ordered map of 2 veretieces + int 012
// for each surface insert 3 pairs 1 for each edge 
//if colide take corner 

//slice the object and get number of islands and number of corners 
void Slice( surextrema surfaces[], int numbsurface, Objstats * stats, float top ){
  printf("entering slice \n");
  float numbcorners = 0;
  int sharpCorners = 0;
  float numbIslands = 0;
 
//  point * endpoints; 
 // float cornerThreshold = cos(cornerAngle); 
 
//stores the other endpoints of the 2 line segments that interset this point



  for(float level = top; level > -1.0; level = level -layerWidth){//per slice
    float corners [numbsurface *4];
    int numbcorners = 0;
    edgeHash slice;
  //for each slice add intersecting lines add them to hashtable in 2 locations with endpoint and first/ second pars
   //find corners when inserting
   
    for(int surfnumb = 0; surfnumb < numbsurface; surfnumb++){
    //it intersects 
      if(surfaces[surfnumb].max >= level && surfaces[surfnumb].min <= level){
        float cur[4];
      //Seg cur;
   //   Seg edges[2];
        bool first = 0;
      //find endpoints of segment 
        for(int i = 0; i < 3; i++){
        //find two intersecting edges
          sur tri;
          for(int i = 0; i < 12; i++)
            tri[i] = surfaces[surfnumb].body[i];
          int B = next(i)*3;
          if((tri[(i+1)*3 + 2] <=  level &&   tri[B + 2] >= level) ||
          (tri[(i+1)*3 + 2] >=  level &&   tri[B + 2] <= level)){
          //the endpoints must be on oposite sides of level
            float deltaZ = (tri[(i+1)*3 + 2] -level) /        (tri[(i+1)*3 + 2] -  tri[B + 2]);
            cur[first * 2 + 0] = (tri[(i+1)*3 + 0] -  ((tri[(i+1)*3 + 0] -  tri[B + 0])) *deltaZ) ;
            cur[first * 2 + 1] = (tri[(i+1)*3 + 1] -  ((tri[(i+1)*3 + 0] -  tri[B + 1])) *deltaZ) ;
          }
        }
        //now insert this edge at both endpoints 
        numbcorners += insert(cur, corners, slice, numbcorners, &sharpCorners);
        float temp[4];
        for(int i = 0; i < 2; i++){
          temp[i] = cur[i + 2];
          temp[i +2] = cur[i ];
        }
        numbcorners += insert(temp, corners, slice, numbcorners, &sharpCorners);
     
      }//end proccessing of intersecting surfacee

    }//end processing of 1 surface
 
 //circle to get island number 
  //walk throught segments that make cycles 
  int totalVisits = 0;
  int searchStart = 0;
  bool visited[numbcorners];
  std::pair<float, float> old ;
  std::pair<float, float> current;
  std::pair<float, float> next;
  int pos = 0;
  int left; 
  int right;


  
  while(totalVisits < numbcorners ){

    
    pos = searchStart;
    

    //find pair that corresponds to pointith entry in hash table 
    //  set it in cur
    //neghbors of point 
    old =  std::make_pair(corners[pos * 4 +2] , corners[pos * 4 + 3]) ;
    next =  std::make_pair(corners[pos * 4] , corners[pos * 4 + 1]) ;  

    left = slice [ old];
    right = slice [next];
    for(int i = 0; i < 2; i++){
      for(int j = 0; j < 2;  j++ ){
        if(corners[left * 4 + i*2] == corners[right * 4 + j*2] && 
            corners[left * 4 + i*2 +1] == corners[right * 4 + j*2 +1] )
          current = std::make_pair(corners[left * 4 + i*2]  , corners[left * 4 + i*2 +1 ] );
      }
    }

    old = std::make_pair(invalidLoc, invalidLoc) ; 
    //found the start of an island walk around it
    while(false == visited[pos]){
      visited[pos] = true;
      //pick the side which is not old 
      if(corners[pos * 4] != old.first && corners[pos * 4 +1] != old.second )
         next = std::make_pair(corners[pos * 4] , corners[pos * 4 +1]);
      if(corners[pos * 4 +2] != old.first && corners[pos * 4 +3] != old.second )
         next = std::make_pair(corners[pos * 4 +2] , corners[pos * 4 +3]);
       old = current;
       current = next;
       pos = slice[current];
      
    }
    numbIslands ++; 
    while(true == visited[searchStart] && searchStart < numbcorners) {
      searchStart ++;   
      totalVisits ++;   
    }
    
  }


 

  //for each floor that intersects adds the triangle or trapazod to suport set

  //put object into 2d tree

  //use tree to prune support 
    //where lines intersect 

  //get support volume

  
  }//end processing of one slice
  std::cout << " ended slice" << "\t";
std::cout << " corrners:"<< sharpCorners << "\t";

  stats->data[4] = sharpCorners;
  stats->data[5] = numbIslands;

}

int FeatureFinder ::getFeatures(Objstats * stats, StlReader * reader){
	Objstats mystats;
 
	float crossProd[3];
	float A[3];
  float B[3];
	double VolTotal = 0;
	double areaTotal = 0.0;
	double partial = 0;
  double suportVol = 0.0;
  double suportthreshold =  cos(suportAngle);
 // list <extremaSur> surMinZ;
 // list <extremaSur> surMaxZ;
  surextrema surfaces[(reader)->stats.numbsurface];
	sur cur;
  reader->restReading();
 // printf("numb surfaces:%d\n",(reader)->stats.numbsurface );
	for(uint32_t surfnumb = 0; surfnumb < (reader)->stats.numbsurface; surfnumb++){
		(reader)->getSurface(cur);
      double minZ = cur[1*3 +2 ];
      double maxZ = cur[1*3 +2 ];
      for(int i =1 ; i< 4; i++){
          if(cur[i*3 +2] < minZ) minZ = cur[i*3 +2];
          if(cur[i*3 +2] > maxZ) maxZ = cur[i*3 +2];
      }
	//volume from this surface
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

  		//normal points away from object
  		
  		//see if dot product of surface normal is positive
  		if(normsum < 0) {
  			//negtive faceing toward subtract
  			
  			VolTotal -= partial;
  		}else VolTotal += partial;



      //suport volume
      //if suportAngle betwen the xy and surface < suportAngle 
      double dot = 0.0;
      for( int i =0; i < 3; i++)
          dot += pow(cur[i], 2);
      dot = sqrt(dot);
      dot = cur[2] / dot;
      if(abs(dot) <= suportthreshold && cur[2] < 0){
      //if(cur[2] < 0){
        //needs suport 
        //upper tetrahedron
        
        //body trangulat solid 
        double area = 0.0;
        for(int j = 0; j < 3; j++){
          partial = cur[next(j) *3 + 1 ] - cur[next(j+1) * 3 +1];
          area += partial * cur[(j +1) * 3];
        }
        area = abs(area)/2;
       
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

     //add surface to list for building slice
        for(int i = 0; i < 12; i++) 
          surfaces[surfnumb].body[i] = cur[i];
          surfaces[surfnumb].min = minZ;
          surfaces[surfnumb].max = maxZ;
       


	}
  //slice for more complex features 
  Slice(surfaces, (reader)->stats.numbsurface, stats,  (reader)->stats.extrema[5]);
  //at very end get raft volume


  mystats.data[0] = areaTotal;
//  std::cout << "  area :"<< areaTotal ;
  mystats.data[1] = VolTotal;
  //std::cout << "  volume:"<< VolTotal  << std::endl;

	mystats.data[2] = reader->stats.extrema[5]/ this->layerThickness;
	//std::cout << " layers:"<< mystats.layers ;

mystats.data[3] = suportVol;
std::cout << " support vol:"<< suportVol << "\t";
std::cout << " corrners:"<< stats->data[4] << "\t";
std::cout << " islands:"<< stats->data[5] << "\t";
  // mystats.data[4] = 1;
	* stats = mystats;
	return 0;
}
 