#include "unionfind_modified.cpp"
int main(int argc, char const *argv[])
{
	MPI_Init(NULL,NULL);
	int num_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	// printf("num_processes:%d\n",num_processes);
	int my_rank;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	// printf("hello from process:%d\n",my_rank);
	unordered_map<string, long int> config = getConfig("config.txt");
	long int numPoints = config["numPoints"];
	long int numQueries = config["numQueries"];
	// printf("points:%ld\n",numPoints);
	// printf("queries:%ld\n",numQueries);
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
		// for(int  i = 0; i < numPointsPerProcess; i++)
		// {
		// 	printf("process:%d i:%d parent:%d\n",my_rank,i,(int)unionfindDs[i]);
		// }
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// testing if processes have the correct arrays

	// if(my_rank != 0)
	// {
	// 	for(int i = 0; i < (*pointIdMapping).size(); i++)
	// 	{
	// 		printf("process:%d i:%d :%d\n",my_rank,i,(*pointIdMapping)[i]);
	// 	}
	// }

	if(my_rank == 0)
	{
		vector<long int> recvArr;
		vector<int> recvMappingArr;
		recvArr.resize(numPointsPerProcess);
		recvMappingArr.resize(numPointsPerProcess);
		int procId;
		for(procId = 1; procId < num_processes; procId++)
		{
			// MPI_Recv(&recvArr[0],numPointsPerProcess,MPI_LONG,procId,procId,MPI_COMM_WORLD,&status);
			MPI_Recv(&recvMappingArr[0],numPointsPerProcess,MPI_INT,procId,procId,MPI_COMM_WORLD,&status);


			pointIdMappingMain.insert(pointIdMappingMain.begin() + (procId - 1)*numPointsPerProcess,recvMappingArr.begin(),recvMappingArr.end());
			// printf("points of process %d\n",status.MPI_SOURCE);
			// long int startIndex = (procId - 1)*numPointsPerProcess;
			// long int endIndex = startIndex + numPointsPerProcess - 1;
			// for(long int i = startIndex; i <= endIndex; i++)
			// {
			// 	printf("point:%ld parent:%ld\n",i,recvArr[i - startIndex]);
			// }
			// printf("---------------------------\n");
		}
		pointIdMappingMain.resize(numPoints);
		// for(int k = 0; k < pointIdMappingMain.size(); k++)
		// {
		// 	printf("i:%d process:%d\n",k,pointIdMappingMain[k]);
		// }
	}
	else
	{
		// MPI_Send(&(*unionfindDs)[0],numPointsPerProcess,MPI_LONG,0,my_rank,MPI_COMM_WORLD);
		MPI_Send(&(*pointIdMapping)[0],numPointsPerProcess,MPI_INT,0,my_rank,MPI_COMM_WORLD);
	}
	MPI_Bcast(&pointIdMappingMain[0],pointIdMappingMain.size(),MPI_INT,0,MPI_COMM_WORLD);

	vector<long int> queriesProcessX;
	vector<long int> queriesProcessY;
	vector<long int> queryNums;
	if(my_rank == 0) // rank 0 process will give queries to all other processes
	{
		// for generating random numbers
		const unsigned int range_from  = 0;
		const unsigned int range_to    = UINT_MAX;
		std::random_device rand_dev;
		std::mt19937       generator(rand_dev());
		std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);
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
			unsigned int x = distr(generator)%(numPoints);
			unsigned int y = distr(generator)%(numPoints);
			while(y == x)
			{
				y = distr(generator)%(numPoints);
			}
			// int process_of_x = pointIdMapping[x];
			int process_of_y = pointIdMappingMain[y];
			// printf("proc:%d\n",process_of_y);
			queriesAllProcessesX[process_of_y - 1].push_back((long int)x);
			queriesAllProcessesY[process_of_y - 1].push_back((long int)y);
			queryNumsAllProcesses[process_of_y - 1].push_back(i);
		}
		// testing
		for(int i = 1; i < num_processes; i++)
		{
			printf("queries sent to process %d\n",i);
			for(long int j = 0; j < queriesAllProcessesX[i-1].size(); j++)
			{
				printf("%ld:(%ld,%ld)\n",queryNumsAllProcesses[i-1][j],queriesAllProcessesX[i-1][j],queriesAllProcessesY[i-1][j]);
			}
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
		char* logFileName = (char*)malloc(12);
		strcpy(logFileName, "logprocess");
		logFileName[10] = (char)(my_rank+48);
		logFileName[11] = '\0';
		FILE* fp = fopen(logFileName, "w");
		start_time = MPI_Wtime();
		processQueriesDeferredUpdates(my_rank,queriesProcessX,queriesProcessY,queryNums,unionfindDs,pointIdMappingMain,numPointsPerProcess,num_processes, numQueries, numMessages, multiple); // num_process-1 coz process 0's work is complete
		printUnionFindDs(my_rank, unionfindDs, numPointsPerProcess);
		max_end_time = MPI_Wtime() - start_time;
		printf("time for process(%d):%lf\n",my_rank,max_end_time);
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