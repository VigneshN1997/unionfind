vector<long int> createNewMessagePathCompression(int processDone,long int queryNum,int processRank, long int isReply, long int newQueryX, long int newQueryY,long int oldQueryY ,long int finalParent,int queryOriginatingProcessNum)
{
    vector<long int> message;
    message.clear();
    message.push_back((long int)processDone);
    message.push_back(queryNum);    // for a message which a query (query forwarding)
    message.push_back((long int)processRank);
    message.push_back(isReply); // query number of the query for which reply has come
    message.push_back(newQueryX);
    message.push_back(newQueryY);
    message.push_back(oldQueryY);
    message.push_back(finalParent);// used in path compression
    message.push_back(queryOriginatingProcessNum);
    // prvs query Y  is also needed
    // old query Y is original query Y
    message.resize(9);
    return message;
}

void processReceivedQueryPathCompression(vector<long int> queryRecv,vector<bool>* finished, map<vector<long int>, bool>* replyRequired,map<vector<long int>, int>* replyToBeSent,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,map<int,int>* processQueryNumMappingSend,long int* queryNum,MPI_Status status)
{
    returnStruct* retVal;
    if(queryRecv[0] == 1) // means process that sent this message has finished its processing
    {
        (*finished)[status.MPI_SOURCE - 1] = true;
    }
    else if(queryRecv[3] > 0)
    {
        printf("Received reply for queryNumber %ld by process %d from process %d\n",queryRecv[3],processRank,status.MPI_SOURCE);
        vector<long int> queryReply;
        // (query number,oldQueryY)
        queryReply.push_back(queryRecv[3]);
        queryReply.push_back(queryRecv[6]);
        queryReply.resize(2);
        map<vector<long int>,bool>::iterator map_itr = (*replyRequired).find(queryReply);
        if(map_itr != (*replyRequired).end())
        {
            (*replyRequired).erase(map_itr);
            queryReply.clear();
            // (query number, oldQueryY, queryOriginatingProcess)
            queryReply.push_back(queryRecv[3]);
            queryReply.push_back(queryRecv[6]);
            queryReply.push_back(queryRecv[8]);
            queryReply.resize(3);
            map<vector<long int>,int>::iterator map_itr2 = (*replyToBeSent).find(queryReply);
            if(map_itr2 != (*replyToBeSent).end())
            {
                vector<long int> replyMsg = createNewMessagePathCompression(0,-1,-1,queryReply[0],-1,-1,queryReply[1],queryRecv[7],queryReply[2]);
                sendMessage(replyMsg,map_itr2->second,processQueryNumMappingSend);
                (*replyToBeSent).erase(map_itr2);           
            }
            doPathCompression(queryRecv[6],queryRecv[7],processRank,long int startIndex,vector<int> pointIdMapping ,vector<long int>* unionfindDs);
        }
    }
    else if(queryRecv[1] > 0)
    {
        retVal = unify(queryRecv[4],queryRecv[5],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            vector<long int> replyMsg = createNewMessage(0,-1,-1,queryRecv[1],-1,-1);
            // print message here
            sendMessage(replyMsg,queryRecv[2],processQueryNumMappingSend);
            printf("Sent reply to process %d for union of (%ld,%ld)\n",queryRecv[2],queryRecv[4],queryRecv[5]);
        }
        else
        {
            vector<long int> queryForward = createNewMessage(0,queryRecv[1],queryRecv[2],-1,retVal->query->newQueryX,retVal->query->newQueryY);
            sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[4],queryRecv[5],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess] - 1);
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
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[4],queryRecv[5],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess] - 1);
        }
    }
}

void processQueriesPathCompression(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes)
{
    map<int, int>* processQueryNumMappingSend = new map<int,int>(); // mapping from process number to tag // do something for this
    map<vector<long int>, bool>* replyRequired = new map<vector<long int>, bool>(); // mapping from (query number,oldQueryY) to true
    map<vector<long int>, int>* replyToBeSent = new map<vector<long int>, int>(); // mapping from (query number, oldQueryY, queryOriginatingProcess) to processNum to which to send reply
    long int* queryNum = (long int*)malloc(sizeof(long int)); //  used only when a query is sent to a process which has finished
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
        if(j == processRank)
        {
            continue;
        }
        (*processQueryNumMappingSend)[j] = 0;
    }
    for(int j = 1; j < num_processes; j++)
    {
        (*finished)[j-1] = false;
    }

    for(i = 0; i < numQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        returnStruct* retVal = unifyPathCompression(x,y,unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query->finalParent != -1)
        {
            printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
            continue;
        }

        long int queryNumSend;
        
        queryNumSend = *queryNum; // reply required
        vector<long int> queryforWhichReplyRequired;
        queryforWhichReplyRequired.push_back(queryNumSend);
        queryforWhichReplyRequired.push_back(y);
        (*replyRequired)[queryforWhichReplyRequired] = true;
        (*queryNum)++;

        vector<long int> queryForward = createNewMessagePathCompression(0,queryNumSend,processRank,-1,retVal->query->newQueryX,retVal->query->newQueryY,y,-1,processRank);
        sendMessage(queryForward,retVal->query->toProcess,processQueryNumMappingSend);
        printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess] - 1);

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = false;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(9);
            MPI_Recv(&queryRecv[0],9,MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQueryPathCompression(queryRecv,finished,replyRequired,replyToBeSent,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,queryNum,status);
        }
    }
}


retVal* unifyPathCompression(long int x, long int y, vector<long int>* unionfindDs, vector<int> pointIdMapping, long int startIndex, int process_of_y)
{
    long int root_y = y;
    long int its_parent = (*unionfindDs)[root_y - startIndex];
    long int* final_parent = NULL;
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
            (*unionfindDs[root_y - startIndex]) = x;
            final_parent = (long int*)malloc(sizeof(long int));
            printf("set parent of %ld to %ld\n",root_y,x);
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
    if(its_parent != NULL)
    {
        doPathCompression(y,*final_parent,process_of_y,startIndex,pointIdMapping,unionfindDs);
    }
    return retVal;
}

void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex,vector<int> pointIdMapping ,vector<long int>* unionfindDs)
{
    long int node = startNode;
    while(node < parent && pointIdMapping[node] == process_of_y)
    {
        long int temp = (*unionfindDs)[node - startIndex];
        if((*unionfindDs)[node - startIndex] != parent)
        {
            (*unionfindDs)[node - startIndex] = parent;
            printf("updated parent of %ld to %ld in process %d\n",node,parent,process_of_y);
        }
        node = temp;
    }
}