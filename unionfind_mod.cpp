#include "unionfind_mod.h"

UnionFind_mod* init_unionfindmod(long int n,int num_arrays)
{
	UnionFind_mod* uf = (UnionFind_mod*)malloc(sizeof(UnionFind_mod));
	long int num_elems_per_arr = n / num_arrays;
	// uf->array = new vector<vector<int> >(num_arrays,vector<int>(num_elems_per_arr)); 
	// uf->global_arr = new vector<id_proc>(n);
	(uf->array).clear();
	uf->num_elems = num_elems_per_arr*num_arrays;
	uf->num_elems_per_arr = num_elems_per_arr;
	random_initialize(uf,uf->num_elems,num_arrays);
	return uf;
}

void random_initialize1(UnionFind_mod* uf,long int n,int num_arrays)
{
	long int i,j,k;
	for(i = 0; i < n; i++)
	{
		(uf->global_arr).push_back(-1);
	}
	// printf("\n");
	long int lower = 0;
	long int higher = uf->num_elems_per_arr;
	int procRank = 0;
	while(higher <= uf->num_elems)
	{
		vector<long int> procArr;
		procArr.clear();
		for(long int k = lower; k < higher; k++)
		{
			procArr.push_back(k);
			(uf->global_arr)[k] = procRank;
		}
		(uf->array).push_back(procArr);
		lower = lower + uf->num_elems_per_arr;
		higher = higher + uf->num_elems_per_arr;
		procRank++;
	}
	for(i = 0; i < (uf->array).size(); i++)
	{
		map<int, vector<queryParentMapping> > m;
		m.clear();
		for(j = 0; j < (uf->array).size(); j++)
		{
			if(j == i)
			{
				continue;
			}
			vector<queryParentMapping> vec;
			vec.clear();
			m[j] = vec;
		}
		(uf->updatesDone).push_back(m);
	}
	for(i = 0; i < (uf->array).size(); i++)
	{
		map<vector<long int>, vector<long int> > mappingOneProcess;
		// mappingOneProcess.clear();
		(uf->unionQueriesSent).push_back(mappingOneProcess);
	}
}

void addUpdate(long int queryNum,long int x,long int y,int process_of_y,int queryFromProcess,UnionFind_mod* uf)
{
	queryParentMapping q;
	q.query.clear();
	q.query.push_back(queryNum);
	q.query.push_back(x);
	q.query.push_back(y);
	q.parent = x;
	(uf->updatesDone)[process_of_y][queryFromProcess].push_back(q);
}

void addQuerytoSentQuerySet(long int queryNum,long int new_x,long int new_y,long int curr_x,long int curr_y,int process_of_curr_y,UnionFind_mod* uf)
{
	vector<long int> sentQuery;
	vector<long int> currXY;
	sentQuery.clear();
	currXY.clear();
	sentQuery.push_back(queryNum);
	sentQuery.push_back(new_x);
	sentQuery.push_back(new_y);

	currXY.push_back(curr_x);
	currXY.push_back(curr_y);

	(uf->unionQueriesSent)[process_of_curr_y][sentQuery] = currXY;
}

vector<queryParentMapping> unifyOptimized(long int queryNum,long int x, long int y, UnionFind_mod* uf,int process_of_x,int process_of_y,long int* num_messages,int queryFromProcess)
{
	long int root_y = y;
	long int startIndex = process_of_y*uf->num_elems_per_arr;
	long int its_parent = uf->array[process_of_y][y - startIndex];
	bool unionDoneInThisProcess = false;
	vector<queryParentMapping> updatesToDo;
	updatesToDo.clear();

	// traversing up the tree
	while(root_y < its_parent && uf->global_arr[its_parent] == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y - startIndex];
	}

	// if root_y is global root
	if(root_y == its_parent)
	{
		// point root_y to x
		if(root_y < x)
		{
			uf->array[process_of_y][root_y - startIndex] = x;
			printf("set parent of %ld to %ld\n",root_y,x);
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				addUpdate(queryNum,x,y,process_of_y,queryFromProcess,uf);
			}
		}

		// x and y in same set
		else if(root_y == x)
		{
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				addUpdate(queryNum,x,y,process_of_y,queryFromProcess,uf);
			}
		}
		// root_y > x (then root_y cannot point to x so forward the query)
		else
		{
			*num_messages += 1;
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",x,y,root_y,x,process_of_x,process_of_y);
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,root_y,x,uf,process_of_y,process_of_x,num_messages,process_of_y);
		}
	}
	// root_y is a local root
	else
	{
		// if parent of root_y is < x 
		if(its_parent < x)
		{
			*num_messages += 1;
			addQuerytoSentQuerySet(queryNum,x,its_parent,x,y,process_of_y,uf);
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",x,y,x,its_parent,uf->global_arr[its_parent],process_of_y);
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,x,its_parent,uf,process_of_x,uf->global_arr[its_parent],num_messages,process_of_y);
		}
		else
		{
			*num_messages += 1;
			addQuerytoSentQuerySet(queryNum,its_parent,x,x,y,process_of_y,uf);
			printf("query(%ld,%ld)=>(%ld,%ld) forwarded to %d from %d\n",x,y,its_parent,x,process_of_x,process_of_y);
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,its_parent,x,uf,uf->global_arr[its_parent],process_of_x,num_messages,process_of_y);
		}
	}
	// if union is done in this process then do path compression for this process
	if(unionDoneInThisProcess)
	{
		doPathCompression(y,x,process_of_y,startIndex,uf);
	}

	if(updatesToDo.size() > 0)
	{
		// write this
		vector<queryParentMapping>::iterator itr;
		for(itr = updatesToDo.begin(); itr != updatesToDo.end(); itr++)
		{
			vector<long int> query = itr->query;
			long int parent = itr->parent;
			map<vector<long int>, vector<long int> >::const_iterator map_itr = (uf->unionQueriesSent)[process_of_y].find(query);
			if(map_itr != (uf->unionQueriesSent)[process_of_y].end())
			{
				vector<long int> currXYforQuery = map_itr->second;
				doPathCompression(currXYforQuery[1],parent,process_of_y,startIndex,uf);
			}
		}
	}
	if(queryFromProcess == -1) // if query is from driver process return nothing
	{
		vector<queryParentMapping> nullVec;
		nullVec.clear();
		return nullVec;
	}
	vector<queryParentMapping> updateVec = (uf->updatesDone)[process_of_y][queryFromProcess]; 
	(uf->updatesDone)[process_of_y][queryFromProcess].clear(); // check this
	return updateVec;
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