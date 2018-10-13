all: tests slinky

slinky: source/main.cpp
	g++ -std=c++11 -o3 source/main.cpp -o slinky

tests: source/tests.cpp
	g++ -std=c++11 -o3 source/tests.cpp -o tests