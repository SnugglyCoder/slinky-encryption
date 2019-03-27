#include <iostream>
#include <bitset> 
#include <cmath>
#include <vector>
#include <sstream>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

using namespace std;

vector< unsigned char > SlinkyDecryption( vector< unsigned char >& data, const vector< unsigned char >& key );
vector< unsigned char > SlinkyEncryption( vector< unsigned char >& data, const vector< unsigned char >& key );
int getMinimumPenalty(string x, string y, int pxy, int pgap); 

int rounds;

const int misMatchPenalty = 1; 
        
const int gapPenalty = 2;

int main( int argc, char* argv[] )
{
    
//////////////////////////////////////////
//           Encryption/Decryption      //
//////////////////////////////////////////
    if( argc < 4 )
    {
        cout << "Usage: ./slinky <key> <number of rounds> <testfiles> [testfiles]... " << endl;
        exit(1);
    }

    rounds = atoi(argv[2]);

    InitTable();

    vector< unsigned char > key = LoadKey( string( argv[ 1 ] ) );

    for(int file = 3; file < argc; file++){

        vector< unsigned char > fileData = LoadKey( string( argv[ file ] ) );

        vector< unsigned char > data = fileData;

        SlinkyEncryption( data, key );

        ofstream resultFile( string( argv[file]) + "-resultfile.csv");
        
        stringstream control;
        
        for( unsigned int i = 0; i < data.size(); i++ )
        {
            control << bitset<8>(data[i]);
        }
        
        resultFile << "controlSize," << control.str().size() << endl;
        
        for( int i = 0; i < fileData.size(); i++ )
        {
            vector< unsigned char> dataCopy(fileData);

            dataCopy[ i ] ^= 0x80;

            SlinkyEncryption( dataCopy, key );

            stringstream bitString;

            for( unsigned int i = 0; i < dataCopy.size(); i++ )
            {
                bitString << bitset<8>(dataCopy[i]);
            }
            
            resultFile << bitString.str().size() << "," << getMinimumPenalty(control.str(), bitString.str(), misMatchPenalty, gapPenalty) << "\n";
        }
    }
    return 0;
}

//////////////////////////////////////////
//           Encryption/Decryption      //
//////////////////////////////////////////
int keyPosition = 0;

vector< unsigned char > SlinkyEncryption( vector< unsigned char >& data, const vector< unsigned char >& key )
{
    for( unsigned int i = 0; i < rounds; ++i )
    {
       // cout << "Round " << i + 1 << endl;

        keyPosition = ForwardChain( data, keyPosition, key );

        keyPosition = SubBytes( data, keyPosition, key );

        keyPosition = ReverseChain( data, keyPosition, key );

        keyPosition = AddKey( data, keyPosition, key );

        keyPosition = Expand( data, keyPosition, key );

        CompressData( data );

        data = ShuffleBits( data, key, keyPosition );
    }

	return data;
}

vector< unsigned char > SlinkyDecryption( vector< unsigned char >& data, const vector< unsigned char >& key )
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

////////////////////////////////////////
//         Sequence alignment         //
////////////////////////////////////////

int getMinimumPenalty(string x, string y, int pxy, int pgap) 
{ 
    int i, j; // intialising variables 
      
    int m = x.length(); // length of gene1 
    int n = y.length(); // length of gene2 
      
    // table for storing optimal substructure answers 
    vector<vector<int>> dp(2, vector<int>(n+1,0));
    
    for( int k = 0; k < n+1; k++)
    {
        dp[0][k] = k * pgap;
    }
    
    // calcuting the minimum penalty 
    for (i = 1; i < m+1; i++) 
    { 
        dp[i%2][0] = i * pgap;

        for (j = 1; j < n+1; j++) 
        { 
            if (x[i-1] == y[j-1]) 
            { 
                dp[i%2][j] = dp[(i - 1)%2][j - 1]; 
            } 
            else
            { 
                dp[i%2][j] = min({dp[(i - 1)%2][j - 1] + pxy ,  
                                dp[(i - 1)%2][j] + pgap    ,  
                                dp[i%2][j - 1] + pgap    }); 
            }
        }
    } 

    return dp[m%2][n];
} 