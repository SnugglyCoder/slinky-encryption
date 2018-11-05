#include <iostream>

#include <cmath>
#include <vector>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

int main( int argc, char* argv[] )
{
    if( argc != 3 )
    {
        std::cerr << "Usage: ./tests < key file >    < data file >" << std::endl;

        exit(1);
    }

    InitTable();

    std::vector< unsigned char > key = { 0x00, 0x01, 0x02, 0x03, 0xF1 };

    std::vector< unsigned char > data = { 0xF0, 0x04, 0x03, 0x02, 0x01, 0x00 };

    std::vector< unsigned char > dataCopy( data );

    std::ifstream file("pride_and_prejudice.txt", std::ios::ate | std::ios::binary );

    std::vector< unsigned char > filedata( file.tellg() );

    file.seekg( 0 );

    file.read( (char*)&filedata[0], filedata.size() );

    std::vector< unsigned char > filedatacopy( filedata );

    std::cout << "Size before compression: " << filedata.size() << std::endl;

   // CompressData( filedata );

    std::cout << "Size after compression: " << filedata.size() << std::endl;

   // DecompressData( filedata );

    std::cout << "Size after decompression: " << filedata.size() << std::endl;

    for( unsigned int i = 0; i < filedata.size(); i++ )
    {
        if( filedata[ i ] != filedatacopy[ i ] )
        {
            std::cout << "Not the same" << std::endl;
        }
    }

    std::cout << "Shuffling" << std::endl;

    std::vector< unsigned char > shuffleKey;

    for( unsigned int i = 0; i < 13426; i++ )
    {
        shuffleKey.push_back( filedata[ i ] );
    }

    int keystart = 684759 % shuffleKey.size();

    int keyposition = keystart;

    std::cout << keyposition << std::endl;

    filedata = ShuffleBits( filedata, shuffleKey, keyposition );

    std::cout << "Shuffled" << std::endl;

    filedata = UnshuffleBits( filedata, shuffleKey, keyposition );

    std::cout << "Unshuffled" << std::endl;

    int diffbits = 0;

    for( unsigned int i = 0; i < filedata.size(); i++ )
    {
        if(filedata[i] != filedatacopy[i])
        {
            diffbits++;
        }
    }

    std::cout << "Difference: " << diffbits << std::endl;

    std::cout << "Unit Tests" << std::endl;

    int keyPosition = 0;

    std::cout << "\tTesting AddKey" << std::endl;

    keyPosition = AddKey( data, 0, key );

    if( data[ 0 ] != 0xF0 && data[ 1 ] != 0x05 && data[ 2 ] != 0x01 && data[ 3 ] != 0x01 && data[ 4 ] != 0xF0 && data[ 5 ] != 0x00 )
    {
        std::cout << "\t\tValues wrong" << std::endl;
        exit(1);
    }

    std::cout << "\t\tValues are correct" << std::endl;
    
    if( keyPosition != 1 )
    {
        std::cout << "\t\tIncorrect key position returned" << std::endl;

        exit( 1 );
    }

    std::cout << "\t\tKey position is correct" << std::endl;

    std::cout << std::endl;

    std::cout << "\tTesting data chain" << std::endl;

    data = dataCopy;

    keyPosition = DataChain( data, keyPosition, key );

    if( data[ 5 ] != 0x01 && data[ 4 ] != 0x00 && data[ 3 ] != 0x02 && data[ 2 ] != 0x01 && data[ 1 ] != 0x05 && data[ 0 ] != 0xF5 )
    {
        std::cout << "\t\tValues are wrong" << std::endl;

        exit(1);
    }
    
    std::cout << "\t\tValues are correct" << std::endl;

    if( keyPosition != 2 )
    {
        std::cout << "\t\tKey position is wrong" << std::endl;
        exit(1);
    }
    
    std::cout << "\t\tKey position is correct\n" << std::endl;

    std::cout << "\tTesting SubBytes" << std::endl;

    data = dataCopy;

    keyPosition = SubBytes( data, keyPosition, key );

    // Insert sub bytes check here will take some time to figure out

    // Test expand 

    std::cout << "\tTesting Expand" << std::endl;

    data = dataCopy;

    key = { 0xEF, 0x19, 0x55, 0x3F, 0xF1 };
    
    keyPosition = Expand( data, keyPosition, key );

    if( data.size() != 40 )
    {
        std::cout << "\t\tSize of expanded data is incorrect" << std::endl;
        std::cout << "\t\tSize is " << data.size() << std::endl;
        exit(1);
    }

    std::cout << "\t\tValues Correct" << std::endl;

    key = LoadKey( std::string( argv[1] ) );

    data = LoadKey( std::string( argv[ 2 ] ) );

    // Used to compare changes from primitive and inverse primitive to 
    // evaluate if they are infact inverses of each other
    dataCopy = data;

    keyPosition = 0;

    AddKey( data, keyPosition, key );

    AddKey( data, keyPosition, key );

    std::cout << "Tests with files" << std::endl;

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

    std::cout << "SubBytes and Inverse SubBytes Verification" << std::endl;
    
    keyPosition = SubBytes( data, keyPosition, key );

    InverseSubBytes( data, keyPosition, key );

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