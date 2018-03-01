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
	vector<map<long int,long int> > array;
	vector<id_proc> global_arr;
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


UnionFind* init_unionfind(long int n,int num_arrays);
void random_initialize(UnionFind* uf,long int n,int num_arrays);
void printUnionFind(UnionFind* uf);
bool unify(long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages);
vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
void unifyPathCompression(long int x, long int y, UnionFind* uf, int process_of_x, int process_of_y, long int* num_messages,long int* final_parent);