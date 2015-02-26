
 #define PI 3.14159265

#include "stlReader.h"
int main(void)
{

  std::cout << "Hello World!" << std::endl;
  StlReader reader;
  std::string name = "/home/accts/jcb97/proj/stls/Teapot.stl";
  reader.openFile(name, false);
 
  reader.getStats();
  reader.stats.print();
  reader.restReading();
  Objstats stats;
  reader.getFeatures( &stats);
   reader.restReading();


   float spin[3];
   spin[0] = 0.0;
    spin[1] = PI/2;
     spin[2] = PI/2;
   reader.setRotation(spin);
  reader.restReading();
   reader.getStats();
  
 reader.stats.print();

  return 0;
}