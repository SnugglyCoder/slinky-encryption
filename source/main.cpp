#include <iostream>

#include <cmath>
#include <vector>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

int main( int argc, char* argv[] )
{
    InitTable();

    std::vector< unsigned char > key = LoadKey( std::string( argv[1] ) );

    std::vector< unsigned char > data = LoadKey( std::string( argv[ 2 ] ) );

    std::vector< unsigned char > dataCopy( data );

    int keyPosition = 0;

    AddKey( data, keyPosition, key );

    AddKey( data, keyPosition, key );

    std::cout << "Add Key and Inverse Add Key verification" << std::endl;

    for( unsigned int i = 0; i < data.size(); ++i )
    {
        if( data[ i ] != dataCopy[ i ] )
        {
            std::cout << "not equal" << std::endl;

            exit(1);
        }
    }

    std::cout << "Verified" << std::endl;

    keyPosition = DataChain( data, keyPosition, key );

    keyPosition = InverseDataChain( data, keyPosition, key );

    std::cout << "Data Chain and Inverse Data Chain verification" << std::endl;

    for( unsigned int i = 0; i < data.size(); ++i )
    {
        if( data[ i ] != dataCopy[ i ] )
        {
            std::cout << "not equal" << std::endl;

            exit(1);
        }
    }

    std::cout << "Verified" << std::endl;

    std::cout << "Whiten and Blacken verification" << std::endl;

    keyPosition = Whiten( data, keyPosition, key );

    std::cout << "Key Position: " << keyPosition << std::endl;

    Blacken( data, keyPosition, key );

    for( unsigned int i = 0; i < data.size(); ++i )
    {
        if( data[ i ] != dataCopy[ i ] )
        {
            std::cout << "not equal" << std::endl;

            exit(1);
        }
    }

    std::cout << "Verified" << std::endl;
}