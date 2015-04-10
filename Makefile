CC=g++
CFLAGS=  -Wall -g3 -std=c++11 -fpermissive
LIB = ./cpp/src/



all:    main

main:   ./main.cpp  stlReader.h ./stlReader.cpp ./featureFinder.cpp ./predictor.cpp  ./cpp/src/ap.cpp ./cpp/src/linalg.cpp	./cpp/src/alglibmisc.cpp ./cpp/src/dataanalysis.cpp	./cpp/src/alglibinternal.cpp ./optimizer.cpp
		${CC} ${CFLAGS}  ./main.cpp ./stlReader.cpp ./featureFinder.cpp ./predictor.cpp ./cpp/src/ap.cpp ./cpp/src/linalg.cpp	./cpp/src/alglibmisc.cpp  ./cpp/src/dataanalysis.cpp  ./cpp/src/statistics.cpp ./cpp/src/specialfunctions.cpp ./cpp/src/solvers.cpp ./cpp/src/optimization.cpp 	./cpp/src/alglibinternal.cpp ./optimizer.cpp -o main

clean:
	re -f 

