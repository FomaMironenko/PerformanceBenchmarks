CPP=g++

run: main
	./main

main: main.cpp matrix.cpp
	$(CPP) --std=c++17 -g -O3 -march=native  -o main main.cpp matrix.cpp
