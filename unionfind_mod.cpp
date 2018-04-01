#include "unionfind_mod.h"

UnionFind_mod* init_unionfindmod(long int n,int num_arrays)
{
	UnionFind_mod* uf = (UnionFind_mod*)malloc(sizeof(UnionFind_mod));
	long int num_elems_per_arr = n / num_arrays;
	// uf->array = new vector<vector<long int> >(num_arrays,vector<int>(num_elems_per_arr)); 
	// (uf->array).clear();
	uf->num_elems = num_elems_per_arr*num_arrays;
	uf->num_elems_per_arr = num_elems_per_arr;
	uf->array = (long int**)malloc(num_arrays*sizeof(long int*));
	uf->global_arr = (int*)malloc(uf->num_elems*sizeof(int));
	// uf->global_arr = new vector<id_proc>(uf->num_elems);
	random_initialize1(uf,uf->num_elems,num_arrays);
	return uf;
}

void random_initialize1(UnionFind_mod* uf,long int n,int num_arrays)
{
	long int i,j,k;
	// (uf->global_arr).resize(n);
	for(i = 0; i < n; i++)
	{
		(uf->global_arr)[i] = -1;
	}
	// printf("\n");
	long int lower = 0;
	long int higher = uf->num_elems_per_arr;
	int procRank = 0;
	while(higher <= uf->num_elems)
	{
		(uf->array)[procRank] = (long int*)malloc(uf->num_elems_per_arr*sizeof(long int));
		for(long int k = lower; k < higher; k++)
		{
			(uf->array)[procRank][k - lower] = k;
			(uf->global_arr)[k] = procRank;
		}
		lower = lower + uf->num_elems_per_arr;
		higher = higher + uf->num_elems_per_arr;
		procRank++;
	}

	uf->updatesDone = new vector<map<int, vector<queryParentMapping> > >(num_arrays);
	uf->unionQueriesSent = new vector<map<vector<long int>, vector<long int> > >(num_arrays);
	uf->queriesToBeReplied = new vector<map<vector<long int>, int> >(num_arrays);
	for(i = 0; i < num_arrays; i++)
	{
		map<int, vector<queryParentMapping> > m;
		m.clear();
		for(j = 0; j < num_arrays; j++)
		{
			// if(j == i)
			// {
			// 	continue;
			// }
			vector<queryParentMapping> vec;
			vec.clear();
			m[j] = vec;
		}
		(*(uf->updatesDone)).at(i) = m;
	}
	for(i = 0; i < num_arrays; i++)
	{
		map<vector<long int>, vector<long int> > mappingOneProcess;
		// mappingOneProcess.clear();
		(*(uf->unionQueriesSent)).at(i) = mappingOneProcess;
	}
	for(i = 0; i < num_arrays; i++)
	{
		map<vector<long int>, int > mappingOneProcess;
		// mappingOneProcess.clear();
		(*(uf->queriesToBeReplied)).at(i) = mappingOneProcess;
	}
}

void addUpdate(long int queryNum,long int original_x,long int original_y,long int parent,int process_of_y,int queryFromProcess,UnionFind_mod* uf)
{
	queryParentMapping q;
	q.query.clear();
	q.query.push_back(queryNum);
	q.query.push_back(original_x);
	q.query.push_back(original_y);
	q.parent = parent;
	(*(uf->updatesDone))[process_of_y][queryFromProcess].push_back(q); // add it to updates arr for deferred sending of update to process "queryFromProcess"
}

void addQuerytoSentQuerySet(long int queryNum,Query q,int process_of_query_y,UnionFind_mod* uf)
{
	vector<long int> originalQuery;
	vector<long int> currXY;
	originalQuery.clear();
	currXY.clear();
	originalQuery.push_back(queryNum);
	originalQuery.push_back(q.original_x);
	originalQuery.push_back(q.original_y);

	currXY.push_back(q.query_x);
	currXY.push_back(q.query_y);

	(*(uf->unionQueriesSent))[process_of_query_y][originalQuery] = currXY;
}

void addQueryToQueriesToBeReplied(long int queryNum, long int original_x, long int original_y, int processToSendReplyTo,int currProcess,UnionFind_mod* uf)
{
	vector<long int> originalQuery;
	originalQuery.clear();	
	originalQuery.push_back(queryNum);
	originalQuery.push_back(original_x);
	originalQuery.push_back(original_y);
	(*(uf->queriesToBeReplied))[currProcess][originalQuery] = processToSendReplyTo;
}

void doPathCompression(long int startNode,long int parent,int process_of_y,long int startIndex, UnionFind_mod* uf)
{
	long int node = startNode;
	while(node < parent && uf->global_arr[node] == process_of_y)
	{
		long int temp = uf->array[process_of_y][node - startIndex];
		if(uf->array[process_of_y][node - startIndex] != parent)
		{
			uf->array[process_of_y][node - startIndex] = parent;
			printf("updated parent of %ld to %ld in process %d\n",node,parent,process_of_y);
		}
		node = temp;
	}
}

// query sent will be identified by:(original x, original y), queryNum -> the (a,b) of this process (i.e. with what x,y this process started), 
// queries to send reply to: (original x, original y), queryNum -> process to send reply to // this will tell to which updatesDone arr does the update needs to get added for further forwarding
// updatesDone will be identified by: vector of (original x, original y), queryNum , parent set for each process stored by each process


Query createNewQuery(long int original_x, long int original_y, long int query_x, long int query_y)
{
	Query q;
	q.original_x = original_x;
	q.original_y = original_y;
	q.query_x = query_x;
	q.query_y = query_y;
	return q;
}

void unifyOptimized(long int queryNum, Query q, UnionFind_mod* uf, int process_of_x, int process_of_y, long int* num_messages, vector<queryParentMapping> updatesToDo, int queryFromProcess)
{
	long int startIndex = process_of_y*uf->num_elems_per_arr;
	if(queryFromProcess != -1)
	{
		// query to be replied
		addQueryToQueriesToBeReplied(queryNum,q.original_x,q.original_y,queryFromProcess,process_of_y,uf);
	}
	if(updatesToDo.size() > 0)
	{
		vector<queryParentMapping>::iterator itr;
		for(itr = updatesToDo.begin(); itr != updatesToDo.end(); itr++)
		{
			vector<long int> query = itr->query;
			long int parent = itr->parent;
			map<vector<long int>, vector<long int> >::iterator map_itr = (*(uf->unionQueriesSent))[process_of_y].find(query);
			if(map_itr != (*(uf->unionQueriesSent))[process_of_y].end())
			{
				vector<long int> currXYforQuery = map_itr->second;
				doPathCompression(currXYforQuery[1],parent,process_of_y,startIndex,uf);
				(*(uf->unionQueriesSent))[process_of_y].erase(map_itr);	
			}
			map<vector<long int>, int>::iterator reply_itr = (*(uf->queriesToBeReplied))[process_of_y].find(query);
			if(reply_itr != (*(uf->queriesToBeReplied))[process_of_y].end())
			{
				addUpdate(query[0],query[1],query[2],parent,process_of_y,reply_itr->second,uf);
				(*(uf->queriesToBeReplied))[process_of_y].erase(reply_itr);
			}
		}
	}
	long int root_y = q.query_y;
	long int its_parent = uf->array[process_of_y][root_y - startIndex];
	bool unionDoneInThisProcess = false;
	
	while(root_y < its_parent && uf->global_arr[its_parent] == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y - startIndex];
	}
	if(root_y == its_parent)
	{
		// point root_y to x
		if(root_y < q.query_x)
		{
			uf->array[process_of_y][root_y - startIndex] = q.query_x;
			printf("set parent of %ld to %ld\n",root_y,q.query_x);
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				addUpdate(queryNum,q.original_x,q.original_y,q.query_x,process_of_y,queryFromProcess,uf);
				vector<long int> findQuery;
				findQuery.push_back(queryNum);
				findQuery.push_back(q.original_x);
				findQuery.push_back(q.original_y);
				map<vector<long int>, int>::iterator quer_itr = (*(uf->queriesToBeReplied))[process_of_y].find(findQuery);
				if(quer_itr != (*(uf->queriesToBeReplied))[process_of_y].end())
				{
					(*(uf->queriesToBeReplied))[process_of_y].erase(quer_itr);
				} 
			}
		}

		// x and y in same set
		else if(root_y == q.query_x)
		{
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				addUpdate(queryNum,q.original_x,q.original_y,q.query_x,process_of_y,queryFromProcess,uf);
				vector<long int> findQuery;
				findQuery.push_back(queryNum);
				findQuery.push_back(q.original_x);
				findQuery.push_back(q.original_y);
				map<vector<long int>, int>::iterator quer_itr = (*(uf->queriesToBeReplied))[process_of_y].find(findQuery);
				if(quer_itr != (*(uf->queriesToBeReplied))[process_of_y].end())
				{
					(*(uf->queriesToBeReplied))[process_of_y].erase(quer_itr);
				}
			}
		}
		// root_y > x (then root_y cannot point to x so forward the query)
		else
		{
			*num_messages += 1;
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",q.query_x,q.query_y,root_y,q.query_x,process_of_x,process_of_y);
			vector<queryParentMapping> updateVec = (*(uf->updatesDone))[process_of_y][process_of_x]; 
			(*(uf->updatesDone))[process_of_y][process_of_x].clear();
			Query fwdQuery = createNewQuery(q.original_x,q.original_y,root_y,q.query_x);
			addQuerytoSentQuerySet(queryNum,q,process_of_y,uf);
			unifyOptimized(queryNum,fwdQuery,uf,process_of_y,process_of_x,num_messages,updateVec,process_of_y);
		}
	}
	else
	{
		// if parent of root_y is < x 
		if(its_parent < q.query_x)
		{
			*num_messages += 1;
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",q.query_x,q.query_y,q.query_x,its_parent,uf->global_arr[its_parent],process_of_y);
			vector<queryParentMapping> updateVec = (*(uf->updatesDone))[process_of_y][uf->global_arr[its_parent]]; 
			(*(uf->updatesDone))[process_of_y][uf->global_arr[its_parent]].clear(); // check this
			Query fwdQuery = createNewQuery(q.original_x,q.original_y,q.query_x,its_parent);
			addQuerytoSentQuerySet(queryNum,q,process_of_y,uf);
			unifyOptimized(queryNum,fwdQuery,uf,process_of_x,uf->global_arr[its_parent],num_messages,updateVec,process_of_y);
		}
		else
		{
			*num_messages += 1;
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",q.query_x,q.query_y,its_parent,q.query_x,process_of_x,process_of_y);
			vector<queryParentMapping> updateVec = (*(uf->updatesDone))[process_of_y][process_of_x]; 
			(*(uf->updatesDone))[process_of_y][process_of_x].clear(); // check this
			Query fwdQuery = createNewQuery(q.original_x,q.original_y,its_parent,q.query_x);
			addQuerytoSentQuerySet(queryNum,q,process_of_y,uf);
			unifyOptimized(queryNum,fwdQuery,uf,uf->global_arr[its_parent],process_of_x,num_messages,updateVec,process_of_y);
		}
	}
	if(unionDoneInThisProcess)
	{
		doPathCompression(q.query_y,q.query_x,process_of_y,startIndex,uf);
	}
}

void doPathCompressionOfRemainingUpdates(UnionFind_mod* uf)
{
	bool complete = true;
	int i, j;
	int num_processes = uf->num_elems / uf->num_elems_per_arr;
	while(true)
	{
		for(i = 0; i < num_processes; i++)
		{
			for(j = 0; j < num_processes; j++)
			{
				// if(j == i)
				// {
				// 	continue;
				// }
				if((*(uf->updatesDone))[i][j].size() > 0)
				{
					complete = false;
					vector<queryParentMapping> updateVec = (*(uf->updatesDone))[i][j]; 
					(*(uf->updatesDone))[i][j].clear();
					sendAndProcessUpdates(updateVec,uf,j);
				}
			}
		}
		if(complete)
		{
			break;
		}
		complete = true;
	}
}

void sendAndProcessUpdates(vector<queryParentMapping> updatesToDo, UnionFind_mod* uf, int process_of_y)
{
	long int startIndex = process_of_y*uf->num_elems_per_arr;
	vector<queryParentMapping>::iterator itr;
	for(itr = updatesToDo.begin(); itr != updatesToDo.end(); itr++)
	{
		vector<long int> query = itr->query;
		long int parent = itr->parent;
		map<vector<long int>, vector<long int> >::iterator map_itr = (*(uf->unionQueriesSent))[process_of_y].find(query);
		if(map_itr != (*(uf->unionQueriesSent))[process_of_y].end())
		{
			vector<long int> currXYforQuery = map_itr->second;
			doPathCompression(currXYforQuery[1],parent,process_of_y,startIndex,uf);
			(*(uf->unionQueriesSent))[process_of_y].erase(map_itr);	
		}
		map<vector<long int>, int>::iterator reply_itr = (*(uf->queriesToBeReplied))[process_of_y].find(query);
		if(reply_itr != (*(uf->queriesToBeReplied))[process_of_y].end())
		{
			addUpdate(query[0],query[1],query[2],parent,process_of_y,reply_itr->second,uf);
			(*(uf->queriesToBeReplied))[process_of_y].erase(reply_itr);
		}
	}
}