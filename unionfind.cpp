#include "unionfind.h"

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
	long int i;
	for(i = 0; i < n; i++)
	{
		id_proc s1;
		s1.id = i;
		s1.process_num = -1;
		(uf->global_arr).push_back(s1);
	}
	// printf("\n");
	long int lower = 0;
	long int higher = uf->num_elems_per_arr;
	int j = 0;
	while(higher <= uf->num_elems)
	{
		map<long int,long int> map1;
		for(long int k = lower; k < higher; k++)
		{
			map1[k] = k;
			(uf->global_arr)[k].process_num = j;
		}
		(uf->array).push_back(map1);
		lower = lower + uf->num_elems_per_arr;
		higher = higher + uf->num_elems_per_arr;
		j++;
	}
}

void printUnionFind(UnionFind* uf)
{
	long int i;
	for(i = 0; i < uf->num_elems; i++)
	{
		printf("id:%ld process_num:%d\n",(uf->global_arr[i]).id,(uf->global_arr[i]).process_num);
	}
	for(i = 0; i < (uf->array).size(); i++)
	{
		map<long int,long int>::iterator itr = (uf->array)[i].begin();
		printf("array %ld\n",i);
		while(itr != (uf->array)[i].end())
		{
			printf("id:%ld parent:%ld\n",itr->first,itr->second);
			itr++;
		}
		printf("\n");
	}
}
// this unify is done in process_of_y  (i.e. query is sent to process containing y)
bool unify(long int x, long int y, UnionFind* uf,int process_of_x,int process_of_y,long int* num_messages)
{
	long int root_y = y; // can be local / global root
	long int its_parent = uf->array[process_of_y][root_y];
	while(root_y < its_parent && uf->global_arr[its_parent].process_num == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y];
	}
	if(root_y == its_parent) // root of y is globla root
	{
		if(root_y < x)
		{
			uf->array[process_of_y][root_y] = x;
			return false;
		}
		else if(root_y == x)
		{
			return true;
		}
		else
		{
			*num_messages = *num_messages + 1;
			return unify(root_y,x,uf,process_of_y,process_of_x,num_messages);
			
		}
	}
	else
	{
		if(its_parent < x)
		{
			*num_messages = *num_messages + 1;
			return unify(x,its_parent,uf,process_of_x,uf->global_arr[its_parent].process_num,num_messages);
		}
		else
		{
			*num_messages = *num_messages + 1;
			return unify(its_parent,x,uf,uf->global_arr[its_parent].process_num,process_of_x,num_messages);
		}
	}
}

// simple path compression 
// parent of only local root will be updated(i.e. path compression will be done only on of the local root)
void unifyPartialPathCompression(long int x, long int y, UnionFind* uf, int process_of_x, int process_of_y, long int* num_messages,long int* final_parent)
{
	long int root_y = y;
	long int its_parent = uf->array[process_of_y][root_y];
	while(root_y < its_parent && uf->global_arr[its_parent].process_num == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y];
	}
	if(root_y == its_parent)
	{
		if(root_y < x)
		{
			uf->array[process_of_y][root_y] = x;
			*final_parent =  x;// path compression
			return;
		}
		else if(root_y == x)
		{
			*final_parent = x;
			return;
		}
		else
		{
			*num_messages = * num_messages + 1;
			unifyPartialPathCompression(root_y,x,uf,process_of_y,process_of_x,num_messages,final_parent);
		}
	}
	else
	{
		if(its_parent < x)
		{
			*num_messages = *num_messages + 1;
			unifyPartialPathCompression(x,its_parent,uf,process_of_x,uf->global_arr[its_parent].process_num,num_messages,final_parent);
		}
		else
		{
			*num_messages = *num_messages + 1;
			unifyPartialPathCompression(its_parent,x,uf,uf->global_arr[its_parent].process_num,process_of_x,num_messages,final_parent);
		}
		if(its_parent < *final_parent)
		{	
			uf->array[process_of_y][its_parent] = *final_parent;
		}
	}
}

void unifyPathCompression(long int x, long int y, UnionFind* uf, int process_of_x, int process_of_y, long int* num_messages,long int* final_parent)
{
	long int root_y = y;
	long int its_parent = uf->array[process_of_y][root_y];
	while(root_y < its_parent && uf->global_arr[its_parent].process_num == process_of_y)
	{
		root_y = its_parent;
		its_parent = uf->array[process_of_y][root_y];
	}
	if(root_y == its_parent)
	{
		if(root_y < x)
		{
			uf->array[process_of_y][root_y] = x;
			printf("set parent of %ld to %ld\n",root_y,x);
			*final_parent =  x;// path compression
		}
		else if(root_y == x)
		{
			*final_parent = x;
		}
		else
		{
			*num_messages = * num_messages + 1;
			unifyPathCompression(root_y,x,uf,process_of_y,process_of_x,num_messages,final_parent);
		}
	}
	else
	{
		if(its_parent < x)
		{
			*num_messages = *num_messages + 1;
			unifyPathCompression(x,its_parent,uf,process_of_x,uf->global_arr[its_parent].process_num,num_messages,final_parent);
		}
		else
		{
			*num_messages = *num_messages + 1;
			unifyPathCompression(its_parent,x,uf,uf->global_arr[its_parent].process_num,process_of_x,num_messages,final_parent);
		}
	}
	long int node = y;
	while(node < *final_parent && uf->global_arr[node].process_num == process_of_y && *final_parent != -1)
	{
		int temp = uf->array[process_of_y][node];
		if(uf->array[process_of_y][node] != *final_parent)
		{	
			uf->array[process_of_y][node] = *final_parent;
			printf("updated parent of %ld to %ld\n",node,*final_parent);
		}
		node = temp;		
	}
}

