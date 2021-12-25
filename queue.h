#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define flag puts("flag!") // For debug purpose

/**
 * @brief Priority queue
 * Can be use as a normal queue or list if you're a monster who don't mind commiting war crime.
 * 
 * @param data chr count.
 * @param chr  character of this node.
 */
typedef struct queue {
	int data;
	char chr;
	struct queue *next;
	void *tree_root;
} queue;

/**
 * @brief Create priority queue object.
 * 
 * @param data Times this letter appears in the text file.
 * @param chr The letter of this node.
 * @return queue* 
 */
queue *createqueue(int data, char chr) {
	queue *temp = (queue *) malloc(sizeof(queue));
	temp->next  = NULL;
	temp->data  = data;
	temp->chr   = chr;
	return temp;
}

/**
 * @brief Insert data into a queue.
 * 
 * @param root The head of the queue.
 * @param times Times the letter appears in the text file.
 * @param chr The letter of this node.
 */
void queueInsert(queue **root, int times, char chr) {
	// printf("queue: %c %d\n", (chr=='\n')?'_':chr, times);
	
	queue *temp = createqueue(times, chr);
	// printf("=> %c\n", chr);
	if ((*root) == NULL) {
		// printf("=> insert root  %c\n", chr);
		(*root) = temp;
		return;
	}
	if ((*root)->data > times) {
		// printf("=> insert root pre  %c\n", chr);
		temp->next = (*root);
		(*root) = temp;
		return;
	}
	if ((*root)->data == times) {
		if ((int)((*root)->chr) < (int)chr) {
			// printf("=> insert root back %c\n", chr);
			temp->next = (*root)->next;
			(*root)->next = temp;
			return;
		} else {
			// printf("=> insert root pre  %c\n", chr);
			temp->next = (*root);
			(*root) = temp;
			return;
		}
	}

	queue *ptr = (*root);
	while (ptr->next != NULL && ptr->next->data < times) {
		if (ptr->next->data == times && ptr->next->chr < chr) {
			// printf("=> insert pre  %c\n", chr);
			temp->next = ptr->next;
			ptr->next = temp;
			return;
		}
		if (ptr->next->data == times) {
			// printf("=> insert back  %c\n", chr);
			temp->next = ptr->next->next;
			ptr->next->next = temp;
			return;
		}
		ptr = ptr->next;
	}
	// printf("=> insert normal  %c\n", chr);
	temp->next = ptr->next;
	ptr->next  = temp;
}

/**
 * @brief Delete an object from queue.
 * 
 * @param q The object you want to delete.
 */
void queueDelete(queue **q) {
	if ((*q) == NULL) return;

	queue *temp = (*q);
	(*q) = (*q)->next;
	free(temp);
	return;
}

/**
 * @brief Find queue object.
 * 
 * @param root Head of the queue.
 * @param alphabet The letter to find.
 * @return queue* 
 */
queue *queueFind(queue **root, char alphabet){
	if ((*root)==NULL) return NULL;
	queue *temp = *root;
	while (temp!=NULL) {
		if (temp->chr == alphabet) return temp;
		// flag;
		temp = temp->next;
	}
	return NULL;
}

queue *queueFindSpecific(queue **root, char alphabet, int data){
	if ((*root)==NULL) return NULL;
	queue *temp = *root;
	while (temp!=NULL) {
		if (temp->chr == alphabet && temp->data == data) return temp;
		temp = temp->next;
	}
	return NULL;
}

#endif