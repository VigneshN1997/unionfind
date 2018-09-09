#include "unionfind_modified.h"

vector<pair<vector<long int>*, vector<long int>* >* >* initializeUpdatesStructure(int num_processes) {
    vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone = new vector<pair<vector<long int>*, vector<long int>* >* >;
    (*updatesDone).resize(num_processes - 1);
    for(int i = 1; i < num_processes; i++) {
        pair<vector<long int>*, vector<long int>* >* pairForAProcess = new pair<vector<long int>*, vector<long int>* >;
        vector<long int>* queryNumsVec = new vector<long int>;
        vector<long int>* finalParentsVec = new vector<long int>;
        (*queryNumsVec).clear();
        (*finalParentsVec).clear();
        (*pairForAProcess).first = queryNumsVec;
        (*pairForAProcess).second = finalParentsVec;
        (*updatesDone)[i-1] = pairForAProcess;
    }
    return updatesDone;
}


void processQueriesDeferredUpdates(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int> queryNums, long int* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes, long int totalNumQueries, long int* numMessages, long int* multiple) 
{
    long int* numQueriesCompleted = (long int*)malloc(sizeof(long int));
    *numQueriesCompleted = 0;
    unordered_map<int, int>* processQueryNumMappingSend = new unordered_map<int,int>; // mapping from process number to tag // do something for this
    unordered_map<long int, vector<long int>* >* replyRequired = new unordered_map<long int, vector<long int>* >; // mapping from queryNum to list of y's with which this process started
    unordered_map<long int, bool>* replyRequiredSelf = new unordered_map<long int, bool>;
    unordered_map<long int, vector<int>* >* replyToBeSent = new unordered_map<long int, vector<int>* >; // mapping from queryNum to list of processNums to which to send reply

    vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone = initializeUpdatesStructure(num_processes);

    MPI_Request request; // check this
    MPI_Status status;
    int flag;
    long int myNumQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int startIndex = (processRank - 1) * numPointsPerProcess;

    for(int j = 1; j < num_processes; j++)
    {
        (*processQueryNumMappingSend)[j] = 0;
    }

    for(i = 0; i < myNumQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        // printf("process %d trying union of %ld,%ld\n",processRank, x, y);
        returnStruct* retVal = unifyPathCompression(x,y,unionfindDs,pointIdMapping,startIndex,processRank);

        if(retVal->query->finalParent != -1)
        {
            printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
            continue;
        }

        vector<long int>* queryWithWhichProcessStarted = new vector<long int>;
        (*queryWithWhichProcessStarted).push_back(y);
        (*replyRequired)[queryNums[i]] = queryWithWhichProcessStarted;
        (*replyRequiredSelf)[queryNums[i]] = true;

        sendUpdatesWithQueryForwarding(retVal, updatesDone, queryNums[i], processRank, processQueryNumMappingSend);
        
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = 0;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            int* count = (int*)malloc(sizeof(int));
            MPI_Get_count(&status, MPI_LONG, count);
            vector<long int> queryRecv;
            queryRecv.resize(*count);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQueryModified(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,updatesDone,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
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
                int* count = (int*)malloc(sizeof(int));
                MPI_Get_count(&status, MPI_LONG, count);
                vector<long int> queryRecv;
                queryRecv.resize(*count);
                MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
                processReceivedQueryModified(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,updatesDone,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
            }
        }
    }
    *numQueriesCompleted += myNumQueries;
    printf("process:%d has finished its queries.\n",processRank);
    vector<long int> finishedMsg = createNewMessageModified(5,myNumQueries,-1,-1,0, -1);
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
            int* count = (int*)malloc(sizeof(int));
            MPI_Get_count(&status, MPI_LONG, count);
            vector<long int> queryRecv;
            queryRecv.resize(*count);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQueryModified(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,updatesDone,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }

    while((*replyRequired).size() > 0) {
        for(int i = 1; i < num_processes; i++) {
            long int numUpdates = (*(*((*updatesDone)[i-1])).first).size();
            if(numUpdates > 0) {
                if(i == processRank) {
                    processUpdates(*(*((*updatesDone)[i-1])).first, *(*((*updatesDone)[i-1])).second,numUpdates,replyToBeSent,replyRequired,updatesDone,unionfindDs,pointIdMapping,startIndex,processRank);
                    (*(*((*updatesDone)[i-1])).first).clear();
                    (*(*((*updatesDone)[i-1])).second).clear();
                    continue;
                }
                if(numUpdates > MAX_UPDATES) {
                    numUpdates = MAX_UPDATES;
                }
                
                if(numUpdates > 0) {
                    vector<long int>* queryNumsVec =(*((*updatesDone)[i-1])).first;
                    vector<long int>* finalParentsVec = (*((*updatesDone)[i-1])).second;
                    vector<long int>* queryForward = createNewMessageModified(3,-1,-1,-1, numUpdates, -1);
                    (*queryForward).insert((*queryForward).end(),(*queryNumsVec).begin(), (*queryNumsVec).begin() + numUpdates);
                    (*queryForward).insert((*queryForward).end(),(*finalParentsVec).begin(), (*finalParentsVec).begin() + numUpdates);
                    
                    sendMessage(queryForward,i,processQueryNumMappingSend);

                    (*queryNumsVec).erase((*queryNumsVec).begin(), (*queryNumsVec).begin() + numUpdates);
                    (*finalParentsVec).erase((*finalParentsVec).begin(), (*finalParentsVec).begin() + numUpdates);
                    
                }
            }
        }
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if(flag)
        {
            flag = 0;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            int* count = (int*)malloc(sizeof(int));
            MPI_Get_count(&status, MPI_LONG, count);
            vector<long int> queryRecv;
            queryRecv.resize(*count);
            MPI_Recv(&queryRecv[0],queryRecv.size(),MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            processReceivedQueryModified(queryRecv,replyRequired,replyRequiredSelf,replyToBeSent,updatesDone,unionfindDs,pointIdMapping,startIndex,processRank,processQueryNumMappingSend,status, numQueriesCompleted);
        }
    }

    printf("process:%d exiting.\n",processRank);
}

// 1->query forward 2->query forward with updates 3->only updates(no query) 4->reply for completing a query 5->processing of a process finished
vector<long int> createNewMessageModified(long int messageType, long int queryNum, long int newQueryX, long int newQueryY, long int numUpdates, long int originalProcessRank)
{
    vector<long int>* message = new vector<long int>;
    (*message).resize(6);
    (*message)[0] = messageType;
    (*message)[1] = queryNum;
    (*message)[2] = newQueryX;
    (*message)[3] = newQueryY;
    (*message)[4] = numUpdates;
    (*message)[5] = originalProcessRank;
    return message;
}

void processUpdates(vector<long int> queryNumsVec, vector<long int> finalParentsVec, long int numUpdates, unordered_map<long int, vector<int>* >* replyToBeSent, unordered_map<long int, vector<long int>* >* replyRequired, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone,long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank)
{
    long int queryReplyNum = 0;
    long int finalParent = 0;
    for(int i = 0; i < numUpdates; i++) {
        queryReplyNum = queryNumsVec[i];
        finalParent = finalParentsVec[i];
        unordered_map<long int,vector<int>* >::iterator map_itr2 = (*replyToBeSent).find(queryReplyNum);
        if(map_itr2 != (*replyToBeSent).end())
        {
            vector<int> tempVec = *(map_itr2->second);
            for(int i = 0; i < tempVec.size(); i++) {
                (*(*(*updatesDone)[tempVec[i]-1]).first).push_back(queryReplyNum);
                (*(*(*updatesDone)[tempVec[i]-1]).second).push_back(finalParent);
            }
            (*replyToBeSent).erase(map_itr2);           
        }
        unordered_map<long int, vector<long int>* >::iterator map_itr = (*replyRequired).find(queryReplyNum);
        if(map_itr != (*replyRequired).end())
        {
            vector<long int> tempYs = *(map_itr->second);
            for(int i = 0; i < tempYs.size(); i++) {
                doPathCompression(tempYs[i],finalParent,processRank,startIndex,pointIdMapping ,unionfindDs);
            }
            (*replyRequired).erase(map_itr);
        }
    }
}


void sendUpdatesWithQueryForwarding(returnStruct* retVal, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone, long int queryNum, long int originalProcessRank, unordered_map<int,int>* processQueryNumMappingSend) {
    int toProcess = retVal->query->toProcess;
    long int numUpdates = (*(*((*updatesDone)[toProcess-1])).first).size();
    if(numUpdates > MAX_UPDATES) {
        numUpdates = MAX_UPDATES;
    }
    vector<long int>* queryNumsVec =(*((*updatesDone)[toProcess-1])).first;
    vector<long int>* finalParentsVec = (*((*updatesDone)[toProcess-1])).second;
    if(numUpdates > 0) {
        vector<long int>* queryForward = createNewMessageModified(2,queryNum,retVal->query->newQueryX,retVal->query->newQueryY, numUpdates, originalProcessRank);
        (*queryForward).insert((*queryForward).end(),(*queryNumsVec).begin(), (*queryNumsVec).begin() + numUpdates);
        (*queryForward).insert((*queryForward).end(),(*finalParentsVec).begin(), (*finalParentsVec).begin() + numUpdates);
        
        sendMessage(queryForward,toProcess,processQueryNumMappingSend);

        (*queryNumsVec).erase((*queryNumsVec).begin(), (*queryNumsVec).begin() + numUpdates);
        (*finalParentsVec).erase((*finalParentsVec).begin(), (*finalParentsVec).begin() + numUpdates);
        
    }
    else {
        vector<long int>* queryForward = createNewMessageModified(1,queryNum,retVal->query->newQueryX,retVal->query->newQueryY, 0, originalProcessRank);
        sendMessage(queryForward,toProcess,processQueryNumMappingSend);
    }
    printf("Sent query %ld union=>union(%ld,%ld) to process %d with tag %d\n",queryNum,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
}

void processReceivedQueryModified(vector<long int> queryRecv, unordered_map<long int, vector<long int>* >* replyRequired,unordered_map<long int, bool>* replyRequiredSelf,unordered_map<long int, vector<int>* >* replyToBeSent, vector<pair<vector<long int>*, vector<long int>* >* >* updatesDone, long int* unionfindDs,vector<int> pointIdMapping,long int startIndex, int processRank,unordered_map<int,int>* processQueryNumMappingSend,MPI_Status status, long int* numQueriesCompleted)
{
    returnStruct* retVal;
    int srcOfMsg = status.MPI_SOURCE;
    if(queryRecv[0] == 1 || queryRecv[0] == 2) {
        if(queryRecv[0] == 2) {
            printf("hello %d %d %d (%d)\n", status.MPI_TAG, status.MPI_TAG+1, status.MPI_TAG+2, processRank);
            long int numUpdates = queryRecv[4];
            vector<long int> queryNumsVec;
            vector<long int> finalParentsVec;
            queryNumsVec.assign(queryRecv.begin() + 6, queryRecv.begin() + 6 + numUpdates);
            finalParentsVec.assign(queryRecv.begin() + 6 + numUpdates, queryRecv.begin() + 6 + numUpdates + numUpdates);
            // printf("::::r\n");
            // for(int i = 0; i < numUpdates; i++) {
            //     printf("(%ld %ld)",queryNumsVec[i],finalParentsVec[i]);
            // }
            // printf("\n");
            processUpdates(queryNumsVec, finalParentsVec, numUpdates, replyToBeSent, replyRequired, updatesDone, unionfindDs, pointIdMapping, startIndex, processRank);
        }
        retVal = unifyPathCompression(queryRecv[2],queryRecv[3],unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query->finalParent != -1)
        {
            printf("Union of %ld done by process %d\n",queryRecv[1],processRank);
            vector<long int> replyMsg = createNewMessagePathCompression(4,queryRecv[1],-1,-1,-1);
            sendMessage(replyMsg,queryRecv[5],processQueryNumMappingSend);
            printf("Reply for query %ld sent to process %d\n", queryRecv[1], (int)queryRecv[5]);
            // printf("Union of %ld and %ld done by process %d\n",queryRecv[2],queryRecv[3],processRank);
            (*(*(*updatesDone)[srcOfMsg-1]).first).push_back(queryRecv[1]);
            (*(*(*updatesDone)[srcOfMsg-1]).second).push_back(retVal->query->finalParent);
            unordered_map<long int,vector<int>* >::iterator map_itr2 = (*replyToBeSent).find(queryRecv[1]);
            if(map_itr2 != (*replyToBeSent).end())
            {
                vector<int> tempVec = *(map_itr2->second);
                for(int i = 0; i < tempVec.size(); i++) {
                    (*(*(*updatesDone)[tempVec[i]-1]).first).push_back(queryRecv[1]);
                    (*(*(*updatesDone)[tempVec[i]-1]).second).push_back(retVal->query->finalParent);
                }
                (*replyToBeSent).erase(map_itr2);           
            }
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
            sendUpdatesWithQueryForwarding(retVal, updatesDone, queryForWhichReplyRequired, queryRecv[5], processQueryNumMappingSend);
            // printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",queryRecv[2],queryRecv[3],retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,(*processQueryNumMappingSend)[retVal->query->toProcess] - 1);
        }

    }
    
    else if(queryRecv[0] == 3) {
        long int numUpdates = queryRecv[4];
        vector<long int> queryNumsVec;
        vector<long int> finalParentsVec;
        vector<long int> queryNumsVec;
        vector<long int> finalParentsVec;
        queryNumsVec.assign(queryRecv.begin() + 6, queryRecv.begin() + 6 + numUpdates);
        finalParentsVec.assign(queryRecv.begin() + 6 + numUpdates, queryRecv.begin() + 6 + numUpdates + numUpdates);
        // printf("::::P\n");
        // for(int i = 0; i < numUpdates; i++) {
        //     printf("(%ld %ld)",queryNumsVec[i],finalParentsVec[i]);
        // }
        // printf("\n");
        processUpdates(queryNumsVec, finalParentsVec, numUpdates, replyToBeSent, replyRequired, updatesDone, unionfindDs, pointIdMapping, startIndex, processRank);

    }
    else if(queryRecv[0] == 4) {
        unordered_map<long int,bool>::iterator map_itr3 = (*replyRequiredSelf).find(queryRecv[1]);
        if(map_itr3 != (*replyRequiredSelf).end())
        {
            printf("Reply for query %ld received\n", queryRecv[1]);
            (*replyRequiredSelf).erase(map_itr3);           
        }
    }
    else if(queryRecv[0] == 5) {
        *numQueriesCompleted += queryRecv[1];
    }
}
