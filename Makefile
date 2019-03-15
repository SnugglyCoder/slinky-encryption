all: slinky sequence-alignment doit #tests

slinky: source/main.cpp
	g++ -std=c++11 source/main.cpp -o slinky

#tests: source/tests.cpp
#	g++ -std=c++11 source/tests.cpp -o tests

sequence-alignment: source/sequence-alignment.cpp
	g++ -std=c++11 source/sequence-alignment.cpp -o sequence-alignment
	
doit: source/init.cpp
	g++ -std=c++11 source/init.cpp -o doit