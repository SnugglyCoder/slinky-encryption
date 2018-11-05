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

// XOR the last byte of data with current key byte
// Chain this back through to the beginning of the message
int DataChain( std::vector< unsigned char >& data, const int keyPosition, const std::vector< unsigned char >& key )
{
	data[ data.size() - 1 ] ^= key[ keyPosition ];

	for( int i = data.size() - 2; i >= 0; --i )
	{
		data[ i ] ^= data[ i + 1 ];
	}

	return keyPosition + 1;
}

int InverseDataChain( std::vector< unsigned char >& data, int keyPosition, const std::vector < unsigned char >& key )
{
	keyPosition -= 1;

	for( int i = 0; i < data.size() - 1; ++i )
	{
		data[ i ] ^= data[ i + 1 ];
	}

	data[ data.size() - 1 ] ^= key[ keyPosition ];

	return keyPosition;
}

// Add key then chain the data blocks
int Whiten( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	keyPosition = AddKey( data, keyPosition, key );

	return DataChain( data, keyPosition, key );
}

int Blacken( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char > & key )
{
	keyPosition = InverseDataChain( data, keyPosition, key );
	
	return AddKey( data, ( data.size() - keyPosition ) % key.size(), key );
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

			bitValue |= ( (0x80 >> keyBitOffset % 8 ) & dataByte );

			bitValue >>= ( 7 - keyBitOffset % 8 );

			bitValue <<= 2 - bit;

			power |= bitValue;
		
			keyBitOffset++;
		}

		std::vector< unsigned char > expandedByte = ExpandByte( key[ keyPosition ], power + 1 );

		keyPosition = ( keyPosition + 1 ) % key.size();

		for( int j = 0; j < expandedByte.size(); ++j )
		{
			expandedData.push_back( expandedByte[ j ] );
		}
	}

	data = expandedData;

	return ( keyPosition + 1 )% key.size();
}

int Shrink( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	std::vector< unsigned char > shrunkData;

	int keyBitOffset = 0;

	for( int i = 0; i < data.size(); ++i )
	{
		unsigned char keyByte = key[ ( keyPosition + keyBitOffset / 8 ) % key.size() ];
	}
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
	for( int i = 0; i < data.size(); ++i )
	{
		keyPosition = (keyPosition - 1) % key.size();

		data[ i ] = inverseSbox[ data[ i ] ];

		unsigned char row = data[ i ] / 16;

		unsigned char column = data[ i ] % 16;

		row -= ( key[ keyPosition ] & 0xF0 ) % 16;

		column -= ( key[ keyPosition ] & 0x0F >> 4 ) % 16;

		data[ i ] = row * 16 + column; 
	}

	return ( keyPosition - 1 ) % key.size();
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

		//std::cout << ( ( keyPosition + keyBit )  / 8 ) % key.size() << std::endl;

		keyByte >>= ( 4 * ( ( keyBit % 8 ) / 4 ) );

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

		unsigned char inputByte = data[ *outputBit / 8 ];
		
		unsigned char bitValue = inputByte & (0x80 >> (*outputBit % 8 ));
		
		bitValue >>= ( 7 - ( *outputBit % 8 ) );

		bitValue <<= ( 7 - ( *inputBit % 8 ) );

		shuffledBits[ *inputBit / 8 ] |= bitValue;
	}

	return shuffledBits;
}

#endif