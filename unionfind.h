#include "def.h"

UnionFind* init_unionfind(long int n,int num_arrays);
void random_initialize(UnionFind* uf,long int n,int num_arrays);
void printUnionFind(UnionFind* uf);
bool unify(long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages);
vector<Result*> do_statistical_analysis(vector <int> numBucketsArr,vector <long int> numPointsArr,vector<float> queryPercentArr);
void unifyPathCompression(long int x, long int y, UnionFind* uf, int process_of_x, int process_of_y, long int* num_messages,long int* final_parent);
void unifyPartialPathCompression(long int x, long int y, UnionFind* uf, int process_of_x, int process_of_y, long int* num_messages,long int* final_parent);