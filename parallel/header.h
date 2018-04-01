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

struct sendQuery
{
	long int newQueryX;
	long int newQueryY;
	long int finalParent; // used in path compression algorithm
	int toProcess;
	// int fromProcess;
};

struct returnStruct
{
	struct sendQuery* query;
	bool unionDone;
};

typedef struct sendQuery sendQuery;
typedef struct returnStruct returnStruct;

vector<long int>* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping);
void sendMessage(vector<long int> queryForward, int toProcess,map<int,int>* processQueryNumMappingSend);
void processReceivedQuery(vector<long int> queryRecv,vector<bool>* finished, map<long int,bool>* replyRequired,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,map<int,int>* processQueryNumMappingSend,long int* queryNum,MPI_Status status);
vector<long int> createNewMessage(int processDone,long int queryNum,int processRank, long int isReply, long int newQueryX, long int newQueryY);
bool convertToBool(long int num);
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes);
returnStruct* unify(long int x, long int y, vector<long int>* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y);
sendQuery* createQueryFwd(long int newQueryX, long int newQueryY, int toProcess, long int finalParent);