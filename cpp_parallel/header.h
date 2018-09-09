#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <climits>
#include <cstring>

struct sendQuery
{
	long int newQueryX;
	long int newQueryY;
	long int finalParent; // used in path compression algorithm
	int toProcess;
	// int fromProcess;
};

struct returnStruct
{
	struct sendQuery* query;
	bool unionDone;
};

typedef struct sendQuery sendQuery;
typedef struct returnStruct returnStruct;



using namespace std;
