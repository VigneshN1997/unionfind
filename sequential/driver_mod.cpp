#include "generateandTest.cpp"
vector<Result*> do_statistical_analysis1(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr)
{
	vector<Result*> statsVector;
	long int* num_messages = (long int*)malloc(sizeof(long int));
	long int* final_parent = (long int*)malloc(sizeof(long int));
	printf("bucket\tnumPts\t\tnumQueries\ttime\t\tnumMsgs\t\tmultiple\n");
	for(int j = 0; j < numPointsArr.size(); j++)
	{
		for(int i = 0; i < numBucketsArr.size(); i++)
		{
			UnionFind* uf = init_unionfind(numPointsArr[j],numBucketsArr[i]);
			for(int k = 0; k < queryPercentArr.size(); k++)
			{
				Result* res = (Result*)malloc(sizeof(Result));
				res->bucketSize = numBucketsArr[i];
				res->numPoints = numPointsArr[j];
				res->numQueries = (long int)(queryPercentArr[k]*numPointsArr[j]);
				res->numMessages = 0;
				res->multipleMsgs = 0;
				printf("%d\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries);
				clock_t st_time,end_time;
				st_time = clock();
				string queryFileName = "queryFile_"+to_string(uf->num_elems) + "_" + to_string(res->numQueries) + "_" + to_string(numBucketsArr[i]);
				fstream queryFile(queryFileName,std::ios_base::in);
				for(long int l = 0; l < res->numQueries; l++)
				{
					long int x,y;
					queryFile >> x;
					queryFile >> y;
					int process_of_x = uf->global_arr[x];
					int process_of_y = uf->global_arr[y];
			// 		printf("union of:%ld(%d) %ld(%d)\n",x,process_of_x,y,process_of_y);
					*num_messages = 0;
					*final_parent = -1;
					unifyPathCompression(x,y,uf,process_of_x,process_of_y,num_messages,final_parent);
					if(*num_messages > 0)
					{
						res->multipleMsgs++;
					}
					// cout << "  sameSet:" << sameSet << "  num_messages:" << *num_messages << "\n";
					res->numMessages += *num_messages;
				}
				end_time = clock();
				res->time = (double)(end_time - st_time)/(double)CLOCKS_PER_SEC;
				printf("%d\t%ld\t\t%ld\t\t%lf\t\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries,res->time,res->numMessages,res->multipleMsgs);
				statsVector.push_back(res);
				printUnionfindToFileMap(uf,(long int)(queryPercentArr[k]*numPointsArr[j]));
				queryFile.close();
			}
			free(uf);
		}	
	}
	return statsVector;
}


vector<Result*> do_statistical_analysis2(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr)
{
	vector<Result*> statsVector;
	long int* num_messages = (long int*)malloc(sizeof(long int));
	// printf("bucket\tnumPts\t\tnumQueries\ttime\t\tnumMsgs\t\tmultiple\n");
	for(int j = 0; j < numPointsArr.size(); j++)
	{
		for(int i = 0; i < numBucketsArr.size(); i++)
		{
			UnionFind_mod* uf = init_unionfindmod(numPointsArr[j],numBucketsArr[i]);
			for(int k = 0; k < queryPercentArr.size(); k++)
			{
				Result* res = (Result*)malloc(sizeof(Result));
				res->bucketSize = numBucketsArr[i];
				res->numPoints = numPointsArr[j];
				res->numQueries = (long int)(queryPercentArr[k]*numPointsArr[j]);
				res->numMessages = 0;
				res->multipleMsgs = 0;
				printf("%d\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries);
				clock_t st_time,end_time;
				st_time = clock();
				string queryFileName = "queryFile_"+to_string(uf->num_elems) + "_" + to_string(res->numQueries) + "_" + to_string(numBucketsArr[i]);
				fstream queryFile(queryFileName,std::ios_base::in);
				for(long int l = 0; l < res->numQueries; l++)
				{
					long int x,y;
					queryFile >> x;
					queryFile >> y;
					int process_of_x = uf->global_arr[x];
					int process_of_y = uf->global_arr[y];

					Query q = createNewQuery(x,y,x,y);
			// 		printf("union of:%ld(%d) %ld(%d)\n",x,process_of_x,y,process_of_y);
					*num_messages = 0;
					// *final_parent = -1;
					vector<queryParentMapping> updateVec;
					updateVec.clear();
					// if(numPointsArr[j] == 1000000 && numBucketsArr[i] == 128 && queryPercentArr[k] == 0.2 && x == 966764 && y == 999998)
					// {
					// 	printUnionfindToFileVector(uf);
					// }
					unifyOptimized(l,q,uf,process_of_x,process_of_y,num_messages,updateVec,-1);
					if(*num_messages > 0)
					{
						res->multipleMsgs++;
					}
					// cout << "  sameSet:" << sameSet << "  num_messages:" << *num_messages << "\n";
					res->numMessages += *num_messages;
				}
				end_time = clock();
				long int numUpdatesLeft = 0;
				for(int r = 0; r < (*(uf->updatesDone)).size(); r++)
				{
					for(int s = 0; s < (*(uf->updatesDone[r])).size(); s++)
					{
						numUpdatesLeft += (*(uf->updatesDone[r]))[s].size(); 
					}
				}
				printf("%d\t%ld\t\t%ld\t\t%lf\t\t%ld\t\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries,res->time,res->numMessages,res->multipleMsgs,numUpdatesLeft);
				doPathCompressionOfRemainingUpdates(uf);
				res->time = (double)(end_time - st_time)/(double)CLOCKS_PER_SEC;
				statsVector.push_back(res);
				
				// printUnionfindToFileVector(uf,(long int)(queryPercentArr[k]*numPointsArr[j]));
				queryFile.close();
			}
			delete(uf->updatesDone);
			delete(uf->unionQueriesSent);
			delete(uf->queriesToBeReplied);
			// for(int h = 0; h < uf->num_elems / uf->num_elems_per_arr; h++)
			// {
			// 	free(uf->array[i]);
			// }
			free(uf->array);
			free(uf->global_arr);
			free(uf);
		}	
	}
	return statsVector;
}
int main(int argc, char const *argv[])
{
	vector <int> numBucketsArr;
	int smallestBucketSize = 8;
	int i;
	for(i = 0; i < 8; i++,smallestBucketSize *= 2)
	{
		numBucketsArr.push_back(smallestBucketSize);
	}
	// ,100000000,1000000000};
	vector <long int> numPointsArr{1000000,50000000};
	vector<float> queryPercentArr{0.2,0.5};
	generateQueryFiles(numBucketsArr,numPointsArr,queryPercentArr);
	// do_statistical_analysis1(numBucketsArr,numPointsArr,queryPercentArr);
	do_statistical_analysis2(numBucketsArr,numPointsArr,queryPercentArr);
	// UnionFind* uf = init_unionfind(20,4);
	// long int* num_messages = (long int*)malloc(sizeof(long int));
	// long int x,y;
	// x = 1;
	// y = 2;
	// *num_messages = 0;
	// unifyOptimized(1,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);


	// x = ;
	// y = ;
	// *num_messages = 0;
	// unifyOptimized(2,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);

	// x = ;
	// y = ;
	// *num_messages = 0;
	// unifyOptimized(3,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);

	// x = ;
	// y = ;
	// *num_messages = 0;
	// unifyOptimized(4,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);

	// x = ;
	// y = ;
	// *num_messages = 0;
	// unifyOptimized(5,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);

	// x = ;
	// y = ;
	// *num_messages = 0;
	// unifyOptimized(6,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	// printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);
	return 0;
}