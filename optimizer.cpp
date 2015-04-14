#include <limits>
#include "stlReader.h"
#include "predictor.h"

#include <cstring>
#include <iostream>
#include <cmath>
#include <stdlib.h>

#define stable  .001
#define epslon  .0001
#define PIE  3.14159265358979323846


void Optimizer ::bestRotate(StlReader * reader, rot * bestRot){
	Predictor pred;
	pred.read(this->filename);
	rot rotation;
	rot gradient;
	rot bestCurRot;
	float totalGrad;
	int bestTime = 600 *2000;
	int curTime;
	curTime = pred.predictObj(reader);
	printf("start at:%d\n", curTime);
	for(int tries = 0; tries < this->numbTries; tries ++ ){
		// rand gen a new rotation 
		for(int i = 0 ; i < 3; i++)
			rotation[i] = rand() * 2 * PIE / RAND_MAX;

		for(int steps = 0; steps < this->numbSteps; steps ++){
			//get gardient
			reader->setRotation(rotation);
			reader->setDown(false);
			curTime = pred.predictObj(reader);
			for(int i = 0; i < 3; i++){
				rotation [i] += epslon;
				reader->setRotation(rotation);
				reader->setDown(false);
				gradient[i] +=  alpha *((curTime - pred.predictObj(reader)) / epslon);
				rotation [i] -= epslon;
			}
			totalGrad = 0;
			//update gradiantes 
			for(int i = 0; i < 3; i++){
				rotation[i] += gradient[i];
				totalGrad += gradient[i];
			}
			if(totalGrad < stable)break;
		}
		printf("\nended try\n");
		if(curTime < bestTime){
			for(int i = 0 ; i < 3; i++)
				bestCurRot[i] = rotation[i];
			printf("Moved from %d to %d \n", bestTime,curTime );
				bestTime = curTime;
			
		}
	}
	for(int i = 0 ; i < 3; i++)
	 * bestRot[i] = bestCurRot[i];
printf("Final time %d \n", bestTime );
	return;
} 