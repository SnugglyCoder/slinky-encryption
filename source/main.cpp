#include <iostream>

#include <cmath>
#include <vector>
#include <sstream>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

std::vector< unsigned char > SlinkyDecryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key );
std::vector< unsigned char > SlinkyEncryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key );

int rounds;

int main( int argc, char* argv[] )
{
    if( argc != 5 )
    {
        std::cout << "Usage: ./slinky <key> <input> <control-output> <bit-output> <number of rounds>" << std::endl;
        exit(1);
    }

    rounds = std::atoi(argv[5]);

    InitTable();

    std::vector< unsigned char > key = LoadKey( std::string( argv[ 1 ] ) );

    std::vector< unsigned char > fileData = LoadKey( std::string( argv[ 2 ] ) );

    std::vector< unsigned char > data;

    for( unsigned int i = 0; i < 100; i++ )
    {
        data.push_back( fileData[ i ] );
    }

    std::vector< unsigned char > dataCopy( data );

   // std::cout << "Encrypting" << std::endl;

    SlinkyEncryption( data, key );

    std::vector< unsigned char > encryptedData( data );

    //std::cout << "Decrypting" << std::endl;

    //SlinkyDecryption( data, key );

    //std::cout << "Comparing" << std::endl;

    //for( int i = 0; i < data.size(); i++ )
    {
        //if( data[i] != dataCopy[i])
        {
            //std::cout << "Process failed" << std::endl;
            //exit(1);
        }
    }

    std::ofstream fileOut( std::string(argv[3]) + std::string("-control.txt") );

    for( unsigned int i = 0; i < encryptedData.size(); i++ )
    {
        fileOut << std::bitset<8>(encryptedData[i]);
    }
  
    fileOut.close();

    for( int i = 0; i < dataCopy.size(); i++ )
    {
        data = dataCopy;

        data[ i ] ^= 0x80;

        SlinkyEncryption( data, key );

        std::stringstream filename;

        filename << argv[4] << "-bit" << i  << ".txt";

        std::ofstream fileOut( filename.str() );

        for( unsigned int i = 0; i < data.size(); i++ )
        {
            fileOut << std::bitset<8>(data[i]);
        }
    
        fileOut.close();
    }

    return 0;
}

int keyPosition = 0;

std::vector< unsigned char > SlinkyEncryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key )
{
    for( unsigned int i = 0; i < rounds; ++i )
    {
       // std::cout << "Round " << i + 1 << std::endl;

        keyPosition = ForwardChain( data, keyPosition, key );

        keyPosition = SubBytes( data, keyPosition, key );

        keyPosition = ReverseChain( data, keyPosition, key );

        keyPosition = AddKey( data, keyPosition, key );

        keyPosition = Expand( data, keyPosition, key );

       // std::cout << data.size() << std::endl;

        CompressData( data );

       // std::cout << data.size() << std::endl;

        data = ShuffleBits( data, key, keyPosition );
    }

	return data;
}

std::vector< unsigned char > SlinkyDecryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key )
{
    for( unsigned int i = 0; i < rounds; ++i )
    {
        data = UnshuffleBits( data, key, keyPosition );

        DecompressData( data );

        keyPosition = Unexpand( data, keyPosition, key );

        keyPosition = InverseAddKey( data, keyPosition, key );

        keyPosition = InverseReverseChain( data, keyPosition, key );

        keyPosition = InverseSubBytes( data, keyPosition, key );

        keyPosition = InverseForwardChain( data, keyPosition, key );
    }

	return data;
}