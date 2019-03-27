// taken from geeks for geeks
// CPP program to implement sequence alignment 
// problem. 

#include <string>
#include <iostream>
#include <vector>
  
using namespace std; 
  
// function to find out the minimum penalty 
int getMinimumPenalty(string x, string y, int pxy, int pgap) 
{ 
    int i, j; // intialising variables 
      
    int m = x.length(); // length of gene1 
    int n = y.length(); // length of gene2 
      
    // table for storing optimal substructure answers 
    vector<vector<int>> dp(2, vector<int>(n+1,0));
   
    /*
    for( int k = 0; k < m + 1; k++) 
    {
        dp[k][0] = k * pgap;
    }
    */
    
    
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

// Driver code 
int main( int argc, char* argv[] ){ 

    int misMatchPenalty = 1; 
    int gapPenalty = 2; 
  
    const string test1 = "11111";
    const string test2 = "00000";
    const string test3 = "1111111111";
    const string test4 = "10101";

    bool allPassed = true;

    if( 0 != getMinimumPenalty(test1, test1, misMatchPenalty, gapPenalty) )
    {
        cout << "Failed identical test" << endl;

        allPassed = false;
    }

    if( test1.size() != getMinimumPenalty(test1, test2, misMatchPenalty, gapPenalty ) )
    {
        cout << "Failed opposite test" << endl;

        allPassed = false;
    }

    if( test1.size() != getMinimumPenalty(test2, test1, misMatchPenalty, gapPenalty ) )
    {
        cout << "Failed reversed opposite test" << endl;

        allPassed = false;
    }

    if( 2 != getMinimumPenalty(test1, test4, misMatchPenalty, gapPenalty) )
    {
        cout << "Failed alternating test" << endl;

        allPassed = false;
    }

    if( 2 != getMinimumPenalty(test4, test1, misMatchPenalty, gapPenalty) )
    {
        cout << "Failed reversed alternating test" << endl;

        allPassed = false;
    }

    if( 10 != getMinimumPenalty(test1, test3, misMatchPenalty, gapPenalty) )
    {
        cout << "Failed gap test " << getMinimumPenalty(test1, test3, misMatchPenalty, gapPenalty) << endl;

        allPassed = false;
    }

    if( 10 != getMinimumPenalty(test3, test1, misMatchPenalty, gapPenalty) )
    {
        cout << "Failed gap test " << endl;

        allPassed = false;
    }

    if( allPassed ) 
    {
        cout << "All tests passed" << endl;
    }

    return 0; 
} 