
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
  FeatureFinder finder;
  finder.getFeatures( &stats, &reader);
   reader.restReading();


   float spin[3];
   spin[0] = 0.0;
    spin[1] = PI/2;
     spin[2] = PI/2;
   reader.setRotation(spin);
  reader.restReading();
   reader.getStats();
  
 reader.stats.print();
 reader.setDown(false);
 reader.restReading();
    reader.getStats();
  

 reader.stats.print();
  reader.restReading();



  /*
  reader.restReading();
 reader.saveObject("/home/accts/jcb97/proj/stls/Teapotspin.stl");
std::cout << " -------------------------------"<< std::endl;
StlReader modreader;
modreader.openFile("/home/accts/jcb97/proj/stls/Teapotspin.stl", false);
 reader.restReading();
modreader.getStats();
modreader.stats.print();
*/
  return 0;
}