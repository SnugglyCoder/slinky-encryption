// taken from geeks for geeks
// CPP program to implement sequence alignment 
// problem. 
#include <bits/stdc++.h> 
#include <vector>
  
using namespace std; 
  
// function to find out the minimum penalty 
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

string LoadKey( const std::string& filename )
{
	ifstream inputStream( filename, std::ios::binary | std::ios::ate );

	string bytes;
	
	bytes.resize( inputStream.tellg() );

	inputStream.seekg( 0 );

	inputStream.read( (char*)&bytes[0], bytes.size() );

	inputStream.close();

	return bytes;
}

struct ComparisonData
{
    int string1Size;
    int string2Size;
    int matchCost;

    ComparisonData( int size1, int size2, int cost ) : string1Size(size1), string2Size(size2), matchCost(cost){}
    ComparisonData(){}
};



// Driver code 
int main( int argc, char* argv[] ){ 

    int misMatchPenalty = 1; 
    int gapPenalty = 2; 
  
    if ( argc < 4 ) 
    {
        std::cout << "usage: ./sequence-alignment <result-file> <control> <testfile> [testfiles]...";
        exit(1);
    }

    vector< string > bitStrings(argc-3);

    std::cout << "Loading control" << std::endl;

    string control = LoadKey( argv[2] );

    std::cout << "Loading tests" << std::endl;

    for(int i = 0; i < bitStrings.size(); i++ )
    {
        bitStrings[i] = LoadKey(argv[i+3]);
    }

    std::cout << "Starting comparision" << std::endl;

    std::ofstream resultFile( argv[1] );

    resultFile << "BitString1,BitString2,Minimum Cost Difference" << std::endl;

    for( int i = 0; i < bitStrings.size(); i++ )
    {   
        resultFile << control.size() << "," << bitStrings[i].size() << "," << getMinimumPenalty(control, bitStrings[i], misMatchPenalty, gapPenalty) << "\n";
    }

  

    return 0; 
} 