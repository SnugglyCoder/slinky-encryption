#include <iostream>
#include <bits/stdc++.h> 
#include <cmath>
#include <vector>
#include <sstream>
#include "../headers/power-table.hpp"
#include "../headers/slinky-primitives.hpp"

using namespace std;

vector< unsigned char > SlinkyDecryption( vector< unsigned char >& data, const vector< unsigned char >& key );
vector< unsigned char > SlinkyEncryption( vector< unsigned char >& data, const vector< unsigned char >& key );
int getMinimumPenalty(string x, string y, int pxy, int pgap); 
string LoadKeySA( const string& filename );
struct ComparisonData
{
    int string1Size;
    int string2Size;
    int matchCost;

    ComparisonData( int size1, int size2, int cost ) : string1Size(size1), string2Size(size2), matchCost(cost){}
    ComparisonData(){}
};

int rounds;

int main( int argc, char* argv[] )
{
    
//////////////////////////////////////////
//           Encryption/Decryption      //
//////////////////////////////////////////
    if( argc != 6 )
    {
        cout << "Usage: ./slinky <key> <input> <bit-output> <number of rounds> <testfiles> [testfiles]... " << endl;
        exit(1);
    }

    rounds = atoi(argv[5]);

    InitTable();

    vector< unsigned char > key = LoadKey( string( argv[ 1 ] ) );

    vector< unsigned char > fileData = LoadKey( string( argv[ 2 ] ) );

    vector< unsigned char > data;

    for( unsigned int i = 0; i < 100; i++ )
    {
        data.push_back( fileData[ i ] );
    }

    vector< unsigned char > dataCopy( data );

   // cout << "Encrypting" << endl;

    SlinkyEncryption( data, key );

    vector< unsigned char > encryptedData( data );

    //cout << "Decrypting" << endl;

    //SlinkyDecryption( data, key );

    //cout << "Comparing" << endl;

    //for( int i = 0; i < data.size(); i++ )
    {
        //if( data[i] != dataCopy[i])
        {
            //cout << "Process failed" << endl;
            //exit(1);
        }
    }

//    ofstream fileOut( string(argv[3]) + string("-control.txt") );
    ofstream fileOut( string("control.txt") );

    for( unsigned int i = 0; i < encryptedData.size(); i++ )
    {
        fileOut << bitset<8>(encryptedData[i]);
    }
  
    fileOut.close();

    for( int i = 0; i < dataCopy.size(); i++ )
    {
        data = dataCopy;

        data[ i ] ^= 0x80;

        SlinkyEncryption( data, key );

        stringstream filename;

        filename << argv[3] << "-bit" << i  << ".txt";

        ofstream fileOut( filename.str() );

        for( unsigned int i = 0; i < data.size(); i++ )
        {
            fileOut << bitset<8>(data[i]);
        }
    
        fileOut.close();
    }

////////////////////////////////////////
//         Sequence alignment         //
////////////////////////////////////////

    int misMatchPenalty = 1; 
    int gapPenalty = 2; 
  
    // if ( argc < 4 ) 
    // {
    //     cout << "Usage: ./slinky <key> <input> <bit-output> <number of rounds> <testfiles> [testfiles]..." << endl;
    //     exit(1);
    // }

    vector< string > bitStrings(argc-6);

    cout << argc << endl;
    cout << bitStrings.size() << endl;
    
    cout << "Loading control" << endl;

    string control = LoadKeySA( "control.txt" );

    cout << "Loading tests" << endl;

    for(int i = 0; i < bitStrings.size(); i++ )
    {
        bitStrings[i] = LoadKeySA(argv[i+6]);
    }

    cout << "Starting comparision" << endl;

    ofstream resultFile( "result.csv" );

    resultFile << "BitString1,BitString2,Minimum Cost Difference" << endl;

    for( int i = 0; i < bitStrings.size(); i++ )
    {   
        resultFile << control.size() << "," << bitStrings[i].size() << "," << getMinimumPenalty(control, bitStrings[i], misMatchPenalty, gapPenalty) << "\n";
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

       // cout << data.size() << endl;

        CompressData( data );

       // cout << data.size() << endl;

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
    vector<vector<int>> dp(m+1, vector<int>(n+1,0));
  
    // calcuting the minimum penalty 
    for (i = 1; i <= m; i++) 
    { 
        for (j = 1; j <= n; j++) 
        { 
            if (x[i - 1] == y[j - 1]) 
            { 
                dp[i][j] = dp[i - 1][j - 1]; 
            } 
            else
            { 
                dp[i][j] = min({dp[i - 1][j - 1] + pxy ,  
                                dp[i - 1][j] + pgap    ,  
                                dp[i][j - 1] + pgap    }); 
            }
        }
    } 

    return dp[m][n];
} 

string LoadKeySA( const string& filename )
{
	ifstream inputStream( filename, ios::binary | ios::ate );

	string bytes;
	
	bytes.resize( inputStream.tellg() );

	inputStream.seekg( 0 );

	inputStream.read( (char*)&bytes[0], bytes.size() );

	inputStream.close();

	return bytes;
}