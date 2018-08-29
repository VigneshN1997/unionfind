#include <def.h>
struct ll_node
{
	int key;
	void* data;
	struct ll_node* next;
	struct ll_node* prvs;
};

struct head
{
	int count;
	struct ll_node* first;
	struct ll_node* last;
};

typedef struct ll_node ll_node;
typedef struct head HEAD;

HEAD* initializeLinkedList();
void insertInFront(HEAD* h, void* data, int key);
void insertAtEnd(HEAD* h, void* data, int key);
void* deleteFromFront(HEAD* h);
void deleteNode(HEAD* h,ll_node* n);