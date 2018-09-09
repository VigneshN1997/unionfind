#include "unionfind_pathc.cpp"

#define MAX_UPDATES 100	

vector<pair<vector<long int>*, vector<long int>* >* >* initializeUpdatesStructure(int num_processes);
void processQueriesDeferredUpdates(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int> queryNums, long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple);
vector<long int>* createNewMessageModified(long int messageType, long int queryNum, long int newQueryX, long int newQueryY, long int numUpdates, long int originalProcessRank);
void processUpdates(vector<long int> queryNumsVec, vector<long int> finalParentsVec, long int numUpdates, unordered_map<long int, vector<int>* >* replyToBeSent, unordered_map<long int, vector<long int>* >* replyRequired, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank);
void sendUpdatesWithQueryForwarding(returnStruct* retVal, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone, long int queryNum, long int originalProcessRank, unordered_map<int,int>* processQueryNumMappingSend);
void processReceivedQueryModified(vector<long int> queryRecv, unordered_map<long int, vector<long int>* >* replyRequired,unordered_map<long int, bool>* replyRequiredSelf,unordered_map<long int, vector<int>* >* replyToBeSent, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone, long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend,MPI_Status status, long int* numQueriesCompleted);
