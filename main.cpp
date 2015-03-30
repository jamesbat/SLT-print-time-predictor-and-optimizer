
 #define PI 3.14159265

#include "stlReader.h"
#include "predictor.h"


void pingArray(float * A, int row, int col){
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

int main(void)
{

  std::cout << "Hello World!" << std::endl;
  StlReader reader;
   std::string basename = "/home/accts/jcb97/proj/stls/";


  /*reader.openFile(basename + "data/9.stl", false);
  reader.getStats();
  reader.restReading();
 Objstats stats;
  FeatureFinder finder; 
  finder.getFeatures( &stats, &reader);
   reader.restReading();*/


 Predictor guesser;
  guesser.learnFrom("./data");
  int buildtime =  guesser.predict("/home/accts/jcb97/proj/stls/data/57.stl");
  std::cout << " got :" << buildtime << "to print 57"<< std::endl;
  guesser.store("currentModel");
  double score = guesser.test("./data");
  std::cout <<" guesser got" << score << std::endl;
  //Predictor secondguesser;
  //secondguesser.read("currentModel");
  //score = secondguesser.test("./data");
  //std::cout <<" secondguesser got" << score << std::endl;
  /*
   float spin [3];
    int dim[3];
   dim[0] =300;
  dim[1] = 300;
  dim[2] = 300;
 
  for(int i = 25 ; i <41; i++){
    StlReader reader;
    std::string curname = basename+ std::to_string(i) +".stl";
    reader.openFile(curname, false);
    std::cout << curname << std::endl;
    for(int rot = 0; rot < 4; rot ++){

      for(int j = 0; j <3; j++ )
      {
        if(rot != 0) spin[j] = rand() * PI;
        else  spin[j] = 0;
      }
       reader.setRotation(spin);
      reader.getStats();
      reader.restReading();
 
      reader.shrinkToFit(dim, false );
          reader.restReading();
      reader.setDown(false);
          reader.restReading();
      std::string outname = basename + "data/" + std::to_string(i*3 + rot)+".stl";
      reader.saveObject(outname);

      //pingArray(reader.transform.trans, 3,1);
       //   std::cout <<  std::endl;
      reader.clear();
    }
  }*/
 /*
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
  reader.restReading();
 reader.stats.print();
 int dim[3];
 dim[0] =300;
 dim[1] = 300;
 dim[2] = 300;
 reader.shrinkToFit(dim,true );
 reader.setDown(false);
 reader.restReading();
    reader.getStats();
  

 reader.stats.print();
  reader.restReading();



  
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