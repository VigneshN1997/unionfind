#include "unionfind.cpp"

typedef struct
{
	long int query; // queryNumber
	long int parent;
}queryParentMapping;

// queryParentMapping: stores (queryNum,x,y) -> id of parent set -> will be used for deferred updates
// unionQueriesSent: (queryNum,original_x, original_y) to  (a,b) (i.e. (x,y) of the current function) -> will be used for path compression
// queriesToBeReplied: (queryNum,original_x, original_y) to processNum (process from which this query came -> store the update in that corresponding updatesDone vector)

typedef struct 
{
	long int original_x; 	// original x for query
	long int original_y; 	// original y for query
	long int query_x;		// current x(the process receiving the query will start traversal with this x,y)
	long int query_y;
}Query;

typedef struct
{
	long int num_elems;
	long int num_elems_per_arr;
	long int** array;
	int* global_arr;
	vector<unordered_map<int, vector<queryParentMapping> > >* updatesDone;
	vector<unordered_map<long int, vector<long int> > >* unionQueriesSent;
	vector<unordered_map<long int, int> >* queriesToBeReplied;
}UnionFind_mod;

UnionFind_mod* init_unionfindmod(long int n,int num_arrays);
void random_initialize1(UnionFind_mod* uf,long int n,int num_arrays);
void unifyOptimized(long int queryNum, Query q, UnionFind_mod* uf, int process_of_x, int process_of_y, long int* num_messages, vector<queryParentMapping> updatesToDo, int queryFromProcess);
void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex, UnionFind_mod* uf);


void addUpdate(long int queryNum,long int parent,int process_of_y,int queryFromProcess,UnionFind_mod* uf);
void addQuerytoSentQuerySet(long int queryNum,Query q,int process_of_query_y,UnionFind_mod* uf);
void addQueryToQueriesToBeReplied(long int queryNum, int processToSendReplyTo,int currProcess,UnionFind_mod* uf);
Query createNewQuery(long int original_x, long int original_y, long int query_x, long int query_y);

void doPathCompressionOfRemainingUpdates(UnionFind_mod* uf);
void sendAndProcessUpdates(vector<queryParentMapping> updatesToDo, UnionFind_mod* uf, int process_of_y);