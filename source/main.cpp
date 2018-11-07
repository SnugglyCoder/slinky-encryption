#include <iostream>

#include <cmath>
#include <vector>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

int main( int argc, char* argv[] )
{
    if( argc != 4 )
    {
        std::cout << "Usage: ./slinky <key> <input> <output>" << std::endl;
        exit(1);
    }

    InitTable();

    std::vector< unsigned char > key = LoadKey( std::string( argv[1] ) );

    std::vector< unsigned char > data = LoadKey( std::string( argv[ 2 ] ) );

    std::vector< unsigned char > dataCopy( data );

    int keyPosition = 0;

    
}