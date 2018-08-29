#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "header.h"

long int* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping);
unordered_map<string, long int> getConfig(string configFileName);
void printUnionFindDs(int processRank, long int* unionFindDs, long int numPointsPerProcess);