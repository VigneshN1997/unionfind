#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <cstdlib>
#include <climits>
#include <vector>
#include <random>
#include <unordered_set>
#include <ctime>
#include <algorithm>
#include <sys/stat.h>
using namespace std;

typedef struct
{
	long int id;
	int process_num;
}id_proc;

typedef struct
{
	long int num_elems;
	long int num_elems_per_arr;
	long int** array;
	int* global_arr;
}UnionFind;


typedef struct
{
	int bucketSize;
	long int numPoints;
	long int numQueries;
	double time;
	long int numMessages;
	long int multipleMsgs;
}Result;

