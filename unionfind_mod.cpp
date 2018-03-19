
// this function will initialize the unionfind data structure
// by dividing the points among the arrays ( the division of points is not random)
UnionFind* init_unionfind(long int n,int num_arrays)
{
	UnionFind* uf = (UnionFind*)malloc(sizeof(UnionFind));
	long int num_elems_per_arr = n / num_arrays;
	(uf->array).clear();
	(uf->global_arr).clear();
	uf->num_elems = num_elems_per_arr*num_arrays;
	uf->num_elems_per_arr = num_elems_per_arr;
	random_initialize(uf,uf->num_elems,num_arrays);
	return uf;
}


// for initializing the arrays of unionfind ds
void random_initialize(UnionFind* uf,long int n,int num_arrays)
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

	// arrays initialized here
	while(higher <= uf->num_elems)
	{
		vector<long int> procArr;
		procArr.clear();
		for(k = lower; k < higher; k++)
		{
			procArr.push_back(k);
			(uf->global_arr)[k] = procRank;
		}
		(uf->array).push_back(procArr);
		lower = lower + uf->num_elems_per_arr;
		higher = higher + uf->num_elems_per_arr;
		procRank++;
	}

	// initializing the deferred updates data structure
	for(i = 0; i < (uf->array).size(); i++)
	{
		unordered_map<int, vector<queryParentMapping> > m;
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
		unordered_map<vector<long int>, vector<long int> > > mappingOneProcess;
		mappingOneProcess.clear();
		(uf->unionQueriesSent).push_back(mappingOneProcess);
	}
}

vector<queryParentMapping> unifyOptimized(long int queryNum,long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages,int queryFromProcess)
{
	long int root_y = y; // root_y will have the local root/ global root
	long int startIndex = process_of_y*uf->num_elems_per_arr;	// for array index manipulation
	long int its_parent = uf->array[process_of_y][y - startIndex]; // parent of y
	bool unionDoneInThisProcess = false;						// if union operation is finished in this process
	vector<queryParentMapping> updatesToDo;						// vector that will store the deferred updates returned by the process the current process forwards the query to
	updatesToDo.clear();

	// go up the tree till you get to local / global root
	while(root_y < its_parent && uf->global_arr[its_parent] == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y - startIndex];
	}

	if(root_y == its_parent)
	{
		if(root_y < x)
		{
			uf->array[process_of_y][root_y - startIndex] = x;
			printf("set parent of %ld to %ld\n",root_y,x);
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				queryParentMapping q;
				q.query.clear();
				q.query.push_back(queryNum);
				q.query.push_back(x);
				q.query.push_back(y);
				q.parent = x;
				(uf->updatesDone)[process_of_y][queryFromProcess].push_back(query);
			}
		}
		else if(root_y == x)
		{
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				queryParentMapping q;
				q.query.clear();
				q.query.push_back(queryNum);
				q.query.push_back(x);
				q.query.push_back(y);
				q.parent = x;
				(uf->updatesDone)[process_of_y][queryFromProcess].push_back(query);
			}
		}
		else
		{
			*num_messages += 1;
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,root_y,x,uf,process_of_y,process_of_x,num_messages,process_of_y);
		}
	}
	else
	{
		if(its_parent < x)
		{
			*num_messages += 1;
			vector<long int> sentQuery;
			vector<long int> currXY;
			sentQuery.clear();
			currXY.clear();
			sentQuery.push_back(queryNum);
			sentQuery.push_back(x);
			sentQuery.push_back(its_parent);

			currXY.push_back(x);
			currXY.push_back(y);

			(uf->unionQueriesSent)[process_of_y][sentQuery] = currXY;
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,x,its_parent,uf,process_of_x,uf->global_arr[its_parent],num_messages,process_of_y);
		}
		else
		{
			*num_messages += 1;
			vector<long int> sentQuery;
			vector<long int> currXY;
			sentQuery.clear();
			currXY.clear();
			sentQuery.push_back(queryNum);
			sentQuery.push_back(its_parent);
			sentQuery.push_back(x);

			currXY.push_back(x);
			currXY.push_back(y);

			(uf->unionQueriesSent)[process_of_y][sentQuery] = currXY;
			vector<queryParentMapping> updatesToDo = unifyOptimized(queryNum,its_parent,x,uf,uf->global_arr[its_parent],process_of_x,num_messages,process_of_y);
		}
	}
	if(unionDoneInThisProcess)
	{
		long int node = y;
		while(node < x && uf->global_arr[node] == process_of_y)
		{
			int temp = uf->array[process_of_y][node - startIndex];
			if(uf->array[process_of_y][node - startIndex] != x)
			{
				uf->array[process_of_y][node - startIndex] = x;
				printf("updated parent of %ld to %ld\n",node,x);
			}
			node = temp;
		}
	}

	if(updatesToDo.size() > 0)
	{
		// write this
		vector<queryParentMapping>::iterator itr;
		for(itr = updatesToDo.begin(); itr != updatesToDo.end(); itr++)
		{
			vector<long int> query = itr->query;
			long int parent = itr->parent;
			unordered_map<vector<long int>, vector<long int> > >::const_iterator map_itr = (uf->unionQueriesSent)[process_of_y].find(query);
			if(map_itr != (uf->unionQueriesSent)[process_of_y].end())
			{
				vector<long int> currXYforQuery = map_itr->second;
				long int node = currXYforQuery[1]; // y
				while(node < parent && uf->global_arr[node] == process_of_y)
				{
					long int temp = uf->array[process_of_y][node - startIndex];
					if(uf->array[process_of_y][node - startIndex] != parent)
					{
						uf->array[process_of_y][node - startIndex] = parent;
					}
					node = temp;
				}
				// erase
				(uf->unionQueriesSent)[process_of_y].erase(map_itr);
			}
		}
	}
	if(queryFromProcess == -1) // if query is from root process return nothing
	{
		vector<queryParentMapping> nullVec;
		nullVec.clear();
		return nullVec;
	}
	vector<queryParentMapping> updateVec = (uf->updatesDone)[process_of_y][queryFromProcess]; 
	(uf->updatesDone)[process_of_y][queryFromProcess].clear(); // check this
	return updateVec;
}	