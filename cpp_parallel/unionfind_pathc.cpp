#include "unionfind_pathc.h"
// message type: 1-query forward, 2-reply, 3-processing of a process finished
vector<long int> createNewMessagePathCompression(long int messageType, long int queryNum, long int newQueryX, long int newQueryY,long int finalParent)
{
    vector<long int> message;
    message.clear();
    message.push_back(messageType);
    message.push_back(queryNum);
    message.push_back(newQueryX);               
    message.push_back(newQueryY);
    message.push_back(finalParent);
    message.resize(5);
    return message;
}

// CHECK THIS SELF MESSAGES CAN BE SENT ****

void processQueriesPathCompression(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int> queryNums, long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries)
{
    long int* numQueriesCompleted = (long int*)malloc(sizeof(long int));
    *numQueriesCompleted = 0;

    unordered_map<int, int>* processQueryNumMappingSend = new unordered_map<int,int>; // mapping from process number to tag // do something for this
    unordered_map<long int, vector<long int>* >* replyRequired = new unordered_map<long int, vector<long int>* >; // mapping from queryNum to list of y's with which this process started
    unordered_map<long int, bool>* replyRequiredSelf = new unordered_map<long int, bool>;
    unordered_map<long int, vector<int>* >* replyToBeSent = new unordered_map<long int, vector<int>* >; // mapping from queryNum to list of processNums to which to send reply
    
    MPI_Request request; // check this
    MPI_Status status;
    int flag;
    long int myNumQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int startIndex = (processRank - 1) * numPointsPerProcess;
    // for(int i = 0; i < myNumQueries; i++)
    // {
    //     printf("%ld:%d: (%d,%d)\n",queryNums[i],processRank,(int)queriesProcessX[i],(int)queriesProcessY[i]);
    // }

    for(int j = 1; j < num_processes; j++)
    {
        (*processQueryNumMappingSend)[j] = 0;
    }

    for(i = 0; i < myNumQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        returnStruct* retVal = unifyPathCompression(x,y,unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query->finalParent != -1)
        {
            // printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
            continue;
        }

        vector<long int>* queryWithWhichProcessStarted = new vector<long int>;
        (*queryWithWhichProcessStarted).push_back(y);
        (*replyRequired)[queryNums[i]] = queryWithWhichProcessStarted;
        (*replyRequiredSelf)[queryNums[i]] = true;
        vector<long int> queryForward = createNewMessagePathCompression(1,queryNums[i],retVal->query->newQueryX,retVal->query->newQueryY,-1);
        sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
        // printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = 0;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(5);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQueryPathCompression(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }
    if((*replyRequiredSelf).size() > 0)
    {
        while((*replyRequiredSelf).size() > 0)
        {
            // printf("not done proc:%d\n",processRank);
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if(flag)
            {
                flag = 0;
                int src = status.MPI_SOURCE;
                int tag = status.MPI_TAG;
                vector<long int> queryRecv;
                queryRecv.resize(5);
                MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
                processReceivedQueryPathCompression(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
            }
        }
    }
    *numQueriesCompleted += myNumQueries;
    printf("process:%d has finished its queries.\n",processRank);
    vector<long int> finishedMsg = createNewMessagePathCompression(3,myNumQueries,-1,-1,-1);
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
            processReceivedQueryPathCompression(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }
    printf("process:%d exiting.\n",processRank);
}


void processReceivedQueryPathCompression(vector<long int> queryRecv, unordered_map<long int, vector<long int>* >* replyRequired,unordered_map<long int, bool>* replyRequiredSelf,unordered_map<long int, vector<int>* >* replyToBeSent,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend,MPI_Status status, long int* numQueriesCompleted)
{
    returnStruct* retVal;
    if(queryRecv[0] == 1) {
        retVal = unifyPathCompression(queryRecv[2],queryRecv[3],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query->finalParent != -1)
        {
            // printf("Union of %ld and %ld done by process %d\n",queryRecv[2],queryRecv[3],processRank);
            vector<long int> replyMsg = createNewMessagePathCompression(2,queryRecv[1],-1,-1,retVal->query->finalParent);
            sendMessage(replyMsg,status.MPI_SOURCE,processQueryNumMappingSend);
        }
        else
        {
            long int queryForWhichReplyRequired = queryRecv[1];
            unordered_map<long int, vector<long int>* >::iterator repItr = (*replyRequired).find(queryForWhichReplyRequired);
            unordered_map<long int, vector<int>* >::iterator repSItr = (*replyToBeSent).find(queryForWhichReplyRequired);
            if(repItr != (*replyRequired).end()) {
                (*(*replyRequired)[queryForWhichReplyRequired]).push_back(queryRecv[3]);
            }
            else {
                vector<long int>* vec = new vector<long int>;
                (*vec).push_back(queryRecv[3]);
                (*replyRequired)[queryForWhichReplyRequired] = vec;  
            }
            if(repSItr != (*replyToBeSent).end()) {
                (*(*replyToBeSent)[queryForWhichReplyRequired]).push_back(status.MPI_SOURCE);
            }
            else {
                vector<int>* vec = new vector<int>;
                (*vec).push_back(status.MPI_SOURCE);
                (*replyToBeSent)[queryForWhichReplyRequired] = vec;  
            }
            vector<long int> queryForward = createNewMessagePathCompression(1,queryForWhichReplyRequired,retVal->query->newQueryX,retVal->query->newQueryY,-1);
            sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
            // printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[2],queryRecv[3],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }
    }
    else if(queryRecv[0] == 2) {
        // printf("Received reply for query %ld by process %d from process %d\n",queryRecv[1],processRank,status.MPI_SOURCE);
        long int queryReplyNum = queryRecv[1];
        unordered_map<long int,vector<int>* >::iterator map_itr2 = (*replyToBeSent).find(queryReplyNum);
        if(map_itr2 != (*replyToBeSent).end())
        {
            vector<int> tempVec = *(map_itr2->second);
            for(int i = 0; i < tempVec.size(); i++) {
                sendMessage(queryRecv,tempVec[i],processQueryNumMappingSend);
            }
            (*replyToBeSent).erase(map_itr2);           
        }
        unordered_map<long int,bool>::iterator map_itr3 = (*replyRequiredSelf).find(queryReplyNum);
        if(map_itr3 != (*replyRequiredSelf).end())
        {
            (*replyRequiredSelf).erase(map_itr3);           
        }
        unordered_map<long int, vector<long int>* >::iterator map_itr = (*replyRequired).find(queryReplyNum);
        if(map_itr != (*replyRequired).end())
        {
            vector<long int> tempYs = *(map_itr->second);
            for(int i = 0; i < tempYs.size(); i++) {
                doPathCompression(tempYs[i],queryRecv[4],processRank,startIndex,pointIdMapping ,unionfindDs);
            }
            (*replyRequired).erase(map_itr);
        }
    }
    else if(queryRecv[0] == 3) {
        *numQueriesCompleted += queryRecv[1];
    }
}



returnStruct* unifyPathCompression(long int x, long int y, long int* unionfindDs, vector<int> pointIdMapping, long int startIndex, int process_of_y)
{
    long int root_y = y;
    long int its_parent = unionfindDs[root_y - startIndex];
    long int* final_parent = NULL;
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
            final_parent = (long int*)malloc(sizeof(long int));
            // printf("set parent of %ld to %ld\n",root_y,x);
            *final_parent = x;
            retVal->query = createQueryFwd(-1,-1,-1,*final_parent); // rest values not needed
            retVal->unionDone = true;
        }
        else if(root_y == x)
        {
            final_parent = (long int*)malloc(sizeof(long int));
            *final_parent = x;
            retVal->query = createQueryFwd(-1,-1,-1,*final_parent);
            retVal->unionDone = false; // means the two points are in the same set
        }
        else
        {
            // forward message
            retVal->query = createQueryFwd(root_y,x,pointIdMapping[x],-1); // final parent is not yet set
        }
    }
    else
    {
        if(its_parent < x)
        {
            // forward message
            retVal->query = createQueryFwd(x,its_parent,pointIdMapping[its_parent],-1);
        }
        else
        {
            // forward message
            retVal->query = createQueryFwd(its_parent,x,pointIdMapping[x],-1);
        }
    }
    if(final_parent != NULL)
    {
        doPathCompression(y,*final_parent,process_of_y,startIndex,pointIdMapping,unionfindDs);
    }
    return retVal;
}

void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex,vector<int> pointIdMapping ,long int* unionfindDs)
{
    long int node = startNode;
    while(node < parent && pointIdMapping[node] == process_of_y)
    {
        long int temp = unionfindDs[node - startIndex];
        if(unionfindDs[node - startIndex] != parent)
        {
            unionfindDs[node - startIndex] = parent;
            // printf("updated parent of %ld to %ld in process %d\n",node,parent,process_of_y);
        }
        node = temp;
    }
}