#include "stlReader.h"
#define featDim 3
//the dimention of the featur vector


class Predictor {
	StlReader reader;
	FeatureFinder finder;
public:
	bool educated;
	float W[featDim];
	int learnFrom(std::string filename);
	int test(std::string filename, std::string filePrefix);
	void store(std::string filename);
	void read(std::string filename);
	int predict(std::string filename); 
};