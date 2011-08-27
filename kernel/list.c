#include "kernel/list.h"

bool is_head(list_node_t *pnode)
{
	return (pnode != NULL) &&
	       (pnode->prev) == NULL &&
	       (pnode->next != NULL);
}

bool is_interio(list_node_t *pnode)
{
	return (pnode != NULL) &&
	       (pnode->prev != NULL) &&
	       (pnode->next != NULL);
}

bool is_tail(list_node_t *pnode)
{
	return (pnode != NULL) &&
	       (pnode->prev != NULL) &&
	       (pnode->next == NULL);
}

void init_list(list_t *plist)
{
	plist->head.prev = NULL;
	plist->head.next = &plist->tail;
	plist->tail.prev = &plist->head;
	plist->tail.next = NULL;
}

list_node_t *begin_list(list_t *plist)
{
	return plist->head.next;
}

list_node_t *next_list(list_node_t *pnode)
{
	return pnode->next;
}

list_node_t *end_list(list_t *plist)
{
	return &plist->tail;
}

list_node_t *head_list(list_t *plist)
{
	return &plist->head;
}

list_node_t *tail_list(list_t *plist)
{
	return &plist->tail;
}

list_node_t *front_list(list_t *plist)
{
	return plist->head.next;
}

list_node_t *back_list(list_t *plist)
{
	return plist->tail.prev;
}

bool is_empty_list(list_t *plist)
{
	return (begin_list(plist) == end_list(plist));
}

void insert_after_list(list_node_t *before, list_node_t *pnode)
{
	pnode->prev = before->prev;
	pnode->next = before;
	before->prev->next = pnode;
	before->prev = pnode;
}

void insert_before_list(list_node_t *after, list_node_t *pnode)
{
	pnode->prev = after;
	pnode->next = after->next;
	after->next->prev = pnode;
	after->next = pnode;
}

void insert_front_list(list_t *plist, list_node_t *pnode)
{
	insert_after_list(begin_list(plist), pnode);
}

void insert_back_list(list_t *plist, list_node_t *pnode)
{
	insert_after_list(end_list(plist), pnode);
}

list_node_t *delete_list(list_node_t *pnode)
{
	pnode->prev->next = pnode->next;
	pnode->next->prev = pnode->prev;
	return pnode->next;
}

list_node_t *delete_front_list(list_t *plist)
{
	list_node_t *front = front_list(plist);
	delete_list(front);
	return front;
}

list_node_t *delete_back_list(list_t *plist)
{
	list_node_t *back = back_list(plist);
	delete_list(back);
	return back;
}

