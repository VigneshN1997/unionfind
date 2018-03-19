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
	vector<map<vector<long int>, vector<long int> > > > unionQueriesSent;
}UnionFind;

UnionFind* init_unionfind(long int n,int num_arrays);
void random_initialize(UnionFind* uf,long int n,int num_arrays);
void printUnionFind(UnionFind* uf);
bool unifyOptimized(long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages);
