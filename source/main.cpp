#include <iostream>

#include <cmath>
#include <vector>
#include "../headers/power-table.hpp"

int main()
{
    for( unsigned int i = 1; i < 9; i++ )
    {
        for( int j = 0; j < 256; ++j )
        {
            std::cout << (int)FindNthRoot( powerTable[ (i-1) * 256 + j ], i ) << std::endl;
        }
    }

    /*
    int counter = 0;

    std::vector < uint64_t > lookupTable;

    lookupTable.resize(8*256);

    for( uint64_t i = 0; i < 8; ++i )
    {
        for( uint64_t j = 0; j < 256; ++j )
        {
            lookupTable[ i * 256 + j ] = 1;

            for( unsigned int k = 0; k <= i; ++k )
            {
                lookupTable[ i * 256 + j ] *= j;
            }
        }
    }

    std::cout << "uint64_t expansionTable = { ";

    for( unsigned int i = 0; i < lookupTable.size(); ++i )
    {
        std::cout << "\t" << lookupTable[ i ] << ",\n";
    }

    std::cout << "}";
    */
}