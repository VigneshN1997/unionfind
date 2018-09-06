
/*
This file is used for generating union find queries and also writing the final union find ds to file
*/
#include "generateFiles.h"

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
				long int numPointsPerProcess = (numPointsArr[j])/(numBucketsArr[i]);

				generateRandomQueries(numQueries,numPointsPerProcess*numBucketsArr[i],numBucketsArr[i]);
			}
		}
	}
}

void generateRandomQueries(long int numQueries, long int maxNum,int numBuckets)
{
	long int num_elems_per_arr = maxNum / numBuckets;
	maxNum = num_elems_per_arr * numBuckets;
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