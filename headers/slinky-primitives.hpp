#ifndef SLINKY_PRIMITIVES_HPP
#define SLINKY_PRIMITIVES_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <bitset>
#include <sstream>
#include <stack>
#include <iomanip>
#include <algorithm>

#include "sbox.hpp"
#include "power-table.hpp"
#include "compressor.h"
#include "decompressor.h"
#include "modelA.h"

// Simply XOR each byte of data with byte of key
// Cycle through the key as it is done
int AddKey( std::vector< unsigned char >& data, const int keyPosition, const std::vector< unsigned char >& key )
{
	int i = 0;

	for( i; i < data.size(); ++i )
	{
		data[ i ] ^= key[ ( i + keyPosition ) % key.size() ];
	}

	return ( i + keyPosition ) % key.size();
}

int InverseAddKey( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	int datasize = data.size();

	keyPosition = (keyPosition - datasize);

	while( keyPosition < 0 )
	{
		keyPosition += key.size();
	}

	keyPosition %= key.size();


	for( int i = 0; i < data.size(); i++ )
	{
		data[ i ] ^= key[ ( keyPosition + i ) % key.size() ];
	}

	return keyPosition;
}

// XOR the last byte of data with current key byte
// Chain this back through to the beginning of the message
int ForwardChain( std::vector< unsigned char >& data, const int keyPosition, const std::vector< unsigned char >& key )
{
	data[ 0 ] ^= key[ keyPosition ];

	for( int i = 0; i < data.size() - 1; i++ )
	{
		data[ i + 1 ] ^= data[ i ];
	}

	return keyPosition + 1;
}

int InverseForwardChain( std::vector< unsigned char >& data, int keyPosition, const std::vector < unsigned char >& key )
{
	keyPosition -= 1;
	
	for( int i = data.size() - 1; i > 0; i-- )
	{
		data[ i ] ^= data[ i - 1 ];
	}

	data[ 0 ] ^= key[ keyPosition ];

	return keyPosition;
}

int ReverseChain( std::vector< unsigned char >& data, const int keyPosition, const std::vector< unsigned char >& key )
{
	data[ data.size() - 1 ] ^= key[ keyPosition ];

	for( int i = data.size() - 2; i >= 0; --i )
	{
		data[ i ] ^= data[ i + 1 ];
	}

	return keyPosition + 1;
}

int InverseReverseChain( std::vector< unsigned char >& data, int keyPosition, const std::vector < unsigned char >& key )
{
	keyPosition -= 1;

	for( int i = 0; i < data.size() - 1; ++i )
	{
		data[ i ] ^= data[ i + 1 ];
	}

	data[ data.size() - 1 ] ^= key[ keyPosition ];

	return keyPosition;
}

// Expands the message by using the message as the power and the key as the base and performing an exponentiation
// This allows changes in the message to create changes in size very easily. 
int Expand( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	std::vector< unsigned char > expandedData;

	int keyBitOffset = 0;

	for( int dataByte = 0; dataByte < data.size(); ++dataByte )
	{
		unsigned char power = 0x00;

		for( int bit = 0; bit < 3; ++bit )
		{
			unsigned char keyByte = key[ (keyPosition + keyBitOffset / 8) % key.size() ];
			
			unsigned char bitValue = 0x00;

			bitValue |= ( (0x80 >> keyBitOffset % 8 ) & keyByte );

			bitValue >>= ( 7 - keyBitOffset % 8 );

			bitValue <<= 2 - bit;

			power |= bitValue;

			keyBitOffset++;
		}

		std::vector< unsigned char > expandedByte = ExpandByte( data[ dataByte ], power + 1 );

		for( int j = 0; j < expandedByte.size(); ++j )
		{
			expandedData.push_back( expandedByte[ j ] );
		}
	}

	data = expandedData;

	return ( keyPosition + data.size() * 3 )% key.size();
}

int Unexpand( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	std::vector< unsigned char > collapsedData;

	int datasize = data.size() * 3;

	keyPosition = (keyPosition - datasize);

	while( keyPosition < 0 )
	{
		keyPosition += key.size();
	}

	keyPosition %= key.size();

	int keyBitOffset = 0;

	int dataByte = 0;

	while( dataByte < data.size() )
	{
		unsigned char power = 0x00;

		for( int bit = 0; bit < 3; ++bit )
		{
			unsigned char keyByte = key[ (keyPosition + keyBitOffset / 8) % key.size() ];
			
			unsigned char bitValue = 0x00;

			bitValue |= ( (0x80 >> keyBitOffset % 8 ) & keyByte );

			bitValue >>= ( 7 - keyBitOffset % 8 );

			bitValue <<= 2 - bit;

			power |= bitValue;

			keyBitOffset++;
		}

		uint64_t radicand = 0x0000000000000000;

		for( uint64_t byte = 0; byte <= power; byte++ )
		{
			radicand |= (uint64_t((data[ dataByte + power - byte ])) << ( byte * 8 ));
		}

		dataByte += power + 1;

		collapsedData.push_back( FindNthRoot( radicand, power + 1 ) );
	}

	data = collapsedData;

	return keyPosition;
}

// Use Rijndael sbox to sub bytes
// However, it is going to slide based on the current key byte
// It uses the first nibble of the byte to rotate the box horizontally
// The second nibble rotates it vertically
// Makes the substitution sensitive to the key and to length of message
int SubBytes( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	for( int i = 0; i < data.size(); ++i )
	{
		unsigned char row = data[ i ] / 16;

		unsigned char column = data[ i ] % 16;

		row +=  ( key[ keyPosition ] & 0xF0 );

		column += ( key[ keyPosition ] & 0x0F ) << 4;

		row %= 16;

		column %= 16;

		data[ i ] = sbox[ row * 16 + column ];

		keyPosition = ( keyPosition + 1 ) % key.size();
	}

	return ( keyPosition + 1 ) % key.size();
}

int InverseSubBytes( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	int datasize = data.size() + 1;

	keyPosition = (keyPosition - datasize);

	while( keyPosition < 0 )
	{
		keyPosition += key.size();
	}

	keyPosition %= key.size();

	int tempKeyPosition = keyPosition;

	for( int i = 0; i < data.size(); ++i )
	{
		tempKeyPosition = (tempKeyPosition + 1) % key.size();

		data[ i ] = inverseSbox[ data[ i ] ];

		unsigned char row = data[ i ] / 16;

		unsigned char column = data[ i ] % 16;

		row -= ( key[ tempKeyPosition ] & 0xF0 ) % 16;

		column -= ( key[ tempKeyPosition ] & 0x0F >> 4 ) % 16;

		data[ i ] = row * 16 + column; 
	}

	return keyPosition;
}

std::vector< unsigned char > LoadKey( const std::string& filename )
{
	std::ifstream inputStream( filename, std::ios::binary | std::ios::ate );

	std::vector< unsigned char > bytes( inputStream.tellg() );

	inputStream.seekg( 0 );

	inputStream.read( (char*)&bytes[0], bytes.size() );

	inputStream.close();

	return bytes;
}

void CompressData(std::vector< unsigned char >& data)
{
	std::stringstream input;

	for( unsigned int i = 0; i < data.size(); i++ )
	{
		input << data[i];
	}

    std::stringstream output;
    modelA<int, 16, 14> cmodel;

    compress(input, output, cmodel);

	data.resize( output.str().size() );

	for( unsigned int i = 0; i < data.size(); ++i )
	{
		data[i] = output.str()[i];
	}
}

void DecompressData(std::vector< unsigned char >& data)
{
	std::stringstream input;

	for( unsigned int i = 0; i < data.size(); i++ )
	{
		input << data[i];
	}

    std::stringstream output;
    modelA<int, 16, 14> cmodel;

    decompress(input, output, cmodel);

	data.resize( output.str().size() );

	for( unsigned int i = 0; i < data.size(); ++i )
	{
		data[i] = output.str()[i];
	}
}

void DeteremineInOut( unsigned char keyByte, bool& currentInputLeft,	bool& currentInputEnd, bool& currentOutputLeft, bool& currentOutputEnd, int*& inputBit, int*& outputBit,
					  int& ile, int& ire, int& ilm, int& irm, int& ole, int& ore, int& olm, int& orm )
{
	if( keyByte & 0x08 )
		{
			currentInputLeft = !currentInputLeft;
		}

		if( keyByte & 0x04 )
		{
			currentInputEnd = !currentInputEnd;
		}

		if( keyByte & 0x02 )
		{
			currentOutputLeft = !currentOutputLeft;
		}

		if( keyByte & 0x01 )
		{
			currentOutputEnd = !currentOutputEnd;
		}

		if( currentOutputEnd )
		{
			if( currentOutputLeft )
			{
				if( ole < olm-1 )
				{
					outputBit = &ole;
					ole++;
				}

				else
				{
					outputBit = &ore;
					ore--;
				}
			}

			else
			{
				if( orm < ore-1 )
				{
					outputBit = &ore;
					ore--;
				}

				else
				{
					outputBit = &ole;
					ole++;
				}
			}
		}

		else
		{
			if( currentOutputLeft )
			{
				if( ole < olm-1 )
				{
					outputBit = &olm;
					olm--;
				}

				else
				{
					outputBit = &orm;
					orm++;
				}
			}

			else
			{
				if( orm < ore-1 )
				{
					outputBit = &orm;
					orm++;
				}

				else
				{
					outputBit = &olm;
					olm--;
				}
			}
		}

		if( currentInputEnd )
		{
			if( currentInputLeft )
			{
				if( ile < ilm-1 )
				{
					inputBit = &ile;
					ile++;
				}

				else
				{
					inputBit = &ire;
					ire--;
				}
			}

			else
			{
				if( irm < ire-1 )
				{
					inputBit = &ire;
					ire--;
				}

				else
				{
					inputBit = &ile;
					ile++;
				}
			}
		}

		else
		{
			if( currentInputLeft )
			{
				if( ile < ilm-1 )
				{
					inputBit = &ilm;
					ilm--;
				}

				else
				{
					inputBit = &irm;
					irm++;
				}
			}

			else
			{
				if( irm < ire-1 )
				{
					inputBit = &irm;
					irm++;
				}

				else
				{
					inputBit = &ilm;
					ilm--;
				}
			}
		}
}

std::vector< unsigned char > ShuffleBits( const std::vector< unsigned char >& data, const std::vector< unsigned char >& key, int& keyPosition )
{
	std::vector< unsigned char > shuffledBits( data.size(), 0x00 );

	int ile = -1;
	int ire = (data.size() - 1 ) * 8 + 8;
	int ilm = ire / 2 + 1;
	int irm = ilm - 1;
	int ole = ile;
	int ore = ire;
	int olm = ilm;
	int orm = irm;

	int* inputBit = &ile;
	int* outputBit = &ole;

	bool currentInputLeft = 1;
	bool currentInputEnd = 1;

	bool currentOutputLeft = 1;
	bool currentOutputEnd = 1;

	std::vector< int > inputBits;
	std::vector< int > outputBits;

	for( int keyBit = 0; keyBit < data.size() * 4 * 8; keyBit += 4 )
	{
		unsigned char keyByte = key[ ( ( keyPosition + keyBit )  / 8 ) % key.size() ];

		keyByte >>= ( 4 * ( ( keyBit % 8 ) / 4 ) );

		DeteremineInOut(keyByte, currentInputLeft, currentInputEnd, currentOutputLeft, currentOutputEnd, inputBit, outputBit,
					    ile, ire, ilm, irm, ole, ore, olm, orm );

		unsigned char inputByte = data[ *inputBit / 8 ];
		
		unsigned char bitValue = inputByte & (0x80 >> (*inputBit % 8 ));
		
		bitValue >>= ( 7 - ( *inputBit % 8 ) );

		bitValue <<= ( 7 - ( *outputBit % 8 ) );

		shuffledBits[ *outputBit / 8 ] |= bitValue;
	}

	keyPosition = (keyPosition + data.size() * 2 ) % key.size(); 

	return shuffledBits;
}

std::vector< unsigned char > UnshuffleBits( const std::vector< unsigned char >& data, const std::vector< unsigned char >& key, int& keyPosition )
{
	std::vector< unsigned char > shuffledBits( data.size(), 0x00 );

	int datasize = data.size() * 2;
	int keysize = key.size();


	keyPosition = (keyPosition - datasize);

	while( keyPosition < 0 )
	{
		keyPosition += key.size();
	}

	keyPosition %= key.size();

	int ile = -1;
	int ire = (data.size() - 1 ) * 8 + 8;
	int ilm = ire / 2 + 1;
	int irm = ilm - 1;
	int ole = ile;
	int ore = ire;
	int olm = ilm;
	int orm = irm;

	int* inputBit = &ile;
	int* outputBit = &ole;

	bool currentInputLeft = 1;
	bool currentInputEnd = 1;

	bool currentOutputLeft = 1;
	bool currentOutputEnd = 1;

	std::vector< int > inputBits;
	std::vector< int > outputBits;

	for( int keyBit = 0; keyBit < data.size() * 4 * 8; keyBit += 4 )
	{
		unsigned char keyByte = key[ ( ( keyPosition + keyBit )  / 8 ) % key.size() ];

		keyByte >>= ( 4 * ( ( keyBit % 8 ) / 4 ) );

		DeteremineInOut(keyByte, currentInputLeft, currentInputEnd, currentOutputLeft, currentOutputEnd, inputBit, outputBit,
					    ile, ire, ilm, irm, ole, ore, olm, orm );

		unsigned char inputByte = data[ *outputBit / 8 ];
		
		unsigned char bitValue = inputByte & (0x80 >> (*outputBit % 8 ));
		
		bitValue >>= ( 7 - ( *outputBit % 8 ) );

		bitValue <<= ( 7 - ( *inputBit % 8 ) );

		shuffledBits[ *inputBit / 8 ] |= bitValue;
	}

	return shuffledBits;
}


#endif