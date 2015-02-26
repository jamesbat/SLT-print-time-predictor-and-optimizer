#include <limits>
#include "stlReader.h"
#include <cstring>
#include <iostream>
#include <cmath>

void  StlReader ::  hello(void){
		std::cout << "Hello i'm a reader World!" << std::endl;
	} 

int printSur(sur in){
	for(int i = 0; i < 4; i++){
		for(int j = 0 ; j < 3; j++ )
			std::cout << in[i*3 + j] <<" -- ";
	
		std::cout <<std::endl;
	}
	std::cout <<std::endl;
	return 0;
}
void matrixmult (float * A , float * B, float * C, int n, int p, int m ){
	//A nxp   B pxm   C nxm   compute A*B store in c 
	int indexc, indexa, indexb;
 	float cvalue;
 	for(int col=0;col < m; col++){
  
    for(int row=0;row < n; row++) {

      indexb = col;
      indexc = row * m + col;
      cvalue = 0.;
      for (indexa = row*p; indexa < (row*p + p); indexa++, indexb+= m) 
			cvalue += A[indexa]*B[indexb];
      C[indexc] = cvalue; 

    }
  }
}

int StlReader :: getSurface(sur out){
	sur first;
	char waste[2];
	//float in[4][3];
	if(!this->open){
		std::cout << "cant read from closed reader" << std::endl;
		return -1;
	}
	//for(int vert= 0; vert< 4; vert++)
	//			source.read((char *) out, sizeof(float) *12);
		//for(int vert= 0; vert< 4; vert++)
		//	source.read((char *) &first[vert * 3], sizeof(float)*3 );
		//source.read((char *)  first, sizeof(float) *12);
	//	source.read(waste, 2);
//	for( int i = 0 ; i < 4; i ++)
//		for(int j = 0; j < 3; j++)
//				out[4*i +j] = in [j][i];


	if(this->transform.rotate){
		std::cout << "I'm rotating" << std::endl;
		source.read((char *) first, sizeof(float) *12);
		source.read(waste, 2);
		for(int vert= 0; vert< 4; vert++)
			matrixmult (this->transform.R , first, out , 3, 3, 4 );
		
		return 0;
	}else{

		source.read((char *) out, sizeof(float) *12);
		source.read(waste, 2);
		
		
	return 0;
	}
	//printSur(out);

}


int StlReader :: openFile (std::string newname, bool overwrite){
	if(source.is_open()) return -1;
	source.open(newname, std::ios::in | std::ios::binary); 
	if(!source.is_open()) {
		std::cout << "could not open "<< newname << std::endl;
		return -2;
	}
	char front[8];
	 source.read(front, 8);
	 std::cout << front << std::endl;


	if(strcmp(front, "solid") == 0){
		std::cout << "not binary" << std::endl;
		isbinary = false;
		char firstline[256];
		source.getline(firstline, 256);
	}else{
		
		isbinary = true;
		char header [ 72];
		source.read(header, 72);
		std::cout << header << std::endl;
			if(source.fail())std::cout << "3 in failing failing" << std::endl;
		uint32_t temp = 0;
		source.read((char *) &temp, sizeof(temp));
		//std::cout << "got value" <<temp << std::endl;
		stats.numbsurface = (uint32_t) temp; 
	}
	open = true;
	sur in;
	this->getSurface(in);
	
	//std::cout << "read done numbsurface "<< stats.numbsurface  << std::endl;
	return 0;
}




int StlReader :: getStats (void){
	//read through surfaces 
	float max = std::numeric_limits<float>::max()/10 ;
	for(int i =0; i < 3; i++){
		this->stats.extrema[2*i] = (float)   max ;
		this->stats.extrema[2*i +1] = (float) -1 * max ;
	//	std::cout << "extrema i is "<< this->stats.extrema[2*i +1]  << std::endl;
	}
	//this->stats.print();
	if(this->isbinary){
	
		sur in;// norm[3], v1[3], v2[3], v3[3];
		
		float cur[2]; // min max
		for( int surface = 0; surface < (int) this->stats.numbsurface; surface++){
		
			getSurface(in);	
			//printSur(in); 
			for(int i =0 ; i < 3 ; i++)
				this->stats.avenorm[i] += in[i];

			for(int j = 0; j< 3 ; j++){
				if(in[1*3 +j] <= in[2*3 + j]) {
					cur[0] = in[1*3 + j];
					cur[1] = in[ 2 *3+ j];
				}
				else{
					cur[0] = in[2*3 +j];
					cur[1] = in[1*3 +j];
				}
				if(in[3*3 +j] > cur[1]) cur[1] = in[3*3 + j];
				else if(in[ 3*3+j] < cur[0]) cur[0] = in[3*3 +j];
				if(cur[0] < this->stats.extrema[2*j]) this->stats.extrema[2*j] =cur[0];
				if(cur[1] > this->stats.extrema[2*j + 1]) this->stats.extrema[2*j +1] =cur[1];
			
			}
		
		}
		for ( int i = 0; i < 3 ; i++)
			this->stats.avenorm[i] /= this->stats.numbsurface;
		//std::cout << this->stats.numbsurface  << std::endl;
	}
	return -1;
}




void StlStats:: print(void){
	std::cout << " numbsurface "; 
	std::cout << this->numbsurface << std::endl;
	std::cout << "x: "<< this->extrema[0]<< " -- "<< this->extrema[1] ;
	std::cout << "  y: "<< this->extrema[2]<< " -- "<< this->extrema[3];
	std::cout << "  z: "<< this->extrema[4]<< " -- "<< this->extrema[5] <<std::endl;
	std::cout << " avenorm " << this->avenorm[0] << " -- " << this->avenorm[1];
	std::cout << " -- " << this->avenorm[2] << std::endl;
}


int StlReader :: getFeatures(Objstats * stats){
	Objstats mystats;
	float cross[3];
	float total = 0;
	float partial = 0;
	sur cur;
	for(uint32_t i = 0; i < this->stats.numbsurface; i++){
		getSurface(cur);
		
		//volume = abs (a dot (b cross c)) /6 x sign 
		cross[0] = cur[2 *3 + 1]*cur[3*3 + 2]-cur[2*3 + 2]*cur[3*3 + 1];
 		cross[1] = cur[2*3 + 2]*cur[3*3 + 0]-cur[2*3 + 0]*cur[3*3 + 2];
  		cross[2] = cur[2*3 + 0]*cur[3*3 + 1]-cur[2*3 + 1]*cur[3*3 + 0];
	
  		partial = cross[0] * cur[1*3 + 0] + cross[1] * cur[1*3 + 1] + cross[2] * cur[1*3 + 2];

  		partial = abs(partial) / 6.0;

  		float normsum = 0;
  		//normal points awway from object
  		for( int j = 0; j < 3; j++)
  			normsum += cur[0*3 + i];
  		//see if dot product of surface normal is positive
  		if(normsum < 0) {
  			//negtive faceing toward subtract
  			
  			total -= partial;
  		}else total += partial;

	}
	mystats.volume = total;
	std::cout << "total volume  was:"<< total  << std::endl;
	* stats = mystats;
	return 0;
}



void setsquare( float rad, int a, int b, float * mat){
	//assumes 3 x3  a > b
	float cosTemp =0;
	float sinTemp = 0;
	 cosTemp = cos(rad);
	 sinTemp = sin(rad);
	 mat[ a + 3*a] = cosTemp;
	 mat[ b + 3*a ] = -1 * sinTemp;
	 mat[a + 3*b] = sinTemp;
	 mat[b + 3*b] = cosTemp;
	 return;
}



int StlReader :: setRotation (float spin[3]){

	//calculate the rotation matrix 
	// assume inputs in radians 

	// start with identity 
	float tempR [9];

	//x then y then z
	
	 float Rx[9];
	 Rx[0] = 1;
	setsquare(spin[0], 1 ,2, Rx);

	float  Ry[9];
	Ry[4] = 1;
	setsquare(spin[1], 0 , 2, Ry);

	float Rz[9];
	Rz[8] = 1;
	setsquare(spin[2], 0, 1, Rz);
	
	matrixmult (Rx , Ry, tempR, 3, 3, 3 );
	matrixmult (tempR , Rz, this->transform.R, 3, 3, 3 );
	this->transform.rotate = true;
	return 0;

}


int StlReader :: restReading(void){
	source.clear();
	source.seekg(84);
	return 0;
}