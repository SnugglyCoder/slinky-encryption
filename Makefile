all: tests slinky

slinky: source/main.cpp
	g++ -std=c++11 source/main.cpp -o slinky

tests: source/tests.cpp
	g++ -std=c++11 source/tests.cpp -o tests