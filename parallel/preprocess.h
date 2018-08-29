#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "def.h"

long int* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping);
unordered_map<string, int> getConfig(string configFileName);