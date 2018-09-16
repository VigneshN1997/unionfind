#include "generateFiles.cpp"

int main(int argc, char const *argv[])
{
	vector<int> numProcessArr;
	numProcessArr.push_back(10);

	vector<float> queryPercentArr;
	queryPercentArr.push_back(0.2);
	queryPercentArr.push_back(0.5);

	vector<long int> numPointsArr;
	numPointsArr.push_back(1000000);
	numPointsArr.push_back(10000000);
	numPointsArr.push_back(20000000);
	numPointsArr.push_back(50000000);
	numPointsArr.push_back(100000000);
	numPointsArr.push_back(200000000);
	
	generateQueryFiles(numProcessArr, numPointsArr, queryPercentArr);
	return 0;
}