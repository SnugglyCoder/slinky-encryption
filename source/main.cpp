#include <iostream>

#include <cmath>
#include <vector>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

std::vector< unsigned char > SlinkyDecryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key );
std::vector< unsigned char > SlinkyEncryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key );

int main( int argc, char* argv[] )
{
    if( argc != 4 )
    {
        std::cout << "Usage: ./slinky <key> <input> <output>" << std::endl;
        exit(1);
    }

    InitTable();

    std::vector< unsigned char > key = LoadKey( std::string( argv[ 1 ] ) );

    std::vector< unsigned char > fileData = LoadKey( std::string( argv[ 2 ] ) );

    std::vector< unsigned char > data;

    for( unsigned int i = 0; i < 1000; i++ )
    {
        data.push_back( fileData[ i ] );
    }

    std::vector< unsigned char > dataCopy( data );

    //std::cout << "Encrypting" << std::endl;

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

    std::ofstream fileOut( std::string(argv[3]) + std::string(".txt"), std::ios::binary );

    for( unsigned int i = 0; i < encryptedData.size() * 8; i++ )
    {
        if( encryptedData[ i / 8 ] >> ( i % 8 ) == 0x00 )
        {
            fileOut << "0";
        }

        else
        {
            fileOut << "1";
        }
    }

    //std::cout << data.size();

    fileOut.close();

    //std::cout << "Success!" << std::endl;

    for( unsigned int bit = 0; bit < data.size(); bit++ )
    {
        data[ bit / 8 ] = data[ bit / 8 ] ^ ( 0x80 >> ( bit % 8 ) );

        std::vector< unsigned char > dataCopy2(data);

        SlinkyEncryption( data, key );

        std::vector< unsigned char > encryptedData( data );

        //std::cout << "Decrypting" << std::endl;

        //SlinkyDecryption( data, key );

        //std::cout << "Comparing" << std::endl;

        //for( int i = 0; i < data.size(); i++ )
        {
            //if( data[i] != dataCopy2[i])
            {
               // std::cout << "Process failed" << std::endl;
               // exit(1);
            }
        }

        std::stringstream fileOutName;

        fileOutName << argv[3] << bit << ".txt";

        std::ofstream fileOut( fileOutName.str(), std::ios::binary );

        for( unsigned int i = 0; i < encryptedData.size() * 8; i++ )
        {
            if( encryptedData[ i / 8 ] >> ( i % 8 ) == 0x00 )
            {
                fileOut << "0";
            }

            else
            {
                fileOut << "1";
            }
        }

        fileOut.close();

        data = dataCopy;
    }

    return 0;
}

int keyPosition = 0;

std::vector< unsigned char > SlinkyEncryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key )
{
    for( unsigned int i = 0; i < 3; ++i )
    {
        keyPosition = ForwardChain( data, keyPosition, key );

        keyPosition = SubBytes( data, keyPosition, key );

        keyPosition = ReverseChain( data, keyPosition, key );

        keyPosition = AddKey( data, keyPosition, key );

        keyPosition = Expand( data, keyPosition, key );

        //std::cout << data.size() << std::endl;

        CompressData( data );

        //std::cout << data.size() << std::endl;

        data = ShuffleBits( data, key, keyPosition );
    }

    std::cout << data.size() << "\n";

	return data;
}

std::vector< unsigned char > SlinkyDecryption( std::vector< unsigned char >& data, const std::vector< unsigned char >& key )
{
	//int keyPosition = 219816;

    for( unsigned int i = 0; i < 3; ++i )
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