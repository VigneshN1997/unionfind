#include "unionfind_mod.cpp"


bool exists_test(string file_name);
void generateQueryFiles(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
void generateRandomQueries(long int numQueries, long int maxNum,int numBuckets);


void printUnionfindToFileVector(UnionFind_mod* uf,long int numQueries);
void printUnionfindToFileMap(UnionFind* uf,long int numQueries);

vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
