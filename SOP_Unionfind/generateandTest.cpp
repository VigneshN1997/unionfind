#include "testing.h"

/*
This file is used for generating union find queries and also writing the final union find ds to file
*/

bool exists_test(string file_name)
{
	struct stat buffer;
	return (stat(file_name.c_str(), &buffer) == 0);
}

void generateQueryFiles(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr)
{
	int i,j,k;
	for(i = 0; i < numBucketsArr.size(); i++)
	{
		for(j = 0; j < numPointsArr.size(); j++)
		{
			for(k = 0; k < queryPercentArr.size(); k++)
			{
				long int numQueries = (long int)(queryPercentArr[k]*numPointsArr[j]);
				generateRandomQueries(numQueries,numPointsArr[j],numBucketsArr[i]);
			}
		}
	}
}

void generateRandomQueries(long int numQueries, long int maxNum,int numBuckets)
{

	string file_name = "queryFile_"+to_string(maxNum) + "_" + to_string(numQueries) + "_" + to_string(numBuckets);
	if(exists_test(file_name))
	{
		printf("file %s exists\n",file_name.c_str());
		return;
	}
	ofstream queryFile(file_name,ios::out);
	// FILE* fp = fopen("queryFile_"+ltoa(maxNum) + "_" + ltoa(numQueries) + "_" + itoa(numBuckets),"w");
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
		queryFile << x;
		queryFile << " ";
		queryFile << y;
		queryFile << "\n";
		// fprintf(fp, "%ld %ld\n",(long int)x,(long int)y);
	}
	queryFile.close();
}

void printUnionfindToFileMap(UnionFind* uf,long int numQueries)
{
	int num_processes = (uf->array).size();
	string file_name = "unionfindDs_"+to_string(uf->num_elems) + "_" + to_string(numQueries) + "_" + to_string(num_processes);
	ofstream fp(file_name,ios::out);
	int i;
	map<long int,long int>::iterator j;
	for(i = 0; i < num_processes; i++)
	{
		fp << "Points with process ";
		fp << i;
		fp << "\n";
		fp << "Point |Parent of point\n";
		for(j = uf->array[i].begin(); j != uf->array[i].end(); j++)
		{
			fp << j->first;
			fp << ":";
			fp << j->second;
			fp << "\n";
		}
	}
	fp.close();
}


void printUnionfindToFileVector(UnionFind_mod* uf,long int numQueries)
{
	int num_processes = uf->num_elems / uf->num_elems_per_arr;
	string file_name = "unionfindDsMod_"+to_string(uf->num_elems) + "_" + to_string(numQueries) + "_" + to_string(num_processes);
	ofstream fp(file_name,ios::out);
	// FILE* fp = fopen(,"w");
	int i;
	long int j;
	long int startIndex;
	for(i = 0; i < num_processes; i++)
	{
		startIndex = i*uf->num_elems_per_arr;
		fp << "Points with process ";
		fp << i;
		fp << "\n";
		fp << "Point |Parent of point\n";
		for(j = 0; j < uf->num_elems_per_arr; j++)
		{
			fp << startIndex+j;
			fp << ":";
			fp << uf->array[i][j];
			fp << "\n";
		}
	}
	fp.close();
}

void setTesting(UnionFind_mod* uf)
{
	vector<bool> setFound;
}

