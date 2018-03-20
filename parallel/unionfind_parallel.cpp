#include "preprocess.cpp"

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes)
{
    map<int, int> processQueryNumMappingSend; // mapping from process number to tag
    map<long int, bool> replyRequired; // mapping from query number to true
    long int queryNum = 0; // used only when a query is sent to a process which has finished
    vector<bool> finished;
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
        processQueryNumMappingSend[j] = 0;
    }
    for(int j = 1; j < num_processes; j++)
    {
        finished.push_back(false);
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

        vector<long int> queryForward;
        queryForward.clear();
        /*Query control messages*/
        queryForward.push_back(0); // the process has not completed its queries
        if(finished[(retVal->query->toProcess) - 1] == 1) // need to send query to a process which has finished its processing
        {
            queryForward.push_back(queryNum); // reply required
            replyRequired[queryNum++] = true;
        }
        else
        {
            queryForward.push_back(-1); // reply not required
        }
        queryForward.push_back(processRank); // which process will receive reply
        queryForward.push_back(-1); // since this message is not a reply message (reply message will contain a queryNum here)
        /*Query x and y*/
        queryForward.push_back(retVal->query->newQueryX);
        queryForward.push_back(retVal->query->newQueryY);
        MPI_Isend(&queryForward[0],6,MPI_LONG,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess],MPI_COMM_WORLD,&request);
        printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess]);
        
        processQueryNumMappingSend[retVal->query->toProcess] += 1;
        if(processQueryNumMappingSend[retVal->query->toProcess] == INT_MAX)
        {
            processQueryNumMappingSend[retVal->query->toProcess] = 0;
        }

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            flag = false;
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            vector<long int> queryRecv;
            queryRecv.resize(6);
            MPI_Recv(&queryRecv[0],6,MPI_LONG,src,tag,MPI_COMM_WORLD,&status);
            if(queryRecv[0] == 1) // means process that sent this message has finished its processing
            {
                finished[src - 1] = 1; 
            }
            else if(queryRecv[3] > 0)
            {
                map<long int,bool>::iterator map_itr = replyRequired.find(queryRecv[3]);
                replyRequired.erase(map_itr);
            }
            else if(queryRecv[1] > 0) // reply needs to be sent
            {
                retVal = unify(queryRecv[4],queryRecv[5],unionfindDs,pointIdMapping,startIndex,processRank);
                vector<long int> queryForward1;
                queryForward.resize(6);
                if(retVal->query == NULL)
                {
                    queryForward1[0] = 0;
                    queryForward1[1] = -1;
                    queryForward1[2] = -1;
                    queryForward1[3] = queryRecv[1];
                    queryForward1[4] = -1;
                    queryForward1[5] = -1;
                    MPI_Isend(&queryForward[0],6,MPI_LONG,queryRecv[2],processQueryNumMappingSend[queryRecv[2]],MPI_COMM_WORLD,&request);

                    processQueryNumMappingSend[queryRecv[2]] += 1;
                    if(processQueryNumMappingSend[queryRecv[2]] == INT_MAX)
                    {
                        processQueryNumMappingSend[queryRecv[2]] = 0;
                    }
                }
                else
                {
                    queryForward1[0] = 0;
                    queryForward1[1] = queryRecv[1];
                    queryForward1[2] = queryRecv[2];
                    queryForward1[3] = -1;
                    queryForward1[4] = retVal->query->newQueryX;
                    queryForward1[5] = retVal->query->newQueryY;
                    MPI_Isend(&queryForward[0],6,MPI_LONG,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess],MPI_COMM_WORLD,&request);

                    processQueryNumMappingSend[retVal->query->toProcess] += 1;
                    if(processQueryNumMappingSend[retVal->query->toProcess] == INT_MAX)
                    {
                        processQueryNumMappingSend[retVal->query->toProcess] = 0;
                    }
                }
            }
            else // reply need not be sent
            {
                retVal = unify(queryRecv[4],queryRecv[5],unionfindDs,pointIdMapping,startIndex,processRank);
                vector<long int> queryForward1;
                queryForward.resize(6);
                if(retVal->query == NULL)
                {
                    printf("Union of %ld and %ld done by process %d\n",queryRecv[4],queryRecv[5],processRank);
                }
                else
                {
                    
                }
            }



            retVal = unify(queryRecv[0],queryRecv[1],unionfindDs,pointIdMapping,startIndex,processRank);
            if(retVal->query == NULL)
            {
                printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
                continue;
            }

            queryForward.clear();
            queryForward.push_back(retVal->query->newQueryX);
            queryForward.push_back(retVal->query->newQueryY);
            MPI_Isend(&queryForward[0],2,MPI_LONG,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess],MPI_COMM_WORLD,&request);
            printf("Sent query union(%ld,%ld)=>union(%ld,%ld) to process %d with tag %d\n",x,y,retVal->query->newQueryX,retVal->query->newQueryY,retVal->query->toProcess,processQueryNumMappingSend[retVal->query->toProcess]);
            
            processQueryNumMappingSend[retVal->query->toProcess] += 1;
            if(processQueryNumMappingSend[retVal->query->toProcess] == INT_MAX)
            {
                processQueryNumMappingSend[retVal->query->toProcess] = 0;
            }
        }
    }
}

returnStruct* unify(long int x, long int y, vector<long int> unionfindDs, vector<int> pointIdMapping, long int startIndex,int process_of_y)
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
            retVal->unionDone = true;
        }
        else if(root_y == x)
        {
            retVal->unionDone = false;
        }
        else
        {
            retVal->query = (sendQuery*)malloc(sizeof(sendQuery));
            retVal->query->newQueryX = root_y;
            retVal->query->newQueryY = x;
            retVal->query->toProcess = pointIdMapping[x];
            retVal->query->fromProcess = process_of_y;
        }
    }
    else
    {
        retVal->query = (sendQuery*)malloc(sizeof(sendQuery));
        if(its_parent < x)
        {
            retVal->query->newQueryX = x;
            retVal->query->newQueryY = its_parent;
            retVal->query->toProcess = pointIdMapping[x];
            retVal->query->fromProcess = pointIdMapping[its_parent];
        }
        else
        {
            retVal->query->newQueryX = its_parent;
            retVal->query->newQueryY = x;
            retVal->query->toProcess = pointIdMapping[its_parent];
            retVal->query->fromProcess = pointIdMapping[x];
        }
    }
    return retVal;
}