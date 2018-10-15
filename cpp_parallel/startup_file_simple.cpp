#include "unionfind_simple.cpp"
int main(int argc, char* argv[])
{
	MPI_Init(NULL,NULL);
	int num_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	int my_rank;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	long int numPoints = atol(argv[1]);
	long int numQueries = atol(argv[2]);
	long int numPointsPerProcess = (long int)(numPoints/(num_processes - 1));
	numPoints = numPointsPerProcess*(num_processes-1);

	// should all processes create this global array or should only process 0(master) have this array
	vector<int> pointIdMappingMain;
	pointIdMappingMain.resize(numPoints);
	vector<int>* pointIdMapping = NULL;
	if(my_rank != 0)
	{	
		pointIdMapping = new vector<int>();
		(*pointIdMapping).resize(numPointsPerProcess);
	}
	// union find array of process my_rank
	long int* unionfindDs = NULL;
	if(my_rank != 0)
	{
		unionfindDs = createArr(my_rank,numPointsPerProcess,pointIdMapping);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(my_rank == 0)
	{
		vector<long int> recvArr;
		vector<int> recvMappingArr;
		recvArr.resize(numPointsPerProcess);
		recvMappingArr.resize(numPointsPerProcess);
		int procId;
		for(procId = 1; procId < num_processes; procId++)
		{
			MPI_Recv(&recvMappingArr[0],numPointsPerProcess,MPI_INT,procId,procId,MPI_COMM_WORLD,&status);


			pointIdMappingMain.insert(pointIdMappingMain.begin() + (procId - 1)*numPointsPerProcess,recvMappingArr.begin(),recvMappingArr.end());
		}
		pointIdMappingMain.resize(numPoints);
	}
	else
	{
		MPI_Send(&(*pointIdMapping)[0],numPointsPerProcess,MPI_INT,0,my_rank,MPI_COMM_WORLD);
	}
	MPI_Bcast(&pointIdMappingMain[0],pointIdMappingMain.size(),MPI_INT,0,MPI_COMM_WORLD);

	vector<long int> queriesProcessX;
	vector<long int> queriesProcessY;
	vector<long int> queryNums;
	if(my_rank == 0) // rank 0 process will give queries to all other processes
	{
		string queryFileName = "queryFile_"+to_string(numPoints) + "_" + to_string(numQueries) + "_" + to_string(num_processes - 1);
		fstream queryFile(queryFileName,std::ios_base::in);

		vector<vector<long int> > queriesAllProcessesX;
		vector<vector<long int> > queriesAllProcessesY;
		vector<vector<long int> > queryNumsAllProcesses;
		for(int i = 1; i < num_processes; i++)
		{
			vector<long int> queriesOneProcessX;
			vector<long int> queriesOneProcessY;
			vector<long int> queryNumsOneProcess;
			queriesAllProcessesX.push_back(queriesOneProcessX);
			queriesAllProcessesY.push_back(queriesOneProcessY);
			queryNumsAllProcesses.push_back(queryNumsOneProcess);
		}
		for(long int i = 0; i < numQueries; i++)
		{
			unsigned int x;
			unsigned int y;
			queryFile >> x;
			queryFile >> y;
			int process_of_y = pointIdMappingMain[y];
			queriesAllProcessesX[process_of_y - 1].push_back((long int)x);
			queriesAllProcessesY[process_of_y - 1].push_back((long int)y);
			queryNumsAllProcesses[process_of_y - 1].push_back(i);
		}

		// sending respective query arrays to respective processes
		for(int i = 1; i < num_processes; i++)
		{
			long int numQueriesProcess = queriesAllProcessesX[i-1].size();
			MPI_Send(&numQueriesProcess,1,MPI_LONG,i,0,MPI_COMM_WORLD); // send number of queries for the process
			MPI_Send(&queriesAllProcessesX[i-1][0],numQueriesProcess,MPI_LONG,i,1,MPI_COMM_WORLD); // send the queries vector to the respective process
			MPI_Send(&queriesAllProcessesY[i-1][0],numQueriesProcess,MPI_LONG,i,2,MPI_COMM_WORLD); // send the queries vector to the respective process
			MPI_Send(&queryNumsAllProcesses[i-1][0],numQueriesProcess,MPI_LONG,i,3,MPI_COMM_WORLD);
		}
	}
	else
	{
		long int numQueriesProcess;
		MPI_Recv(&numQueriesProcess,1,MPI_LONG,0,0,MPI_COMM_WORLD,&status);
		queriesProcessX.resize(numQueriesProcess);
		queriesProcessY.resize(numQueriesProcess);
		queryNums.resize(numQueriesProcess);
		MPI_Recv(&queriesProcessX[0],numQueriesProcess,MPI_LONG,0,1,MPI_COMM_WORLD,&status);
		MPI_Recv(&queriesProcessY[0],numQueriesProcess,MPI_LONG,0,2,MPI_COMM_WORLD,&status);
		MPI_Recv(&queryNums[0],numQueriesProcess,MPI_LONG,0,3,MPI_COMM_WORLD,&status);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	// now all processes have the query array they have to process
	double start_time, max_end_time;
	double end_times[num_processes-1];
	long int numMessagesArr[num_processes - 1];
	long int multipleBucketsArr[num_processes - 1];
	long int* numMessages = (long int*)malloc(sizeof(long int));
	*numMessages = 0;
	long int* multiple = (long int*)malloc(sizeof(long int));
	*multiple = 0;
	if(my_rank != 0)
	{
		// char* logFileName = (char*)malloc(12);
		// strcpy(logFileName, "logprocess");
		// logFileName[10] = (char)my_rank;
		// logFileName[11] = '\0';
		// FILE* fp = fopen(logFileName, "w");
		start_time = MPI_Wtime();
		processQueries(my_rank,queriesProcessX,queriesProcessY,unionfindDs,pointIdMappingMain,numPointsPerProcess,num_processes, numQueries, numMessages, multiple); // num_process-1 coz process 0's work is complete
		max_end_time = MPI_Wtime() - start_time;
		MPI_Send(&max_end_time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(numMessages, 1, MPI_LONG, 0, 2, MPI_COMM_WORLD);
		MPI_Send(multiple, 1, MPI_LONG, 0, 3, MPI_COMM_WORLD);		
		// printUnionFindDs(my_rank, unionfindDs, numPointsPerProcess);
	}
	else {
		for(int i = 1; i < num_processes; i++) {
			MPI_Recv(end_times+i-1, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(numMessagesArr+i-1,1, MPI_LONG, i, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(multipleBucketsArr+i-1,1, MPI_LONG,i, 3, MPI_COMM_WORLD, &status);
		}
		for(int i = 1; i < num_processes; i++) {
			*numMessages += numMessagesArr[i-1];
			*multiple += multipleBucketsArr[i-1];
		}
		max_end_time = end_times[0];
		for(int i = 2; i < num_processes; i++) {
			if(end_times[i-1] > max_end_time) {
				max_end_time = end_times[i-1];
			}
		}
		printf("------------------------simple----------------------------\n");
		printf("Number of processes:%d\n", num_processes - 1);
		printf("Number of points:%ld\n", numPoints);
		printf("Number of queries:%ld\n", numQueries);
		printf("Time taken:%lf\n", max_end_time);
		printf("Number of messages:%ld\n", *numMessages);
		printf("Number of queries forwarded(i.e. not resolved by the process it was sent to):%ld\n", *multiple);

	}
	MPI_Finalize();
	return 0;
}