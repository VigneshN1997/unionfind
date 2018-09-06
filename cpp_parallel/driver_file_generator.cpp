#include "generateFiles.cpp"

int main(int argc, char const *argv[])
{
	vector<int> numProcessArr;
	numProcessArr.push_back(2);
	numProcessArr.push_back(4);
	numProcessArr.push_back(8);
	numProcessArr.push_back(16);

	vector<float> queryPercentArr;
	queryPercentArr.push_back(0.2);
	queryPercentArr.push_back(0.5);

	vector<long int> numPointsArr;
	numPointsArr.push_back(1000000);
	numPointsArr.push_back(50000000);
	// numPointsArr.push_back(100000000);
	
	generateQueryFiles(numProcessArr, numPointsArr, queryPercentArr);
	return 0;
}