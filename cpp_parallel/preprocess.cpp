#include "preprocess.h"




long int* createArr(int processRank, long int numPointsPerProcess,vector<int>* pointIdMapping)
{
	long int* unionfindDs = (long int*)malloc(numPointsPerProcess*sizeof(long int));
	long int startIndex = (processRank - 1)*numPointsPerProcess;
	long int lastIndex = startIndex + numPointsPerProcess - 1;
	long int i;
	for(i = startIndex; i <= lastIndex; i++)
	{
		(*pointIdMapping)[i - startIndex] = processRank;
		unionfindDs[i - startIndex] = i;
	}
	return unionfindDs;
}

unordered_map<string, long int> getConfig(string configFileName) {
	unordered_map<string, long int> config;
	ifstream configFile(configFileName);
	string line;
	while(getline(configFile, line)) {
		istringstream is_line(line);
		string key;
		if(getline(is_line, key, '='))
		{
			string value;
			if(getline(is_line, value)) {
				stringstream val(value);
				long int x;
				val >> x;
			 	config[key] = x;
			}
		}
	}
	return config;
}

void printUnionFindDs(int processRank, long int* unionFindDs, long int numPointsPerProcess) {
	string file_name = "unionfindDs_"+ to_string(processRank);
	ofstream fp(file_name,ios::out);
	long int startIndex = (processRank - 1)*numPointsPerProcess;
	long int lastIndex = startIndex + numPointsPerProcess - 1;
	long int i;
	for(i = startIndex; i <= lastIndex; i++)
	{
		fp  << i;
		fp << ":";
		fp << unionFindDs[i - startIndex];
		fp << "\n";
	}
	fp.close();
}