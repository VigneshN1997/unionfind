#include "unionfind.cpp"

const unsigned int range_from  = 0;
const unsigned int range_to    = UINT_MAX;
std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());
std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);

vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr)
{
	vector<Result*> statsVector;
	printf("bucket\tnumPts\t\tnumQueries\ttime\t\tnumMsgs\t\tmultiple\n");
	for(int j = 0; j < numPointsArr.size(); j++)
	{
		for(int i = 0; i < numBucketsArr.size(); i++)
		{
			UnionFind* uf = init_unionfind(numPointsArr[j],numBucketsArr[i]);
			// printUnionFind(uf);
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
				for(long int l = 0; l < res->numQueries; l++)
				{
					unsigned int x = distr(generator)%(uf->num_elems);
					unsigned int y = distr(generator)%(uf->num_elems);
					while(y == x)
					{
						y = distr(generator)%(uf->num_elems);
					}
					int process_of_x = uf->global_arr[x].process_num;
					int process_of_y = uf->global_arr[y].process_num;
					// printf("union of:%d(%d) %d(%d)",x,process_of_x,y,process_of_y);
					long int* num_messages = (long int*)malloc(sizeof(long int));
					*num_messages = 0;
					bool sameSet = unify((long int)x,(long int)y,uf,process_of_x,process_of_y,num_messages);
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
			}
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
	vector <long int> numPointsArr{1000000,50000000,100000000};
	vector<float> queryPercentArr{0.2,0.5};
	cout << "Bucket Sizes:" << "\t";
	for(i = 0; i < numBucketsArr.size(); i++)
	{
		cout << numBucketsArr[i] << "\t";
	}
	cout << "\nNumber of points:" << "\t";
	for(i = 0; i < numPointsArr.size(); i++)
	{
		cout << numPointsArr[i] << "\t";
	}
	cout << "\nQuery percent:" << "\t";
	for(i = 0; i < queryPercentArr.size(); i++)
	{
		cout << queryPercentArr[i] << "\t";
	}
	cout << "\n";
	// vector<Result*> resultArr = do_statistical_analysis(numBucketsArr,numPointsArr,queryPercentArr);
	
	// testing correctness
	int num_Queries = 20;
	UnionFind* uf = init_unionfind(24,4);
	for(long int l = 0; l < num_Queries; l++)
	{
		unsigned int x = distr(generator)%(uf->num_elems);
		unsigned int y = distr(generator)%(uf->num_elems);
		while(y == x)
		{
			y = distr(generator)%(uf->num_elems);
		}
		int process_of_x = uf->global_arr[x].process_num;
		int process_of_y = uf->global_arr[y].process_num;
		printf("union of:%d(%d) %d(%d)",x,process_of_x,y,process_of_y);
		long int* num_messages = (long int*)malloc(sizeof(long int));
		long int* final_parent = (long int*)malloc(sizeof(long int));
		*num_messages = 0;
		unifyPathCompression((long int)x,(long int)y,uf,process_of_x,process_of_y,num_messages,final_parent);
		// if(*num_messages > 0)
		// {
		// 	res->multipleMsgs++;
		// }
		// cout << "  sameSet:" << sameSet << "  num_messages:" << *num_messages << "\n";
		printf("num_messages:%ld\n",*num_messages);
		// res->numMessages += *num_messages;
	}
	return 0;
}