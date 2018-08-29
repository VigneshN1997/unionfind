#include "linked_list.c"

struct HASH_TABLE
{
	int entries;
	int size;
	HEAD** ll_headList;
};

typedef struct HASH_TABLE HASH_TABLE;

HASH_TABLE* createHashTable(int size);
int hashingFunction(int num, int size_of_ht);
void insertNewEntry(HASH_TABLE* ht, int key, void* data);
ll_node* findEntry(HASH_TABLE* ht,int key);
int getIndex(HASH_TABLE* ht, int keyToSearch);