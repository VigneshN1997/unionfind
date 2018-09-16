#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <sys/stat.h>
#include <climits>
using namespace std;

bool exists_test(string file_name);
void generateQueryFiles(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
void generateRandomQueries(long int numQueries, long int maxNum,int numBuckets);

