/*
FILE:linked_list.c
AUTHOR:Jordan Chou
USERNAME:18348691
UNIT: UCP
PURPOSE: Provides functions that can manipulate a linked list
REFERENCE:-
COMMENTS:-
REQUIRES:-
Last Mod:26-10-2015
*/

#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "boolean.h"

/**
 * Creates a new linked list
 * @return  A pointer to a new linked list
 */
LinkedList* create_list(void)
{
	LinkedList *list;

	/* Allocate the memory needed for a linked list */
	list = (LinkedList*) malloc(sizeof(LinkedList));
	list->head = NULL;/* Initialise the head of the list to NULL */
	list->tail = NULL;/* Initialise the tail of the list to NULL */
	list->count = 0;/* Initialise the count to 0 */

	return list;/* Return the list */
}

/**
 * Inserts the word into the first position of the linked list
 */
void insert_first(LinkedList *list, char word[51])
{
	LinkedListNode *node, *current;

	/* Allocate the memory needed for a node */
	node = (LinkedListNode*) malloc(sizeof(LinkedListNode));

	node->word = word;/* Set the word field of the node to the import */

	node->next = list->head;/* Set next to the current first node of list */
	list->head = node;/* Set the head to the node */

	current = list->head;/* Set the current pointer to the head */

	if (list->count == 0)
	{
		list->tail = current;/* Set the tail to the last node in the list */
	}

	list->count = list->count + 1;/* Increment count */
}


/**
 * Inserts the word into the last position of the linked list
 */
void insert_last(LinkedList *list, char* word)
{
	LinkedListNode *node, *current;

	/* Allocate the memory needed for a node */
	node = (LinkedListNode*) malloc(sizeof(LinkedListNode));

	node->word = word;/* Set the word field of the node to the import */
	node->next = NULL;/* Will be inserted last so 'next' is NULL */

	current = list->head;/* Set the current pointer to the head */

	if (current == NULL)/* If nothing in the list */
	{
		list->head = node;/* Set the head to the first node */
	}
	else
	{
		list->tail->next = node;/* Place the node in the last position */
	}

	list->tail = node;/* Set the tail of the list to the node */
	list->count = list->count + 1;/* Increment the count */
}

/**
 * Removes the first word from a LinkedList
 * @param list The list to remove the word from
 */
char* remove_from_start(LinkedList *list)
{
	char *value;
	LinkedListNode *temp;

	if (list->count == 0)/* If nothing in the list then set value to NULL */
	{
		value = NULL;
	}
	else
	{
		value = list->head->word;/* Store the value of the node */

		temp = list->head->next;/* Set a temporary value to the next node */

		free(list->head);

		list->head = temp;/* Set the head of the list to the temp node */

		list->count = list->count - 1;/* Decrement the count */
	}

	return value;
}

/**
 * Retrieves the value stored in the linked list at the index number
 * @param index The i'th value to retrieve
 */
char* retrieve(LinkedList *list, int index)
{
	int ii;
	char *value;
	LinkedListNode *current;

	value = NULL;/* Initialise the value to NULL */
	current = list->head;/* Set the current node to the head of the list */
	ii = 0;/* Initialise the counter to 0 */

	/* Traverse the list to get to the required index */
	while ((ii < index) && (current != NULL))
	{
		current = current->next;
		ii++;
	}

	if (current != NULL)
	{
		value = current->next->word;
	}

	return value;
}

/**
 * Print out the contents of a linked list
 * @param list The list to print out
 */
void print_list(LinkedList *list)
{
	LinkedListNode *current;

	current = list->head;/* Set the current node to the head of the list */

	while (current != NULL)/* While there are still entries in the list */
	{
		/* Print out the entries */
		printf("%s ", current->word);
		current = current->next;
	}
	printf("\n");/* Print out a newline for formatting purposes */
}

/**
 * Free the contents of the list
 * @param list The list to free
 */
void free_list(LinkedList **list)
{
	LinkedListNode *current, *previous;

	current = (*list)->head;/* Set the current node to the head */

	while (current != NULL)/* While there is still entries in the list */
	{
		previous = current;
		current = current->next;/* Increment the current pointer first */
		free(previous->word);
		free(previous);/* Free the previous list node */
	}

	free(current);/* Free the last node */

	free(*list);/* Free the list */
}
