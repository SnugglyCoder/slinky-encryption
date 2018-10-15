#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <vector>
#include <algorithm>



void HuffmanCompress( std::vector< unsigned char >& data )
{
	int counts[256];

	for(unsigned int i = 0; i < 256; ++i )
	{
		counts[i]=0;
	}

	for( unsigned int i = 0; i < data.size(); ++i )
	{
		counts[ data[ i ] ]++;
	}

	std::sort( data.begin(), data.end() );
}

#endif