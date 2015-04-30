#include <limits>
#include "stlReader.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
 //floors with lower angles must be supported 
#define suportAngle (45* 3.141592 / 180)
#define next(a) (((a+1)%3)+1)
#define leaffill 4 //number of segment pointers that can be stored in a leaf node

//corners with lower angles must be slowed for 
#define cornerAngle (30* 3.141592 / 180)
#define invalidLoc -1000

//need to add tetrahedron top for the volume calculation
 
//surface with its max and min z 

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

void intersect(float * A, float * B, float level, float * C, float * push){
  //finds intersection point between vector A -> B and Z = level plane 
  //store x and y cordinates in C 
  double deltaZ = ( level - A[2] ) / (B[2] - A[2] );
  //if(deltaZ == 0) printf("zero error\n\n");
  //if(deltaZ > std::numeric_limits<float>::max()) printf("inf error\n\n");
  C[0] = push[0] + A[0] + (B[0] - A[0]) * deltaZ; 
  C[1] = push[1] + A[1] + (B[1] - A[1]) * deltaZ; 

}


int insert(float * cur, float * corners, edgeHash * Slice, int numbCorner, int * sharpegde ){
  edgeHash slice = * Slice;
  //return the number of added corrners
  //cur is lenght 4 first point is loc to be added second is other side
  std::pair<float, float> temp = std::make_pair(cur[0], cur[1]) ; 
   // printf("%f %f ____", temp.first, temp.second);
   // printf("%f %f %f %f _____\n", cur[0], cur[1], cur[2], cur[3] );
  if(0 == slice.count(temp) ){
  //first time this point inserted 
   //  printf("# ");
   
       slice.emplace(temp,numbCorner);
    for(int i = 0 ; i < 2; i++){  
      corners[i + (numbCorner *4)] = cur[i + 2];
      corners[2+ i + (numbCorner *4)]= invalidLoc;
    }
  // if(0 == slice.count(temp)||0 == slice.at(temp)  )printf("insert failed\n");
  
    * Slice = slice;
   return 1; //new corner added 
  }else{
  
    int curCorner = slice.at(temp);
    if(corners[curCorner*4 +2 ] == invalidLoc ){
      //   printf("@ ");
        //second point inserted 
        //check sharp angle 
      corners[curCorner*4 +2 ] = cur[3];
      corners[curCorner*4 +3 ] = cur[4];
      float v1[2];
      float v2[2];
      for(int i = 0; i < 2; i++){
        v1[i]= cur[i] - cur[i+2];
        v2[i]= cur[i] - corners[curCorner*4 + i];
      }
      if( fabs(angle(v1, v2, 2) ) > fabs(cos(cornerAngle)) ) * sharpegde = *sharpegde +1;
     // printf("_  %f_%f ",angle(v1, v2, 2) , cos(cornerAngle)  );

    }else{ //three lines at 1 corrner
      printf(" triple intersect ERROR %d %d  %g  %g\n", curCorner, (int) slice.size(), cur[0], cur[1] );
    }
  }
   * Slice = slice;
    return 0;
}



void circleIslands(int numbcorners, edgeHash slice, float * corners, float * numbIslands){

  int searchStart = 0;
  bool visited[numbcorners];
  std::pair<float, float> old ;
  std::pair<float, float> current;
  std::pair<float, float> next;
  int pos = 0;
  int left; 
  int right;

 // std::cout << " slice corners:"<< numbcorners << "\t" << *numbIslands<< "\t";
  while(searchStart < numbcorners ){
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
    *numbIslands = * numbIslands +1; 
    while(true == visited[searchStart] && searchStart < numbcorners) {
      searchStart ++;  
    }
  }
 
}




//slice the object and get number of islands and number of corners 
void FeatureFinder ::Slice( surextrema surfaces[], int numbsurface, Objstats * stats, float top ){
    
  printf("entering slice %f layers  %d surfaces\n", top / this->layerThickness, numbsurface);
 // float numbcorners = 0;
  int sharpCorners = 0;
  float numbIslands = 0;
  float * corners = (float *) malloc(sizeof(float)*numbsurface *4 *2);

  for(float level = top; level > -0.1; level = level -this->layerThickness){//per slice
  
    for( int i = 0 ; i < numbsurface *4*2; i++)
      corners[i] = invalidLoc;
 
    int numbcorners = 0;
    edgeHash slice;
    edgeHash * Slice = &slice;
  //for each slice add intersecting lines add them to hashtable in 2 locations with endpoint and first/ second pars
   //find corners when inserting

    for(int surfnumb = 0; surfnumb < numbsurface; surfnumb++){
    //it intersects 
      if(surfaces[surfnumb].max > level && surfaces[surfnumb].min < level){
        float cur[4];
 
        //find endpoints of segment 
     
  
        sur tri;
        for(int j = 0; j < 12; j++){
           tri[j] = surfaces[surfnumb].body[j];
     //     printf("%g  ", tri[j]);
        }
      //  printf("\n");
        int above[2];
        int below [2];
        int toptotal = 0;
        for(int j = 1; j < 4; j++){
          if(tri[j*3 + 2] >= level){
           above[toptotal] = j*3;
           toptotal ++;
          }
          else below[j - 1 -  toptotal] = j*3;
        }
   //   printf("%d  %d  %d  %d  %d  \n", toptotal, above[0], above[1], below[0], below[1]);
            intersect ( &tri[above[0]], &tri[below[0]], level, cur, tri);
          if(toptotal > 1) intersect ( &tri[above[1]], &tri[below[0]], level, &cur[2], tri);
          else intersect ( &tri[above[0]], &tri[below[1]], level, &cur[2], tri);
          
        
        if((cur[0] != cur[2] ) && cur[1] != cur[3]){
        //now insert this edge at both endpoints 
          numbcorners += insert(cur, corners, Slice, numbcorners, &sharpCorners);
          float temp[4];
          for(int i = 0; i < 2; i++){
            temp[i] = cur[i + 2];
            temp[i +2] = cur[i ];
          }
          numbcorners += insert(temp, corners, Slice, numbcorners, &sharpCorners);
    //    std::pair<float, float> blarg = std::make_pair(cur[0], cur[1]) ; 
      //  if(0 == (Slice)->at(blarg)  && 0 != (Slice)->count(blarg)) printf("outside error");
      //  printf("%d\n",(int) Slice->size());
        }
      }//end proccessing of intersecting surfacee
    }//end processing of 1 surface
   
  //circle to get island number 
  //walk throught segments that make cycles 
//circle the cegments to find the number of islands  
    circleIslands(numbcorners, slice, corners,  &numbIslands);

   // std::cout << numbIslands << std::endl;
 

    slice.clear();
    
  }//end processing of one slice
  free(corners);
 // std::cout << " ended slicing" << "\t";
  //std::cout << " sharp corrners:"<< sharpCorners << "\n";

  stats->data[4] = sharpCorners;
  stats->data[5] = numbIslands;

}






int FeatureFinder ::getFeatures(Objstats * stats, StlReader * reader){
	Objstats mystats;

	float crossProd[3];
	float A[3];
  float B[3];
  float C[3];
	double VolTotal = 0.0;
	double areaTotal = 0.0;
	double partial = 0.0;
  double suportVol = 0.0;
  double suportthreshold =  cos(suportAngle);
  float tetCorrner[3];


 StlStats filestats = reader->stats;
  // list <extremaSur> surMinZ;
 // list <extremaSur> surMaxZ;

 // surextrema * surfaces = ( surextrema *) malloc (filestats.numbsurface * sizeof(surextrema) );
  
	sur cur;
  reader->restReading();
 

	for(uint32_t surfnumb = 0; surfnumb < filestats.numbsurface; surfnumb++){
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
        centroid[dim] /= 4.0;
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
        //abs( (a -D) * ((B -D) x (c - D))) /6
        //work in from  right to left
        //build 4 point direfctly below the highest point
        for(int i = 0; i < 3; i ++)
          if(cur[(i +1) *3 + 2 ] == maxZ){
            for(int j = 0; j < 2; j++)
              tetCorrner[j] = cur[(i +1) *3 +j];
            break;
          }
        //dorp down 
        tetCorrner[2] = minZ;

        for(int i = 0; i < 3; i ++){
          //((B -D) x (c - D))
          A[i] = cur[2 *3 + i] - tetCorrner[i];
          B[i] = cur[3 *3 + i] - tetCorrner[i];
        }

        cross(A, B, C);
        partial = 0.0;
        for(int i = 0; i < 3; i ++){
          //(a -D) * ((B -D) x (c - D))
          A[i] = cur[1 *3 + i] - tetCorrner[i];
          partial += A[i] * C[i];
        }

       
        suportVol += abs(partial)/6;

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
  			crossProd[i] = pow(crossProd[i], 2);
  		partial = 0;
  		for(int i = 0 ; i < 3; i++)
  			partial += crossProd[i];
  		//devide by 2 
  			areaTotal += sqrt(partial)/2;

     //add surface to list for building slice
     //   for(int i = 0; i < 12; i++) 
      //    surfaces[surfnumb].body[i] = cur[i];
     //     surfaces[surfnumb].min = minZ;
      //    surfaces[surfnumb].max = maxZ;
       
    
	}

  //Slice(surfaces, (reader)->stats.numbsurface, stats,  (reader)->stats.extrema[5]);
 // //at very end get raft volume

 // free(surfaces);

  mystats.data[0] = areaTotal;
//  std::cout << "  area :"<< areaTotal ;
  mystats.data[1] = VolTotal;
  //std::cout << "  volume:"<< VolTotal  << std::endl;

	mystats.data[2] = reader->stats.extrema[5]/ this->layerThickness;
//	std::cout << " layers:"<< mystats.data[2]<< "\t" ;

mystats.data[3] = suportVol;
mystats.data[4] = stats->data[4];
mystats.data[5] = stats->data[5];
if(suportVol < 0 ) printf("ff suport error %g\n", suportVol);
if(VolTotal < 0 ) printf("ff vol error %g\n", VolTotal);
if(areaTotal < 0 ) printf("ff area error %g\n", areaTotal);
//std::cout << " support vol:"<< suportVol << "\t";
//std::cout << " corrners:"<< stats->data[4] << "\t";
//std::cout << " islands:"<< stats->data[5] << "\n";
  // mystats.data[4] = 1;
	* stats = mystats;
	return 0;
}
 


 int FeatureFinder ::rotFeatures(Objstats * stats, StlReader * reader){
  Objstats mystats;
//printf("---");
 
  float A[3];
  float B[3];
  float C[3];
  double VolTotal = 0.0;
  double areaTotal = 0.0;
  double partial = 0.0;
  double suportVol = 0.0;
  double suportthreshold =  cos(suportAngle);
  float tetCorrner[3];
  //double suportthreshold =  cos(suportAngle);



 StlStats filestats = reader->stats;
  // list <extremaSur> surMinZ;
 // list <extremaSur> surMaxZ;

  //surextrema * surfaces = ( surextrema *) malloc (filestats.numbsurface * sizeof(surextrema) );
  
  sur cur;
  reader->restReading();
 

  for(uint32_t surfnumb = 0; surfnumb < filestats.numbsurface; surfnumb++){
    (reader)->getSurface(cur);
      double minZ = cur[1*3 +2 ];
      double maxZ = cur[1*3 +2 ];
      for(int i =1 ; i< 4; i++){
          if(cur[i*3 +2] < minZ) minZ = cur[i*3 +2];
          if(cur[i*3 +2] > maxZ) maxZ = cur[i*3 +2];
      }
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
        //abs( (a -D) * ((B -D) x (c - D))) /6
        //work in from  right to left
        //build 4 point direfctly below the highest point
        for(int i = 0; i < 3; i ++)
          if(cur[(i +1) *3 + 2 ] == maxZ){
            for(int j = 0; j < 2; j++)
              tetCorrner[j] = cur[(i +1) *3 +j];
            break;
          }
        //dorp down 
        tetCorrner[2] = minZ;

        for(int i = 0; i < 3; i ++){
          //((B -D) x (c - D))
          A[i] = cur[2 *3 + i] - tetCorrner[i];
          B[i] = cur[3 *3 + i] - tetCorrner[i];
        }

        cross(A, B, C);
        partial = 0.0;
        for(int i = 0; i < 3; i ++){
          //(a -D) * ((B -D) x (c - D))
          A[i] = cur[1 *3 + i] - tetCorrner[i];
          partial += A[i] * C[i];
        }

       
        suportVol += abs(partial)/6;

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




    
  }

//  Slice(surfaces, (reader)->stats.numbsurface, stats,  (reader)->stats.extrema[5]);
 // //at very end get raft volume

 // free(surfaces);

  mystats.data[0] = 0;
//  std::cout << "  area :"<< areaTotal ;
  mystats.data[1] = 0;
  //std::cout << "  volume:"<< VolTotal  << std::endl;

  mystats.data[2] = reader->stats.extrema[5]/ this->layerThickness;
//  std::cout << " layers:"<< mystats.data[2]<< "\t" ;

mystats.data[3] = suportVol;
mystats.data[4] = 0;
mystats.data[5] = 0;
if(suportVol < 0 ) printf("ff suport error");
if(VolTotal < 0 ) printf("ff vol error");
if(areaTotal < 0 ) printf("ff area error");
//std::cout << " support vol:"<< suportVol << "\t";
//std::cout << " corrners:"<< stats->data[4] << "\t";
//std::cout << " islands:"<< stats->data[5] << "\n";
  // mystats.data[4] = 1;
  * stats = mystats;
  return 0;
}
 