#include "stlReader.h"
#define featDim 4
//the dimention of the feature vector


class Predictor {
	StlReader reader;
	FeatureFinder finder;
public:
	bool educated; //has model been trained
	float W[featDim +1 ]; // the coeffients
	int learnFrom(std::string filename);
	//use file as traing  set biuld new coeffients
	int test(std::string filename);
	//use file as test training set use old coeffients 
	void store(std::string filename);
	void read(std::string filename);
	float predict(std::string filename);
	float predictObj(StlReader * read, bool rot);
 
};  