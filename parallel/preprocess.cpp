#include "header.h"

vector<long int>* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping)
{
	vector<long int>* unionfindDs = new vector<long int>(numPointsPerProcess);
	long int startIndex = (processRank - 1)*numPointsPerProcess;
	long int lastIndex = startIndex + numPointsPerProcess - 1;
	long int i;
	for(i = startIndex; i <= lastIndex; i++)
	{
		// unionfindDs[a] stores parent of a
		// so initially unionfindDs[a] = a
		(*pointIdMapping)[i - startIndex] = processRank;
		(*unionfindDs)[i - startIndex] = i;
	}
	return unionfindDs;
}