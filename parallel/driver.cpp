#include "preprocess.cpp"

int main(int argc, char const *argv[])
{
	MPI_Init(NULL,NULL);
	int num_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	int my_rank;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	long int numPoints = atol(argv[1]); // 
	long int numPointsPerProcess = (long int)(numPoints/num_processes);
	numPoints = numPointsPerProcess*num_processes;
	
	// should all processes create this global array or should only process 0(master) have this array
	vector<long int>* pointIdMapping = new vector<long int>(numPoints);
	
	// union find array of process my_rank
	vector<long int>* unionfindDs = NULL;
	if(my_rank != 0)
	{
		unionfindDs = createArr(my_rank,numPointsPerProcess,pointIdMapping);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// testing if processes have the correct arrays
	vector<long int> recvArr;
	recvArr.resize(numPointsPerProcess);
	if(my_rank == 0)
	{
		int procId;
		long int startIndex,endIndex;
		for(procId = 1; procId < num_processes; procId++)
		{
			MPI_Recv(&recvArr[0],numPointsPerProcess,MPI_LONG,procId,procId,MPI_COMM_WORLD,&status);
			printf("points of process %d\n",status.MPI_SOURCE);
			startIndex = procId*numPointsPerProcess;
			endIndex = startIndex + numPointsPerProcess - 1;
			for(long int i = startIndex; i <= endIndex; i++)
			{
				printf("point:%ld parent:%ld\n",i,recvArr[i - startIndex]);
			}
			printf("---------------------------\n");
		}
	}
	else
	{
		MPI_Send(unionfindDs,numPointsPerProcess,MPI_LONG,0,my_rank,MPI_COMM_WORLD);
	}

	// create random queries
	vector<long int> queriesProcessX;
	vector<long int> queriesProcessY;
	if(my_rank == 0) // rank 0 process will give queries to all other processes
	{
		long int numQueries = atol(argv[2]); // 
		// for generating random numbers
		const unsigned int range_from  = 0;
		const unsigned int range_to    = UINT_MAX;
		std::random_device rand_dev;
		std::mt19937       generator(rand_dev());
		std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);
		vector<vector<long int> > queriesAllProcessesX;
		vector<vector<long int> > queriesAllProcessesY;
		for(int i = 1; i < num_processes; i++)
		{
			vector<long int> queriesOneProcessX;
			vector<long int> queriesOneProcessY;
			queriesAllProcessesX.append(queriesOneProcessX);
			queriesAllProcessesY.append(queriesOneProcessY);
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
			int process_of_y = pointIdMapping[y];
			queriesAllProcessesX[process_of_y - 1].append((long int)x);
			queriesAllProcessesY[process_of_y - 1].append((long int)y);
		}
		// testing
		for(int i = 1; i < num_processes; i++)
		{
			printf("queries sent to process %d\n",i);
			for(long int j = 0; j < queriesAllProcessesX[i-1].size(); j++)
			{
				printf("(%d,%d)\n",queriesAllProcessesX[i-1][j],queriesAllProcessesY[i-1][j]);
			}
		}

		// sending respective query arrays to respective processes
		for(int i = 1; i < num_processes; i++)
		{
			long int numQueriesProcess = queriesAllProcessesX[i-1].size();
			MPI_Send(&numQueriesProcess,1,MPI_LONG,i,0,MPI_COMM_WORLD); // send number of queries for the process
			MPI_Send(&queriesAllProcessesX[i-1][0],numQueriesProcess,MPI_LONG,i,1,MPI_COMM_WORLD); // send the queries vector to the respective process
			MPI_Send(&queriesAllProcessesY[i-1][0],numQueriesProcess,MPI_LONG,i,2,MPI_COMM_WORLD); // send the queries vector to the respective process
		}
	}
	else
	{
		long int numQueriesProcess;
		MPI_Recv(&numQueriesProcess,1,MPI_LONG,0,0,MPI_COMM_WORLD);
		queriesProcessX.resize(numQueriesProcess);
		queriesProcessY.resize(numQueriesProcess);
		MPI_Recv(&queriesProcessX[0],numQueriesProcess,MPI_LONG,0,1,MPI_COMM_WORLD);
		MPI_Recv(&queriesProcessY[0],numQueriesProcess,MPI_LONG,0,2,MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	// now all processes have the query array they have to process
	processQueries(my_rank,queriesProcessX,queriesProcessY,unionfindDs,pointIdMapping);
	MPI_Finalize();
	return 0;
}