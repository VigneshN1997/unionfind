// ID: 2015A7PS0355P
// Name: Vignesh Nanda Kumar
#include "linked_list.h"

HEAD* initializeLinkedList()
{
	HEAD* h = (HEAD*)malloc(sizeof(HEAD));
	h->count = 0;
	h->first = NULL;
	h->last = NULL;
	return h;
}

void insertInFront(HEAD* h, void* data. int key)
{
	ll_node* n = (ll_node*)malloc(sizeof(ll_node));
	n->data = data;	
	n->key = key;
	if(h->count == 0)
	{
		n->next = NULL;
		n->prvs = NULL;
		h->first = n;
		h->last = n;
	}
	else
	{
		n->next = h->first;
		n->prvs = NULL;
		(n->next)->prvs = n;
		h->first = n;
	}
	h->count++;
}

void insertAtEnd(HEAD* h, void* data, int key)
{
	ll_node* n = (ll_node*)malloc(sizeof(ll_node));
	n->data = data;
	n->key = key;
	n->next = NULL;
	n->prvs = NULL;
	if(h->first == NULL)
	{
		h->first = n;
		h->last = n;
	}
	else
	{
		(h->last)->next = n;
		n->prvs = h->last;
		h->last = n;
	}
	h->count++;
}

void* deleteFromFront(HEAD* h)
{
	if(h->count == 0)
	{
		return NULL;
	}
	ll_node* temp = h->first;
	h->first = temp->next;
	if(h->first != NULL)
	{
		h->first->prvs = NULL;
	}
	temp->next = NULL;
	h->count--;
	return temp->data;
}

void deleteNode(HEAD* h,ll_node* n)
{
	if(n->prvs == NULL && n->next != NULL)
	{
		h->first = n->next;
		n->next->prvs = NULL;

	}
	else if(n->next == NULL && n->prvs != NULL)
	{
		h->last = n->prvs;
		n->prvs->next = NULL;
	}
	else if(n->next == NULL && n->prvs == NULL)
	{
		h->first = NULL;
		h->last = NULL;
	}
	else
	{	
		n->prvs->next = n->next;
		n->next->prvs = n->prvs;
	}
	n->next = NULL;
	n->prvs = NULL;
	free(n);
}