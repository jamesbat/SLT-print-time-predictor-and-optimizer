
 

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
   reader.hello();
  reader.restReading();
   reader.getStats();
  
 reader.stats.print();

  return 0;
}