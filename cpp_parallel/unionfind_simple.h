#include "preprocess.cpp"
#include <mpi.h>

void sendMessage(vector<long int>* queryForward, int toProcess,unordered_map<int,int>* processQueryNumMappingSend);
void processReceivedQuery(FILE* fp,vector<long int> queryRecv, unordered_map<long int,bool>* replyRequired,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend, MPI_Status status, long int* numQueriesCompleted, long int* numMessages);
vector<long int>* createNewMessage(long int messageType, long int queryNum, long int processRank, long int newQueryX, long int newQueryY);
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple, FILE* fp);
returnStruct* unify(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y);
sendQuery* createQueryFwd(long int newQueryX, long int newQueryY, int toProcess, long int finalParent);
