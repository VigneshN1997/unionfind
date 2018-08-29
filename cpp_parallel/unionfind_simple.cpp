#include "unionfind_simple.h"

void sendMessage(vector<long int> queryForward, int toProcess,unordered_map<int,int>* processQueryNumMappingSend)
{
    MPI_Request request;
    MPI_Isend(&queryForward[0],queryForward.size(),MPI_LONG,toProcess,(*processQueryNumMappingSend)[toProcess],MPI_COMM_WORLD,&request);
    
    (*processQueryNumMappingSend)[toProcess] += 1; // check this update
    if((*processQueryNumMappingSend)[toProcess] == INT_MAX)
    {
        (*processQueryNumMappingSend)[toProcess] = 0;
    }
}

void processReceivedQuery(vector<long int> queryRecv, unordered_map<long int,bool>* replyRequired,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend, MPI_Status status, long int* numQueriesCompleted)
{
    if(queryRecv[0] == 1) {
        returnStruct* retVal = unify(queryRecv[3],queryRecv[4],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            vector<long int> replyMsg = createNewMessage(2,queryRecv[1],-1,-1,-1);
            printf("Union of %ld and %ld done by process %d\n",queryRecv[3],queryRecv[4],processRank);
            sendMessage(replyMsg,queryRecv[2],processQueryNumMappingSend);
            printf("Sent reply to process %d for union of (%ld,%ld)\n",(int)queryRecv[2],queryRecv[3],queryRecv[4]);
        }
        else
        {
            vector<long int> queryForward = createNewMessage(1,queryRecv[1],queryRecv[2],retVal->query->newQueryX,retVal->query->newQueryY);
            sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[3],queryRecv[4],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }
    }
    else if(queryRecv[0] == 2) {
        // printf("Received reply for queryNumber %ld by process %d from process %d\n",queryRecv[3],processRank,status.MPI_SOURCE);
        unordered_map<long int,bool>::iterator map_itr = (*replyRequired).find(queryRecv[1]);
        (*replyRequired).erase(map_itr);
    }
    else if(queryRecv[0] == 3) {
        *numQueriesCompleted += queryRecv[1];
    }
}

// message type: 1-query forward, 2-reply, 3-processing of a process finished

vector<long int> createNewMessage(long int messageType, long int queryNum, int processRank, long int newQueryX, long int newQueryY)
{
    vector<long int> message;
    message.clear();
    message.push_back(messageType);
    message.push_back(queryNum);
    message.push_back((long int)processRank);
    message.push_back(newQueryX);
    message.push_back(newQueryY);
    message.resize(5);
    return message;
}

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries)
{
    long int* numQueriesCompleted = (long int*)malloc(sizeof(long int));
    *numQueriesCompleted = 0;
    unordered_map<int, int>* processQueryNumMappingSend = new unordered_map<int,int>(); // mapping from process number to tag
    unordered_map<long int, bool>* replyRequired = new unordered_map<long int, bool>(); // mapping from query number to true
    MPI_Request request; // check this
    MPI_Status status;
    int flag;
    long int myNumQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int startIndex = (processRank - 1) * numPointsPerProcess;
    
    for(int j = 1; j < num_processes; j++)
    {
        if(j == processRank)
        {
            continue;
        }
        (*processQueryNumMappingSend)[j] = 0;
    }

    // for(int i = 0; i < numQueries; i++)
    // {
    //     printf("%d: (%d,%d)\n",processRank,(int)queriesProcessX[i],(int)queriesProcessY[i]);
    // }

    for(i = 0; i < myNumQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        returnStruct* retVal = unify(x,y,unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            printf("Union of %ld and %ld done by process %d(no forwarding)\n",x,y,processRank);
            continue;
        }

        long int queryNumSend;
        queryNumSend = i;
        (*replyRequired)[i] = true;

        vector<long int> queryForward = createNewMessage(1,queryNumSend,processRank,retVal->query->newQueryX,retVal->query->newQueryY);
        sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
        printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = 0;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(5);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }
    if((*replyRequired).size() > 0)
    {
        while((*replyRequired).size() > 0)
        {
            // printf("waiting for reply %d\n",processRank);
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if(flag)
            {
                flag = 0;
                int src = status.MPI_SOURCE;
                int tag = status.MPI_TAG;
                vector<long int> queryRecv;
                queryRecv.resize(5);
                MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
                processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
            }
        }
    }
    *numQueriesCompleted += myNumQueries;
    printf("process:%d has finished its queries.\n",processRank);
    vector<long int> finishedMsg = createNewMessage(3,myNumQueries,-1,-1,-1);
    for(int j = 1; j < num_processes; j++)
    {
        if(j == processRank)
        {
            continue;
        }
        sendMessage(finishedMsg,j,processQueryNumMappingSend);
    }
    while(true)
    {
        // printf("aaaa%d\n",processRank);
        if(*numQueriesCompleted == totalNumQueries) {
            break;
        }
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if(flag)
        {
            flag = 0;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(5);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }
    printf("process:%d exiting.\n",processRank);
}

returnStruct* unify(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y)
{
    long int root_y = y;
    long int its_parent = unionfindDs[y - startIndex];
    returnStruct* retVal = (returnStruct*)malloc(sizeof(returnStruct));
    retVal->query = NULL;
    while(root_y < its_parent && pointIdMapping[its_parent] == process_of_y)
    {
        root_y = its_parent;
        its_parent = unionfindDs[root_y - startIndex];
    }

    if(root_y == its_parent)
    {
        if(root_y < x)
        {
            unionfindDs[root_y - startIndex] = x;
        }
        else if(root_y > x)
        {
            retVal->query = createQueryFwd(root_y, x, pointIdMapping[x],-1);
        }
    }
    else
    {
        if(its_parent < x)
        {
            retVal->query = createQueryFwd(x,its_parent,pointIdMapping[its_parent],-1);
        }
        else
        {
            retVal->query = createQueryFwd(its_parent,x,pointIdMapping[x],-1);
        }
    }
    return retVal;
}

// finalParent will be used in PathCompression algorithm
sendQuery* createQueryFwd(long int newQueryX, long int newQueryY, int toProcess, long int finalParent)
{
    sendQuery* query = (sendQuery*)malloc(sizeof(sendQuery));
    query->newQueryX = newQueryX;
    query->newQueryY = newQueryY;
    query->toProcess = toProcess;
    query->finalParent = finalParent;
    return query;
}