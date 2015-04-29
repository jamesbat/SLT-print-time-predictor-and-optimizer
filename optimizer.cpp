#include <limits>
#include "stlReader.h"
#include "predictor.h"

#include <cstring>
#include <iostream>
#include <cmath>
#include <stdlib.h>

#define stable  .00001
#define epslon  .001
#define PIE  3.14159265358979323846

void printRot( rot turn){
	for(int i = 0; i < 3; i++)
		printf("%g\t", (turn[i]*180)/ PIE);
}


void Optimizer ::bestRotate(StlReader * reader, rot * bestRot){
	timer t;
	t.start();
	Predictor pred;
	pred.read(this->filename);
	rot rotation;
	rot gradient;
	rot bestCurRot;
	float totalGrad;
	float bestTime = 600 *2000;
	float curTime;
	curTime = pred.predictObj(reader, false);
	printf("\nStart at:%g\n\n", curTime);
	for(int tries = 0; tries < this->numbTries; tries ++ ){
		// rand gen a new rotation 
		for(int i = 0 ; i < 3; i++)
			rotation[i] = rand() * 2 * PIE / RAND_MAX;

		for(int steps = 0; steps < this->numbSteps; steps ++){
			//get gradient
			reader->setRotation(rotation);
			reader->setDown(false);
			curTime = pred.predictObj(reader, true);
			//printf("\nRot Predictions:%g\t", curTime);
			for(int i = 0; i < 3; i++){
				rotation [i] += epslon;
				reader->setRotation(rotation);
				reader->setDown(false);

				totalGrad = pred.predictObj(reader, true);
			//	printf("%g\t", totalGrad);
				gradient[i] +=  alpha *((curTime - totalGrad) / epslon);
				rotation [i] -= epslon;
			}
			totalGrad = 0;
			//update rotations 
		//	printf("\nGradiants: ");
			for(int i = 0; i < 3; i++){
				rotation[i] += gradient[i];
				totalGrad += fabs(gradient[i]);
			}
		
			if(totalGrad < stable){
				break;//stablized
			}
		}

		curTime = pred.predictObj(reader, false);
		if(curTime < bestTime && curTime >= 1){
			for(int i = 0 ; i < 3; i++)
				bestCurRot[i] = rotation[i];
			printf("Moved from %g to %g \n", bestTime,curTime );
				bestTime = curTime;
			
		}
	}
	for(int i = 0 ; i < 3; i++)
	 * bestRot[i] = bestCurRot[i];
	double endtime = t.end();
printf("\nFinal time min: %g secs used:%g \n\n", bestTime, endtime);

	return;
} 