#include "unionfind_mod.cpp"

void generateQueryFiles(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
void printUnionfindToFileVector(UnionFind_mod* uf,long int numQueries);

vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
