#include "header.h"

vector<long int> createArr(int processRank, long int numPointsPerProcess,vector<long int>* pointIdMapping)
{
	vector<long int> unionfindDs;
	unionfindDs.reserve(numPointsPerProcess);
	long int startIndex = processRank*numPointsPerProcess;
	long int lastIndex = startIndex + numPointsPerProcess - 1;
	long int i;
	for(i = startIndex; i <= lastIndex; i++)
	{
		// unionfindDs[a] stores parent of a
		// so initially unionfindDs[a] = a
		(*pointIdMapping)[i] = processRank;
		unionfindDs[i - startIndex] = i;
	}
	return unionfindDs;
}