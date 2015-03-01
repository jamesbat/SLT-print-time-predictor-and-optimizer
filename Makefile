CC=g++
CFLAGS= -pedantic -Wall -g3 -std=c++11




all:    main

main:   ./main.cpp  stlReader.h ./stlReader.cpp ./featureFinder.cpp
		${CC} ${CFLAGS}  ./main.cpp ./stlReader.cpp ./featureFinder.cpp -o main


clean:
	re -f 