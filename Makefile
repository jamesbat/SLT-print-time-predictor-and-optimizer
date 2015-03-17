CC=g++
CFLAGS=  -Wall -g3 -std=c++11 -fpermissive
LIB = ./cpp/src/



all:    main

main:   ./main.cpp  stlReader.h ./stlReader.cpp ./featureFinder.cpp ./predictor.cpp  ./cpp/src/ap.cpp ./cpp/src/linalg.cpp	./cpp/src/alglibmisc.cpp	./cpp/src/alglibinternal.cpp
		${CC} ${CFLAGS}  ./main.cpp ./stlReader.cpp ./featureFinder.cpp ./predictor.cpp ./cpp/src/ap.cpp ./cpp/src/linalg.cpp	./cpp/src/alglibmisc.cpp	./cpp/src/alglibinternal.cpp -o main

clean:
	re -f 