
void processQueries(int processRank,vector<long int> queriesProcessX,vector<long int> queriesProcessY,vector<long int>* unionfindDs,vector<long int>* pointIdMapping)
{
    long int numQueries = queriesProcessX.size();
    long int i;
    long int x,y;
    long int *a,*b;
    a = (long int*)malloc(sizeof(long int));
    b = (long int*)malloc(sizeof(long int));
    int process_of_x,process_of_y;
    bool doneUnion;
    for(i = 0; i < numQueries; i++)
    {
        x = queriesProcessX[i];
        y = queriesProcessY[i];
        *a = x;
        *b = y;
    	doneUnion = unify(x,y,a,b);
    }
}
