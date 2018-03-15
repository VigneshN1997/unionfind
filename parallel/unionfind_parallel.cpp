#include "preprocess.cpp"

void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<int> pointIdMapping,long int numPointsPerProcess)
{
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
    for(i = 0; i < numQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        *a = x;
        *b = y;
        root_y = y;
        its_parent = unionfindDs[y - startIndex];
        while(root_y < its_parent && pointIdMapping[])
    }
}
