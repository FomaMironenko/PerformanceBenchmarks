CPP=clang++-10

run: main
	./main

main: main.cpp
	$(CPP) --std=c++17 -g -O3 -march=native -lpthread -o main main.cpp
