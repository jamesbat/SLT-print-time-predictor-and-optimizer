
int Predictor ::learnFrom(std::string filename){
	int numbpoints = 0;
	Objstats feature;

	alglib::real_2d_array data;
	alglib::real_2d_array times;
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
	data.setlength(numbpoints, featDim);
	times.setlength(numbpoints, 1);
	std::cout << "there are "<< numbpoints<< " data points " << std::endl;


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
		//data(i, 2)  = feature.surfaceArea;
		//data(i, 1)  = feature.volume;
		//data(i, 2)  = feature.raftArea;
		//data(i, 0)  = feature.layers;
	//	data(i, 3)  = feature.constant;
		reader.close();

	//parse out time 
	
		printTime = std::stoi(&line[divider], &divider, 10);
		printTime = printTime * 60; //hours first 
		divider = line.find(':', 0 );//then minutes 
		name = line.substr(divider +1, line.length() - divider -1 );

		printTime = printTime *60 + std::stoi(name, &divider, 10);
	//	std::cout << "div is :"<<divider  << std::endl;
		times(i, 0 ) = printTime;

		std::cout << "finnished obj TIME :"<<printTime  << std::endl;
	
	}
	printf("5%ld %ld\n", (long)data.rows(), (long)data.cols());

getArray(data, numbpoints, featDim);

	std::cout << "starting computation"  << std::endl;
	alglib::real_2d_array partial;
	partial.setlength(featDim, featDim);
	// compute (xt*x)^-1 * xt *y
	// partial = x * xt
	alglib::rmatrixgemm(
			featDim, featDim, numbpoints, 1, //dimmentions
			data, 0, 0, 1,//xt input
			data, 0 , 0, 0, // x input
			0 , partial, 0 ,0);// output

	std::cout << "multi 1 done"  << std::endl;
	getArray(partial, featDim, featDim);
	// partial = (xt*x)^-1  f * f 
	alglib::ae_int_t code;
	alglib::matinvreport rep;
	std::cout << "prep done"  << std::endl;

alglib::integer_1d_array pivots;
//int max = numbpoints;
//if(featDim > max) max = featDim;
pivots.setlength(featDim);

alglib::rmatrixlu(partial, featDim, featDim, pivots);

	try
  {
    alglib::rmatrixluinverse(partial, pivots,  code, rep);
  }
  catch (alglib::ap_error e)
  {
    std::cout << "An exception occurred. Exception Nr. " << e.msg << '\n';
  }
  std::cout << "inverse done"<< " " <<code << std::endl;
  alglib::real_2d_array test;
	test.setlength(featDim, numbpoints);
alglib::rmatrixgemm(
			featDim, numbpoints, featDim, 1, //dimmentions
			 partial, 0, 0, 0,//xt input
			data, 0 , 0, 1, // x input
			0 , test, 0 ,0);// output

	
	
getArray(test, featDim, numbpoints);

	std::cout << "checked inverse "<< std::endl;
	getArray(partial, featDim, featDim);
	alglib::real_2d_array steptwo;
	steptwo.setlength(featDim, numbpoints);
	alglib::rmatrixgemm(
			featDim, numbpoints, featDim, 1, //dimmentions
			partial, 0, 0, 0,//(x*xt)^-1 input
			data, 0 , 0, 1, // xt input
			0 , steptwo, 0 ,0);// output

	std::cout << "multi 2 done"  << std::endl;
	getArray(steptwo, featDim, numbpoints);

	alglib::real_2d_array Wout;
	Wout.setlength(featDim, 1);
	rmatrixgemm(
			featDim, 1, numbpoints, 1, //dimmentions
			steptwo, 0, 0, 0,//x input
			times, 0 , 0, 0, //times input
			0 , Wout, 0 ,0);// output
	std::cout << "multi 3 done"  << std::endl;
	for( int i = 0; i < featDim; i++)
		W[i] = Wout[i][0];


	testArray(W, featDim, 1);
	educated = true;
	//store in w
	return 0;