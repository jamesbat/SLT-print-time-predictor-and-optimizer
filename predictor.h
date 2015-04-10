#include "stlReader.h"
#define featDim 4
//the dimention of the featur vector


class Predictor {
	StlReader reader;
	FeatureFinder finder;
public:
	bool educated;
	float W[featDim +1 ];
	int learnFrom(std::string filename);
	int test(std::string filename);
	void store(std::string filename);
	void read(std::string filename);
	int predict(std::string filename); 
};