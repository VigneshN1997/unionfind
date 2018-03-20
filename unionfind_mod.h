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
#include <bits/stdc++.h>
using namespace std;


typedef struct
{
	vector<long int> query; // vector of size 3(First 2 elements contain query, third element contains parent set)
	long int parent;
}queryParentMapping;


// queryParentMapping: stores (queryNum,x,y) -> id of parent set
// unionQueriesSent: vector of size 4: (queryNum,x,y -> sent) to  (x,y) -> of the current function
typedef struct
{
	long int num_elems;
	long int num_elems_per_arr;
	vector<vector<long int> > array;
	vector<int> global_arr;
	vector<map<int, vector<queryParentMapping> > > updatesDone;
	vector<map<vector<long int>, vector<long int> > > unionQueriesSent;
}UnionFind;

UnionFind* init_unionfind(long int n,int num_arrays);
void random_initialize(UnionFind* uf,long int n,int num_arrays);
void addUpdate(long int queryNum,long int x,long int y,int process_of_y,int queryFromProcess,UnionFind* uf);
void addQuerytoSentQuerySet(long int queryNum,long int new_x,long int new_y,long int curr_x,long int curr_y,int process_of_curr_y,UnionFind* uf);
vector<queryParentMapping> unifyOptimized(long int queryNum,long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages,int queryFromProcess);
void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex, UnionFind* uf);