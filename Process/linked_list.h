/*
FILE:linked_list.h
AUTHOR:Jordan Chou
USERNAME:18348691
UNIT: UCP
PURPOSE: Provides declarations for linked list
REFERENCE:-
COMMENTS:-
REQUIRES:-
Last Mod:26-10-2015
*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct LinkedListNode
{
	char* word;
	struct LinkedListNode* next;
} LinkedListNode;

typedef struct
{
	LinkedListNode* head;
	LinkedListNode* tail;
	int count;
} LinkedList;

LinkedList* create_list(void);
void insert_first(LinkedList *list, char word[51]);
void insert_last(LinkedList *list, char word[51]);
char* remove_from_start(LinkedList *list);
char* retrieve(LinkedList *list, int index);
void print_list(LinkedList *list);
void free_list(LinkedList **list);

#endif
