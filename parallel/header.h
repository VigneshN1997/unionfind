#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <cmath>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <climits>
using namespace std;

struct sendQeury
{
	long int newQueryX;
	long int newQueryY;
	int toProcess;
	int fromProcess;
};

struct returnStruct
{
	struct sendQuery* query;
	bool unionDone;
};

typedef struct sendQuery sendQuery;
typedef struct returnStruct returnStruct;
vector<long int>* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping);
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess);
