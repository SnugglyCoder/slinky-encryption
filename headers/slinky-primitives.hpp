#ifndef SLINKY_PRIMITIVES_HPP
#define SLINKY_PRIMITIVES_HPP

#include <vector>
#include <fstream>
#include <iostream>

#include "sbox.hpp"
#include "power-table.hpp"

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

int Expand( std::vector< unsigned char >& data, int keyPosition, const std::vector< unsigned char >& key )
{
	std::vector< unsigned char > expandedData;

	int keyBitOffset = 0;

	for( unsigned int i = 0; i < data.size(); ++i )
	{
		unsigned char power = 0x00;

		for( int bit = 0; bit < 3; ++bit )
		{
			unsigned char keyByte = key[ ( keyPosition + keyBitOffset / 8 ) % key.size() ];
			
			keyByte &= 1 >> ( keyBitOffset % 8 );

			power |= keyByte;

			keyBitOffset++;
		}

		std::vector< unsigned char > expandedByte = ExpandByte( data[ i ], power + 1 );

		for( int j = 0; j < expandedByte.size(); ++j )
		{
			expandedData.push_back( expandedByte[ j ] );
		}
	}

	data = expandedData;

	return ( ( keyPosition + keyBitOffset / 8 ) + 1 )% key.size();
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

#endif