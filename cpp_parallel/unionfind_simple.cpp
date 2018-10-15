#include "unionfind_simple.h"

void sendMessage(vector<long int>* queryForward, int toProcess,unordered_map<int,int>* processQueryNumMappingSend, MPI_Request* request)
{
    MPI_Isend(&((*queryForward)[0]),(*queryForward).size(),MPI_LONG,toProcess,(*processQueryNumMappingSend)[toProcess],MPI_COMM_WORLD,request);
    
    (*processQueryNumMappingSend)[toProcess] += 1; // check this update
    if((*processQueryNumMappingSend)[toProcess] == INT_MAX)
    {
        (*processQueryNumMappingSend)[toProcess] = 0;
    }
}

void processReceivedQuery(vector<long int>* queryRecv, unordered_map<long int,bool>* replyRequired,long int* unionfindDs,vector<int> pointIdMapping,
long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend, MPI_Status* statusRecv, long int* numQueriesCompleted, 
long int* numMessages, vector<long int>* messagePRQ, MPI_Request* requestPRQ,MPI_Status* statusSend,bool* firstCallPRQ, returnStruct* retVal, sendQuery* sq)
{
    if((*queryRecv)[0] == 1) {
        unify((*queryRecv)[3],(*queryRecv)[4],unionfindDs,pointIdMapping,startIndex,processRank, retVal, sq);
        if(retVal->query == NULL)
        {
            createNewMessage(*firstCallPRQ,messagePRQ, requestPRQ, statusSend, 2,(*queryRecv)[1],-1,-1,-1);
            *firstCallPRQ = false;
            // fprintf(fp,"Union of %ld and %ld done by process %d\n",queryRecv[3],queryRecv[4],processRank);
            *numMessages += 1;
            sendMessage(messagePRQ,(*queryRecv)[2],processQueryNumMappingSend, requestPRQ);
            // fprintf(fp,"Sent reply to process %d for union of (%ld,%ld)\n",(int)queryRecv[2],queryRecv[3],queryRecv[4]);
        }
        else
        {
            createNewMessage(*firstCallPRQ, messagePRQ, requestPRQ, statusSend,1,(*queryRecv)[1],(*queryRecv)[2],retVal->query->newQueryX,retVal->query->newQueryY);
            *firstCallPRQ = false;
            *numMessages += 1;
            sendMessage(messagePRQ,retVal->query->toProcess,processQueryNumMappingSend, requestPRQ);
            // fprintf(fp,"Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[3],queryRecv[4],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }
    }
    else if((*queryRecv)[0] == 2) {
        // fprintf(fp,"Received reply for queryNumber %ld by process %d from process %d\n",queryRecv[1],processRank,status.MPI_SOURCE);
        unordered_map<long int,bool>::iterator map_itr = (*replyRequired).find((*queryRecv)[1]);
        if(map_itr != (*replyRequired).end()) {
            (*replyRequired).erase(map_itr);
        }
    }
    else if((*queryRecv)[0] == 3) {
        *numQueriesCompleted += (*queryRecv)[1];
    }
}

// message type: 1-query forward, 2-reply, 3-processing of a process finished

void createNewMessage(bool firstCall, vector<long int>* vecToFill, MPI_Request* request, MPI_Status* status, long int messageType, long int queryNum, long int processRank, long int newQueryX, long int newQueryY)
{
    // printf("createNewMessage : %ld\n",queryNum);
    if(!firstCall) {
        // printf("wait qnum:%ld\n", queryNum);
        if(*request != MPI_REQUEST_NULL) {
            MPI_Wait(request, status);
        }
        // printf("done wait qnum:%ld size:%ld\n", queryNum,sizeof(*request));
    }
    (*vecToFill)[0] = messageType;
    (*vecToFill)[1] = queryNum;
    (*vecToFill)[2] = processRank;
    (*vecToFill)[3] = newQueryX;
    (*vecToFill)[4] = newQueryY;
}

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple)
{
    int messageSize = 5;
    long int* numQueriesCompleted = (long int*)malloc(sizeof(long int));
    *numQueriesCompleted = 0;
    unordered_map<int, int>* processQueryNumMappingSend = new unordered_map<int,int>(); // mapping from process number to tag
    unordered_map<long int, bool>* replyRequired = new unordered_map<long int, bool>(); // mapping from query number to true
    MPI_Status status;
    int flag;
    long int myNumQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int startIndex = (processRank - 1) * numPointsPerProcess;
    
    vector<long int>* messagePQ = new vector<long int>;
    vector<long int>* messagePRQ = new vector<long int>;
    vector<long int>* queryRecv = new vector<long int>;            
    (*messagePQ).resize(messageSize);
    (*messagePRQ).resize(messageSize);
    (*queryRecv).resize(messageSize);
    MPI_Request* requestPQ = (MPI_Request*)malloc(sizeof(MPI_Request));
    MPI_Request* requestPRQ = (MPI_Request*)malloc(sizeof(MPI_Request));
    MPI_Status* statusSend = (MPI_Status*)malloc(sizeof(MPI_Status));
    MPI_Status* statusRecv = (MPI_Status*)malloc(sizeof(MPI_Status));
    printf("req size:%ld\n", sizeof(*requestPQ));
    returnStruct* retVal = (returnStruct*)malloc(sizeof(returnStruct));
    sendQuery* sq = (sendQuery*)malloc(sizeof(sendQuery));
    bool* firstCallPQ = (bool*)malloc(sizeof(bool));
    bool* firstCallPRQ = (bool*)malloc(sizeof(bool));
    
    *firstCallPQ = true;
    *firstCallPRQ = true;


    for(int j = 1; j < num_processes; j++)
    {
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
        unify(x,y,unionfindDs,pointIdMapping,startIndex,processRank, retVal, sq);
        if(retVal->query == NULL)
        {
            // fprintf(fp,"Union of %ld and %ld done by process %d(no forwarding)\n",x,y,processRank);
            continue;
        }

        *multiple = *multiple + 1;
        long int queryNumSend;
        queryNumSend = i;
        (*replyRequired)[i] = true;

        createNewMessage(*firstCallPQ, messagePQ, requestPQ, statusSend , 1,queryNumSend,(long int)processRank,retVal->query->newQueryX,retVal->query->newQueryY);
        *firstCallPQ = false;
        *numMessages += 1;
        sendMessage(messagePQ,retVal->query->toProcess,processQueryNumMappingSend, requestPQ);
        // fprintf(fp,"Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,statusRecv);
        if(flag)
        {
            flag = 0;
            int src = (*statusRecv).MPI_SOURCE;
            int tag = (*statusRecv).MPI_TAG;
            MPI_Recv(&((*queryRecv)[0]),messageSize,MPI_LONG,src,tag,MPI_COMM_WORLD,statusRecv);
            processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,statusRecv, numQueriesCompleted, numMessages, messagePRQ, requestPRQ, statusSend,firstCallPRQ, retVal, sq);
        }
    }
    if((*replyRequired).size() > 0)
    {
        while((*replyRequired).size() > 0)
        {
            // printf("waiting for reply %d\n",processRank);
            MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,statusRecv);
            if(flag)
            {
                flag = 0;
                int src = (*statusRecv).MPI_SOURCE;
                int tag = (*statusRecv).MPI_TAG;
                MPI_Recv(&((*queryRecv)[0]),messageSize,MPI_LONG,src,tag,MPI_COMM_WORLD,statusRecv);
                processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,statusRecv, numQueriesCompleted, numMessages, messagePRQ, requestPRQ, statusSend,firstCallPRQ, retVal, sq);
            }
        }
    }
    *numQueriesCompleted += myNumQueries;
    // fprintf(fp,"process:%d has finished its queries.\n",processRank);
    createNewMessage(*firstCallPQ, messagePQ, requestPQ, statusSend, 3,myNumQueries,-1,-1,-1);
    for(int j = 1; j < num_processes; j++)
    {
        if(j == processRank)
        {
            continue;
        }
        sendMessage(messagePQ,j,processQueryNumMappingSend, requestPQ); // check this coz multiple sends without wait (will they be sent in order)
    }
    while(true)
    {
        printf("aaaa%d\n",processRank);
        if(*numQueriesCompleted == totalNumQueries) {
            break;
        }
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,statusRecv);
        if(flag)
        {
            flag = 0;
            int src = (*statusRecv).MPI_SOURCE;
            int tag = (*statusRecv).MPI_TAG;
            MPI_Recv(&((*queryRecv)[0]),messageSize,MPI_LONG,src,tag,MPI_COMM_WORLD,statusRecv);
            processReceivedQuery(queryRecv,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,statusRecv, numQueriesCompleted, numMessages, messagePRQ, requestPRQ, statusSend,firstCallPRQ, retVal, sq);
        }
    }
    // fprintf(fp,"process:%d exiting.\n",processRank);
}

void unify(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y, returnStruct* retVal, sendQuery* sq)
{
    long int root_y = y;
    long int its_parent = unionfindDs[y - startIndex];
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
            createQueryFwd(root_y, x, pointIdMapping[x],-1,sq);
            retVal->query = sq;
        }
    }
    else
    {
        if(its_parent < x)
        {
            createQueryFwd(x,its_parent,pointIdMapping[its_parent],-1,sq);
            retVal->query = sq;
        }
        else
        {
            createQueryFwd(its_parent,x,pointIdMapping[x],-1,sq);
            retVal->query = sq;
        }
    }
}

void createQueryFwd(long int newQueryX, long int newQueryY, int toProcess, long int finalParent, sendQuery* query)
{
    query->newQueryX = newQueryX;
    query->newQueryY = newQueryY;
    query->toProcess = toProcess;
    query->finalParent = finalParent;
}