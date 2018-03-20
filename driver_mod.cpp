#include "unionfind_mod.cpp"

// vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr)
// {
// 	vector<Result*> statsVector;
// 	printf("bucket\tnumPts\t\tnumQueries\ttime\t\tnumMsgs\t\tmultiple\n");
// 	for(int j = 0; j < numPointsArr.size(); j++)
// 	{
// 		for(int i = 0; i < numBucketsArr.size(); i++)
// 		{
// 			UnionFind* uf = init_unionfind(numPointsArr[j],numBucketsArr[i]);
// 			// printUnionFind(uf);
// 			for(int k = 0; k < queryPercentArr.size(); k++)
// 			{
// 				Result* res = (Result*)malloc(sizeof(Result));
// 				res->bucketSize = numBucketsArr[i];
// 				res->numPoints = numPointsArr[j];
// 				res->numQueries = (long int)(queryPercentArr[k]*numPointsArr[j]);
// 				res->numMessages = 0;
// 				res->multipleMsgs = 0;
// 				printf("%d\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries);
// 				clock_t st_time,end_time;
// 				st_time = clock();
// 				for(long int l = 0; l < res->numQueries; l++)
// 				{
// 					unsigned int x = distr(generator)%(uf->num_elems);
// 					unsigned int y = distr(generator)%(uf->num_elems);
// 					while(y == x)
// 					{
// 						y = distr(generator)%(uf->num_elems);
// 					}
// 					int process_of_x = uf->global_arr[x].process_num;
// 					int process_of_y = uf->global_arr[y].process_num;
// 					// printf("union of:%d(%d) %d(%d)",x,process_of_x,y,process_of_y);
// 					long int* num_messages = (long int*)malloc(sizeof(long int));
// 					*num_messages = 0;
// 					bool sameSet = unify((long int)x,(long int)y,uf,process_of_x,process_of_y,num_messages);
// 					if(*num_messages > 0)
// 					{
// 						res->multipleMsgs++;
// 					}
// 					// cout << "  sameSet:" << sameSet << "  num_messages:" << *num_messages << "\n";
// 					res->numMessages += *num_messages;
// 				}
// 				end_time = clock();
// 				res->time = (double)(end_time - st_time)/(double)CLOCKS_PER_SEC;
// 				printf("%d\t%ld\t\t%ld\t\t%lf\t\t%ld\t\t%ld\n",res->bucketSize,res->numPoints,res->numQueries,res->time,res->numMessages,res->multipleMsgs);
// 				statsVector.push_back(res);
// 			}
// 			free(uf);
// 		}	
// 	}
// 	return statsVector;
// }



int main(int argc, char const *argv[])
{
	UnionFind* uf = init_unionfind(20,4);
	long int* num_messages = (long int*)malloc(sizeof(long int));
	long int x,y;
	x = 1;
	y = 2;
	*num_messages = 0;
	unifyOptimized(1,x,y,uf,uf->global_arr[x],uf->global_arr[y],num_messages,-1);
	printf("union of (%ld,%ld) done, num_messages=%ld\n",x,y,*num_messages);


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