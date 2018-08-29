
#include "hashtable.h"

// implement this
int hashingFunction(int num, int size_of_ht)
{
	int hash = word[0] % size_of_ht;
	int c;
	for(int i = 1; i < strlen(word); i++)
	{
		c = word[i];
		hash = (hash*31 + c) % size_of_ht;
	}
	// printf("%s %d\n",word,hash);
	return hash%size_of_ht;
}

// create a new lookup table
HASH_TABLE* createHashTable(int size)
{
	HASH_TABLE* h = (HASH_TABLE*)malloc(sizeof(HASH_TABLE));
	h->entries = 0;
	h->size = size;
	// h->loadFactor = 0;
	// h->freeSlots = size;
	h->ll_headList = (HEAD**)malloc(size*sizeof(HEAD*));
	HEAD** hs = h->ll_headList;
	for(int i = 0; i < size; i++)
	{
		hs[i] = initializeLinkedList();
	}
	return h;
}

// hash keyword into the table
void insertNewEntry(HASH_TABLE* ht, int key, void* data);
{
	ht->entries += 1;
	int index = hashingFunction(key,ht->size);
	HEAD* head_index = (ht->ll_headList) + index;
	insertAtEnd(head_index,data,key);
}

// find a word in the table(if doesnt exist return NULL)

int getIndex(HASH_TABLE* ht, int keyToSearch)
{
	int index = hashingFunction(keyToSearch,ht->size);
	return index;
}

ll_node* findEntry(HASH_TABLE* ht,int keyToSearch)
{
	int index = getIndex(ht,keyToSearch);
	// printf("index:%d\n",index);
	ll_node* n = ((ht->ll_headList)+index)->first;
	if(n == NULL)
	{
		return NULL;
	}
	int node_key = n->key;
	if(keyToSearch == node_key)
	{
		return n;
	}
	else
	{
		n = n->next;
		while(n != NULL)
		{
			node_key = n->key;
			if(keyToSearch == node_key)
			{
				return n;
			}
			n = n->next;
		}
	}
	return NULL;
}