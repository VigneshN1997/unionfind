#include "unionfind_simple.cpp"

vector<long int>* createNewMessagePathCompression(long int messageType, long int queryNum, long int newQueryX, long int newQueryY,long int finalParent);
void processQueriesPathCompression(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int> queryNums,long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple, FILE* fp);
void processReceivedQueryPathCompression(vector<long int> queryRecv, unordered_map<long int, vector<long int>* >* replyRequired,unordered_map<long int, bool>* replyRequiredSelf,unordered_map<long int, vector<int>* >* replyToBeSent,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend,MPI_Status status, long int* numQueriesCompleted, long int* numMessages);
returnStruct* unifyPathCompression(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex, int process_of_y);
void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex,vector<int> pointIdMapping ,long int* unionfindDs);
