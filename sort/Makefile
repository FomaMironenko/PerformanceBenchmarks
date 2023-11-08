CPP=clang++-10

run: main
	./main

main: main.cpp sort.cpp
	$(CPP) -g -O3 -march=native  -o main main.cpp sort.cpp
