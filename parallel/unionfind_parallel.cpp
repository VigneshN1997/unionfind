#include "preprocess.cpp"

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess,int num_processes)
{
    map<int, int> processQueryNumMappingSend;
    // map<int, int> processQueryNumMappingRecv;
    for(int i = 1; i < num_processes; i++)
    {
        if(i == processRank)
        {
            continue;
        }
        processQueryNumMappingSend[i] = 0;
        // processQueryNumMappingRecv[i] = 0;
    }
    
    MPI_Request request; // check this
    MPI_Status status;
    int flag;
    long int numQueries = queriesProcessX.size();
    for(int i = 0; i < numQueries; i++)
    {
        printf("%d: (%d,%d)\n",processRank,(int)queriesProcessX[i],(int)queriesProcessY[i]);
    }
    long int i;
    long int x,y;
    long int *a,*b;
    a = (long int*)malloc(sizeof(long int));
    b = (long int*)malloc(sizeof(long int));
    int process_of_x;
    long int root_y,its_parent;
    long int startIndex = (processRank - 1) * numPointsPerProcess;
    MPI_Request request;
    for(i = 0; i < numQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        *a = x;
        *b = y;
        returnStruct* retVal = unify(*a,*b,unionfindDs,pointIdMapping,startIndex,processRank);
        if(retVal->query == NULL)
        {
            printf("Union of %ld and %ld done by process %d\n",x,y,processRank);
            continue;
        }


        vector<long int> queryForward;
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

        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
        if(flag)
        {
            int src = status.MPI_SOURCE;
            int tag = status.MPI_TAG;
            int count;
            vector<long int> queryRecv;
            queryRecv.resize(2);
            MPI_Recv(&queryRecv[0],2,MPI_LONG,src,tag,,MPI_COMM_WORLD,&status);
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