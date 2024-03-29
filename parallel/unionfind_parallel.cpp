#include "preprocess.cpp"

void sendMessage(vector<long int> queryForward, int toProcess,unordered_map<int,int>* processQueryNumMappingSend)
{
    MPI_Request request;
    MPI_Isend(&queryForward[0],queryForward.size(),MPI_LONG,toProcess,(*processQueryNumMappingSend)[toProcess],MPI_COMM_WORLD,&request);
    
    (*processQueryNumMappingSend)[toProcess] += 1;
    if((*processQueryNumMappingSend)[toProcess] == INT_MAX)
    {
        (*processQueryNumMappingSend)[toProcess] = 0;
    }
}

void processReceivedQuery(vector<long int> queryRecv,vector<bool>* finished, unordered_map<long int,bool>* replyRequired,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend,long int* queryNum,MPI_Status status)
{
    returnStruct* retVal;
    if(queryRecv[0] == 1) // means process that sent this message has finished its processing
    {
        (*finished)[status.MPI_SOURCE - 1] = true;
    }
    else if(queryRecv[3] > 0)
    {
        printf("Received reply for queryNumber %ld by process %d from process %d\n",queryRecv[3],processRank,status.MPI_SOURCE);
        unordered_map<long int,bool>::iterator map_itr = (*replyRequired).find(queryRecv[3]);
        (*replyRequired).erase(map_itr);
    }
    else if(queryRecv[1] > 0)
    {
        retVal = unify(queryRecv[4],queryRecv[5],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            vector<long int> replyMsg = createNewMessage(0,-1,-1,queryRecv[1],-1,-1);
            // print message here
            sendMessage(replyMsg,queryRecv[2],processQueryNumMappingSend);
            printf("Sent reply to process %d for union of (%ld,%ld)\n",(int)queryRecv[2],queryRecv[4],queryRecv[5]);
        }
        else
        {
            vector<long int> queryForward = createNewMessage(0,queryRecv[1],queryRecv[2],-1,retVal->query->newQueryX,retVal->query->newQueryY);
            sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[4],queryRecv[5],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }
    }
    else
    {
        retVal = unify(queryRecv[4],queryRecv[5],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            printf("Union of %ld and %ld done by process %d\n",queryRecv[4],queryRecv[5],processRank);
        }
        else
        {
            long int queryNumSend;
            if((*finished)[(retVal->query->toProcess) - 1] == true)
            {
                queryNumSend = *queryNum;
                (*replyRequired)[*queryNum] = true;
                (*queryNum)++;
            }
            else
            {
                queryNumSend = -1;
            }
            vector<long int> queryForward = createNewMessage(0,queryNumSend,processRank,-1,retVal->query->newQueryX,retVal->query->newQueryY);
            sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[4],queryRecv[5],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }
    }
}

vector<long int> createNewMessage(int processDone,long int queryNum,int processRank, long int isReply, long int newQueryX, long int newQueryY)
{
    vector<long int> message;
    message.clear();
    message.push_back((long int)processDone);
    message.push_back(queryNum);
    message.push_back((long int)processRank);
    message.push_back(isReply);
    message.push_back(newQueryX);
    message.push_back(newQueryY);
    message.resize(6);
    return message;
}


bool convertToBool(long int num)
{
    if(num > 0)
    {
        return true;
    }
    return false;
}

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes)
{
    unordered_map<int, int>* processQueryNumMappingSend = new unordered_map<int,int>(); // mapping from process number to tag
    unordered_map<long int, bool>* replyRequired = new unordered_map<long int, bool>(); // mapping from query number to true
    long int* queryNum = (long int*)malloc(sizeof(long int)); // used only when a query is sent to a process which has finished
    *queryNum = 0;
    vector<bool>* finished = new vector<bool>(num_processes);
    MPI_Request request; // check this
    MPI_Status status;
    int flag;
    long int numQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int startIndex = (processRank - 1) * numPointsPerProcess;
    
    for(int j = 1; j < num_processes; j++)
    {
        // if(j == processRank)
        // {
        //     continue;
        // }
        (*processQueryNumMappingSend)[j] = 0;
    }
    for(int j = 1; j < num_processes; j++)
    {
        (*finished)[j-1] = false;
    }

    // for(int i = 0; i < numQueries; i++)
    // {
    //     printf("%d: (%d,%d)\n",processRank,(int)queriesProcessX[i],(int)queriesProcessY[i]);
    // }
    for(i = 0; i < numQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        returnStruct* retVal = unify(x,y,unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
            continue;
        }

        long int queryNumSend;
        if((*finished)[(retVal->query->toProcess) - 1] == true) // need to send query to a process which has finished its processing
        {
            queryNumSend = *queryNum; // reply required
            (*replyRequired)[*queryNum] = true;
            (*queryNum)++;
        }
        else
        {
            queryNumSend = -1; // reply not required
        }
        vector<long int> queryForward = createNewMessage(0,queryNumSend,processRank,-1,retVal->query->newQueryX,retVal->query->newQueryY);
        sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
        printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = false;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(6);
            MPI_Recv(&queryRecv[0],6,MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQuery(queryRecv,finished,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,queryNum,status);
        }
    }
    if((*replyRequired).size() > 0)
    {
        while((*replyRequired).size() > 0)
        {
            printf("waiting for reply %d\n",processRank);
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if(flag)
            {
                flag = false;
                int src = status.MPI_SOURCE;
                int tag = status.MPI_TAG;
                vector<long int> queryRecv;
                queryRecv.resize(6);
                MPI_Recv(&queryRecv[0],6,MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
                processReceivedQuery(queryRecv,finished,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,queryNum,status);
            }
        }
    }
    (*finished)[processRank - 1] = true;
    printf("process:%d has finished.\n",processRank);
    vector<long int> finishedMsg = createNewMessage(1,-1,-1,-1,-1,-1);
    for(int j = 1; j < num_processes; j++)
    {
        if(j == processRank)
        {
            continue;
        }
        sendMessage(finishedMsg,j,processQueryNumMappingSend);
    }
    bool completed = true;
    while(true)
    {
        // printf("aaaa%d\n",processRank);
        for(int j = 1; j < num_processes; j++)
        {
            if(!(*finished)[j - 1])
            {
                completed = false;
                break;
            }
        }
        if(!completed)
        {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            if(flag)
            {
                flag = false;
                int src = status.MPI_SOURCE;
                int tag = status.MPI_TAG;
                vector<long int> queryRecv;
                queryRecv.resize(6);
                MPI_Recv(&queryRecv[0],6,MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
                processReceivedQuery(queryRecv,finished,replyRequired,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,queryNum,status);
            }
        }
        else
        {
            break;
        }
        completed = true;
    }
}

returnStruct* unify(long int x, long int y, vector<long int>* unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y)
{
    long int root_y = y;
    long int its_parent = (*unionfindDs)[y - startIndex];
    returnStruct* retVal = (returnStruct*)malloc(sizeof(returnStruct));
    retVal->query = NULL;
    while(root_y < its_parent && pointIdMapping[its_parent] == process_of_y)
    {
        root_y = its_parent;
        its_parent = (*unionfindDs)[root_y - startIndex];
    }

    if(root_y == its_parent)
    {
        if(root_y < x)
        {
            (*unionfindDs)[root_y - startIndex] = x;
            retVal->unionDone = true;
        }
        else if(root_y == x)
        {
            retVal->unionDone = false;
        }
        else
        {
            retVal->query = createQueryFwd(root_y, x, pointIdMapping[x],-1);
            // retVal->query->fromProcess = process_of_y;
        }
    }
    else
    {
        retVal->query = (sendQuery*)malloc(sizeof(sendQuery));
        if(its_parent < x)
        {
            retVal->query = createQueryFwd(x,its_parent,pointIdMapping[its_parent],-1);
            // retVal->query->fromProcess = pointIdMapping[its_parent];
        }
        else
        {
            retVal->query = createQueryFwd(its_parent,x,pointIdMapping[x],-1);
            // retVal->query->fromProcess = pointIdMapping[x];
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