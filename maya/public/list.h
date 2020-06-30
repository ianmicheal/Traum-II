#pragma once
#include <assert.h>
/*Returns pointer to container*/
#define LIST_DATA(self, type, member)\
	((type *)((char *)(self) - (size_t)((&(((type *)0)->member)))))
/*The useful double linked list*/
typedef struct _Dlist
{
	struct _Dlist *next, *prev;
}Dlist;

static inline void
dlist_init (Dlist *head)
{
	head->next = head;
	head->prev = head;
}
static inline int
dlist_empty (Dlist *head)
{
	return head->next == head;
}
static inline Dlist *
dlist_front (Dlist *head)
{
	return head->next;
}
static inline Dlist *
dlist_back (Dlist *head)
{
	return head->prev;
}
static inline Dlist *
dlist_end (Dlist *head)
{
	return head;
}
static inline void
dlist_remove (Dlist *self)
{
	self->next->prev = self->prev;
	self->prev->next = self->next;
	self->next = NULL;
	self->prev = NULL;
}
static inline void
_dlist_insert (Dlist *self, Dlist *next, Dlist *prev)
{
	next->prev = self;
	self->next = next;
	self->prev = prev;
	prev->next = self;
}
static inline void
dlist_insert_front (Dlist *self, Dlist *head)
{
	_dlist_insert (self, head->next, head);
}
static inline void
dlist_insert_back (Dlist *self, Dlist *head)
{
	_dlist_insert (self, head, head->prev);
}

/*The venerable single list*/
typedef struct _Slist
{
	struct _Slist *next;
}Slist;

static inline void
slist_init (Slist *head)
{
	head->next = head;
}
static inline int
slist_empty (Slist *head)
{
	return head->next == head;
}
static inline Slist *
slist_start (Slist *head)
{
	return head->next;
}
static inline Slist *
slist_end (Slist *head)
{
	return head;
}
static inline void
slist_remove (Slist *self, Slist *head)
{
	Slist *n = head;
	assert (!slist_empty (head) && "Tried to remove item from an empty list");
	while (n->next != self)
	{
		assert (n->next != head && "Tried to remove unlinked item from list");
		n = n->next;
	}
	n->next = self->next;
	self->next = NULL;
}
static inline void
slist_insert (Slist *self, Slist *head)
{
	self->next = head->next;
	head->next = self;
}
