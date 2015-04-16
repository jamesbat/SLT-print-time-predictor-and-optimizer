
#include "predictor.h"
#include "cpp/src/ap.h"
#include "cpp/src/linalg.h"
#include "cpp/src/alglibinternal.h"
#include "cpp/src/dataanalysis.h"
#include "stlReader.h"

//features followed by print time in minutes 

void testArray(float * A, int row, int col){
  if(row * col > 100) {
    printf("error huge array print\n");
    return;
  }
  for(int i = 0; i < row; i++){
    for(int j = 0 ; j < col; j++)
    std::cout <<A[i*col + j] << " ~ " ;
    std::cout <<std::endl;
  }
 std::cout <<std::endl;
}
void getArray(alglib::real_2d_array  A, int row, int col){
  if(row * col > 1000) {
    printf("error huge array print\n");
    return;
  }

  for(int i = 0; i < row ; i++){
    for(int j = 0 ; j < col; j++)
    std::cout << A(i, j) << " ~ " ;
    std::cout <<std::endl;
  }
 std::cout <<std::endl;
}


void Predictor ::store(std::string filename){
	std::ofstream output;
	output.open(filename, std::ios::out);
	std::cout << "storing " ;
	for(int i = 0;  i < featDim; i++){
		output << W[i] << ' ';
		std::cout << W[i] << " ";
	}
  }
void Predictor ::read(std::string filename){
	std::ifstream input;
	input.open(filename, std::ios::in);
	std::cout << "\nreading Predictor " ;
	for(int i = 0;  i < featDim; i++){
		input >> W[i];
		std::cout << W[i] << " ";
	}
	printf("\n");
	educated = true;

}



int Predictor ::learnFrom(std::string filename){
	int numbpoints = 0;
	Objstats feature;

	alglib::real_2d_array data;
	//alglib::real_2d_array times;
	//times in minutes 

	float printTime =0;
	std::string line;
	std::string name;
	std::string::size_type divider =0;
	reader.close();
	//open file 
	std::ifstream input;
	input.open(filename, std::ios::in);
	if(!input.is_open()) {
		std::cout << "could not open "<< filename << std::endl;
		return -2;
	}//

	  input >> numbpoints ;

	if(numbpoints == 0) std::cout << "zero data  points" << std::endl;
	std::cout << "there are "<< numbpoints<< " data points " << std::endl;
	
	data.setlength(numbpoints, featDim + 1);
	//times.setlength(numbpoints, 1);
	


	std::string front = "/home/accts/jcb97/proj/stls/data/";
	std::string back = ".stl";
//pull out first line with number of obj 
std::getline(input, line);

	for(int i = 0; i < numbpoints; i++){	
	//parse each line
		std::getline(input, line);


	//string is file name
		divider = line.find('\t',0 );
		name = line.substr(0, divider );
		//std::string objname(temp);
		
		std::cout << "opening "<< name  << std::endl;
	//open file in reader
		reader.openFile(front + name + back, false);
		reader.getStats();


		reader.restReading();

	
	//get features 	
		finder.getFeatures(&feature, &reader);
		for(int j = 0; j < featDim; j++)
			data(i, j) = feature.data[j];
		
		reader.close();
 
	//parse out time 
	
		printTime = std::stoi(&line[divider], &divider, 10);
		printTime = printTime * 60; //hours first 
		divider = line.find(':', 0 );//then minutes 
		name = line.substr(divider +1, line.length() - divider -1 );

		printTime = printTime + std::stoi(name, &divider, 10);
	//	std::cout << "div is :"<<divider  << std::endl;
		data(i, featDim ) = printTime;

		//std::cout << "finnished obj TIME :"<<printTime  << std::endl;
	
	}
	alglib::linearmodel mod;
	alglib::lrreport rep;
	alglib::ae_int_t info = 0;
std::cout << "builing model "<< std::endl;

	alglib::lrbuild(data, numbpoints, featDim, info, mod, rep);
	std::cout << "report:" << info <<std::endl;
	alglib::real_1d_array coef;
	coef.setlength(featDim+10);
	std::cout<< "rms:" << lrrmserror(mod, data, numbpoints) << std::endl;

	std::cout << "extracting terms "<< std::endl;
	info = featDim+1;
	alglib::lrunpack(mod, coef, info );
		std::cout << "terms out"<< std::endl;
	for(int i = 0; i < info; i++)
		W[i] = coef[i];
	educated = true;
	//store in w
	std::cout << "done "<< std::endl;
	return 0;
}
float Predictor ::predict(std::string filename){

	if(! educated) return -1;
	reader.close();
	Objstats feature;
	//open file 
	std::ifstream input;
	reader.openFile(filename, false); 
	reader.getStats();
	reader.restReading();
	finder.getFeatures(&feature, &reader);
	

	//std::cout <<"  name:"<< filename << " + ";
	float out = 0.0;
	for(int i = 0; i < featDim; i++){
		out += feature.data[i]* W[i];
		printf("%g\t\t", feature.data[i] );
	}
	out += W[featDim];
	if(out <0) printf("predicor error\n");
	//return it
	return out;
} 
float Predictor ::predictObj(StlReader * reader, bool rot){

	if(! educated) return -1;

	Objstats feature;
	reader->getStats();
	reader->restReading();
	//if(rot)	
	finder.getFeatures(&feature, reader);	
	//else finder.getFeatures(&feature, reader);

	float out = 0.0;
	for(int i = 0; i < featDim; i++){
		out += feature.data[i]* W[i];
//		printf("#%g\t\t", feature.data[i] );
	}
//	printf("\n");
	out += W[featDim];
	if(out <0) printf("predicor obj error\n");
	//return it
	return out;
} 


int Predictor ::test(std::string filename){
	if(! educated) return -1;
	//r^2	//R = 1- ssres  / sstot
	//sstot = sum (yi - yave)^2
	//ssres = sum (yi - prediced yi)^2

	//build build time and etime arrays
		int numbpoints = 0;

	double aveErr = 0.0;
	double percentErr =0.0;
	float printTime =0;
	std::string line;
	std::string name;
	std::string::size_type divider =0;
	reader.close();
	//open file 
	std::ifstream input;
	input.open(filename, std::ios::in);
	if(!input.is_open()) {
		std::cout << "could not open "<< filename << std::endl;
		return -2;
	}//

	  input >> numbpoints ;

	if(numbpoints == 0) std::cout << "zero data  points" << std::endl;
	const int constpoints = numbpoints;

	double times[constpoints];
	double eTimes[constpoints];
	std::cout << "there are "<< numbpoints<< " data points " << std::endl;


	std::string front = "/home/accts/jcb97/proj/stls/data/";
	std::string back = ".stl";
//pull out first line with number of obj 
	std::getline(input, line);
	int timeTotal = 0;
	
	 printf("surf area       obj vol       layers        sup vol        real~predicted\n");
	for(int i = 0; i < numbpoints; i++){	
	//parse each line
		std::getline(input, line);


	//string is file name
		divider = line.find('\t',0 );
		name = line.substr(0, divider );
		//std::string objname(temp);
		
	
	//predict time
		eTimes[i] = predict(front + name + back);
		
 
	//read time 
	
		printTime = std::stoi(&line[divider], &divider, 10);
		printTime = printTime * 60; //hours first 
		divider = line.find(':', 0 );//then minutes 
		name = line.substr(divider +1, line.length() - divider -1 );

		printTime = printTime + std::stoi(name, &divider, 10);
		times[i] = printTime;
		printf("\t%g ~ %g\n", times[i], eTimes[i] );
		timeTotal += printTime;
		aveErr += abs(eTimes[i] - times[i]);
		percentErr += abs(eTimes[i] - times[i])/ times[i];
	
	}
	aveErr /= numbpoints;
	percentErr /= numbpoints;
	percentErr *= 100;
	std::cout << std::endl;
	std::cout << "average error:" << aveErr << " perecnt err:"<< percentErr << std::endl;
	double yave = timeTotal / numbpoints;
	 double ssTot = 0;
	 double ssRes = 0;
	 //R = 1- ssres  / sstot
	//sstot = sum (yi - yave)^2
	//ssres = sum (yi - prediced yi)^2
	
	 for( int i = 0 ; i < numbpoints; i++){
	 	ssTot += pow((times[i] - yave), 2 );
	 	ssRes += pow(( times[i]- eTimes[i] ), 2 );
	 //	std::cout << times[i] << " ~ " << eTimes[i] << std::endl;
	 }
	 double R = 1 - ssTot/ ssRes;

	//get yave
	return R;

}