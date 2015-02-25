
 

#include "stlReader.h"
int main(void)
{

  std::cout << "Hello World!" << std::endl;
  StlReader reader;
  std::string name = "/home/accts/jcb97/proj/stls/Teapot.stl";
  reader.openFile(name, false);
 
  reader.getstats();
  reader.stats.print();
  reader.restreading();
//  reader.getfeatures();
   
 // reader.restreading();
 //  reader.getstats();
  
// reader.stats.print();

  return 0;
}