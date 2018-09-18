#include "preprocess.cpp"
#include <mpi.h>

void sendMessage(vector<long int>* queryForward, int toProcess,unordered_map<int,int>* processQueryNumMappingSend, MPI_Request* request);
void processReceivedQuery(vector<long int>* queryRecv, unordered_map<long int,bool>* replyRequired,long int* unionfindDs,vector<int> pointIdMapping,
long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend, MPI_Status* statusRecv, long int* numQueriesCompleted, 
long int* numMessages, vector<long int>* messagePRQ, MPI_Request* requestPRQ,MPI_Status* statusSend,bool* firstCallPRQ, returnStruct* retVal, sendQuery* sq);

void createNewMessage(bool firstCall, vector<long int>* vecToFill, MPI_Request* request, MPI_Status* status, long int messageType, long int queryNum, long int processRank, long int newQueryX, long int newQueryY);
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple);
void unify(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y, returnStruct* retVal, sendQuery* sq);
void createQueryFwd(long int newQueryX, long int newQueryY, int toProcess, long int finalParent, sendQuery* query);
