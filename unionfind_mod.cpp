UnionFind* init_unionfind(long int n,int num_arrays)
{
	UnionFind* uf = (UnionFind*)malloc(sizeof(UnionFind));
	long int num_elems_per_arr = n / num_arrays;
	// uf->array = new vector<vector<int> >(num_arrays,vector<int>(num_elems_per_arr)); 
	// uf->global_arr = new vector<id_proc>(n);
	(uf->array).clear();
	uf->num_elems = num_elems_per_arr*num_arrays;
	uf->num_elems_per_arr = num_elems_per_arr;
	random_initialize(uf,uf->num_elems,num_arrays);
	return uf;
}

void random_initialize(UnionFind* uf,long int n,int num_arrays)
{
	long int i,j,k;
	for(i = 0; i < n; i++)
	{
		id_proc s1;
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
			(uf->global_arr)[k].process_num = procRank;
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
		map<vector<long int>, bool> > mappingOneProcess;
		mappingOneProcess.clear();
		(uf->unionQueriesSent).push_back(mappingOneProcess);
	}
}

vector<queryParentMapping> unifyOptimized(long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages,int queryFromProcess)
{
	long int root_y = y;
	long int startIndex = process_of_y*uf->num_elems_per_arr;
	long int its_parent = uf->array[process_of_y][y - startIndex];
	bool unionDoneInThisProcess = false;
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

			}
		}
		else if(root_y == x)
		{
			unionDoneInThisProcess = true;
			if(queryFromProcess != -1)
			{
				
			}
		}
		else
		{
			*num_messages += 1;
			int queryFromProcess = process_of_y;
			vector<
		}
	}
	else
	{
		if(its_parent < x)
		{

		}
		else
		{

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
	if(queryFromProcess == -1) // if query is from root process return nothing
	{
		vector<queryParentMapping> nullVec;
		nullVec.clear();
		return nullVec;
	}
	else
	{
		return (uf->updatesDone)[process_of_y][]
	}
}	