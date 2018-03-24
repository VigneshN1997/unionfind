#include "unionfind_mod.cpp"

/*
This file is used for generating union find queries and also writing the final union find ds to file
*/

void generateRandomQueries(long int numQueries, long int maxNum,int numBuckets)
{

	
	FILE* fp = fopen("queryFile_"+ltoa(maxNum) + "_" + ltoa(numQueries) + "_" + itoa(numBuckets),"w");
	const unsigned int range_from  = 0;
	const unsigned int range_to    = UINT_MAX;
	std::random_device                  rand_dev;
	std::mt19937                        generator(rand_dev());
	std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);

	for(long int l = 0; l < numQueries; l++)
	{
		unsigned int x = distr(generator) % maxNum;
		unsigned int y = distr(generator) % maxNum;
		while(y == x)
		{
			y = distr(generator)%maxNum;
		}
		fprintf(fp, "%ld %ld\n",(long int)x,(long int)y);
	}
	fclose(fp);
}

// void printUnionfindToFileMap(UnionFind* uf,long int numQueries)
// {
// 	int numBuckets = (int)(uf->num_elems/uf->num_elems_per_arr);
// 	FILE* fp = fopen("unionfindDs_"+ltoa(uf->num_elems) + "_" + ltoa(numQueries) + "_" + itoa(numBuckets),"w");
// 	int num_processes = (uf->array).size();
// 	int i;
// 	map<long int,long int>::iterator j;
// 	for(i = 0; i < num_processes; i++)
// 	{
// 		fprintf(fp,"Points with process %d\n",i);
// 		for(j = array[i].begin(); j != array[i].end(); j++)
// 		{
// 			fprintf(fp, "%ld:%ld\n",j->first,j->second);
// 		}
// 	}
// }


void printUnionfindToFileVector(UnionFind* uf,long int numQueries)
{
	int numBuckets = (int)(uf->num_elems/uf->num_elems_per_arr);
	FILE* fp = fopen("unionfindDs_"+ltoa(uf->num_elems) + "_" + ltoa(numQueries) + "_" + itoa(numBuckets),"w");
	int num_processes = (uf->array).size();
	int i;
	long int j;
	long int startIndex;
	for(i = 0; i < num_processes; i++)
	{
		startIndex = i*uf->num_elems_per_arr;
		fprintf(fp,"Points with process %d\n",i);
		fprintf(fp, "Point Parent of point\n");
		for(j = 0; j < uf->num_elems_per_arr; j++)
		{
			fprintf(fp, "%ld:%ld\n",startIndex+j,uf->array[i][j]);
		}
	}
}