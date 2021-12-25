#ifndef LIST_HEADER
#define LIST_HEADER

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief A list to store charactor and its encoded binary.
 * 
 * @param letter Letter of this node.
 * @param binary Encoded binary of the letter.
 */
typedef struct list {
	char letter;
	char *binary;
	int times;
	struct list *next;
} list;

list *createList(char letter, char *binary, int times);
void pushList(list **listHead, char letter, char *binary, int times);
void deleteList(list **listhead);
list *findList(list **listhead, char letter);

list *createList(char letter, char *binary, int times) {
	list *temp   = (list *)malloc(sizeof(list));
	temp->letter = letter;
	temp->binary = binary;
	temp->times  = times;

	return temp;
}

void pushList(list **listHead, char letter, char *binary, int times) {
	// printf("list: %c %d\n", (letter=='\n')?'_':letter, times);
	list *temp = createList(letter, binary, times);

	if (*listHead == NULL) {
		*listHead = temp;
		return;
	}

	temp->next = *listHead;
	*listHead  = temp;
}

void deleteList(list **listhead) {
	list *temp = *listhead;
	while (*listhead != NULL) {
		temp = *listhead;
		*listhead = (*listhead)->next;
		free(temp);
	} 
}

list *findList(list **listhead, char letter) {
	list *temp = *listhead;
	if (*listhead == NULL) return NULL;
	while (temp != NULL) {
		if (temp->letter == letter) return temp;
		temp = temp->next;
	}
	return temp;
}

char findListLetter(list **listhead, char *code) {
	list *list_temp = *listhead;
	// if (list_temp == NULL) return '\0';
	while (list_temp != NULL) {
			// printf("=%s %s\n", list_temp->binary, code);
		if (!strcmp(list_temp->binary, code)){
			return list_temp->letter;

		} 
		list_temp = list_temp->next;
	}
	return '\0';
}


#endif