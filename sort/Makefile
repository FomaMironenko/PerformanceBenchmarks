CPP=clang++-10

run: main
	./main

main: main.cpp sort.cpp
	$(CPP) -std=c++17 -g -O3 -march=native  -o main main.cpp sort.cpp
