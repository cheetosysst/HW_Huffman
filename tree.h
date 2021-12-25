#ifndef TREE_HEADER
#define TREE_HEADER

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "queue.h"

/**
 * @brief A huffman decoding tree.  
 * 
 * His name is Groot, be nice.
 * 
 * @param data chr count.
 * @param chr  charactor of this node.
 * @param code String containing the encoded binary of this node.
 */
typedef struct tree {
	int data;
	char chr;
	char *code;
	struct tree *left, *right;
} tree;

tree *createTree(int data, char chr, char *code);
void insertQueueTree(queue **queue_head, tree *tree_root);
void treeCodePrefix(tree **tree_root, char *code, char *suffix);
void treeBuild(tree **tree_root, queue **queue_head);
void treeSaveToFile(tree **tree_root, FILE *file_code);

/**
 * @brief Create a Tree object
 * 
 * @param data Times a letter appears in a file.
 * @param chr  The letter in this node.
 * @param code Binary code applied to this node.
 * @return tree* 
 */
tree *createTree(int data, char chr, char *code) {
	tree *temp = (tree *) malloc (sizeof(tree));
	temp->data = data;
	temp->chr  = chr;
	temp->code = code;
	temp->left = NULL;
	temp->right = NULL;

	return temp;
}

void insertQueueTree(queue **queue_head, tree *tree_root) {
	queueInsert(queue_head, tree_root->data, tree_root->chr);
	queue *queue_temp = queueFindSpecific(queue_head, tree_root->chr, tree_root->data);
	queue_temp->tree_root = tree_root;
}

/**
 * @brief Build a huffman encoding tree with provided priority queue.
 * 
 * @param tree_root  The root of the tree to build on.
 * @param queue_head Priority queue to base the tree on.
 */
void treeBuild(tree **tree_root, queue **queue_head) {

	// Data and variables
	tree  *tree_temp  = NULL;
	tree  *tree_one   = NULL; 
	tree  *tree_two   = NULL;
	// queue *queue_temp = NULL;

	// Iterate through the priority queue and build tree tree.
	// queue_temp = *queue_head;
	while (1) {
		// printf("%c %d\n", (queue_temp)->chr, queue_temp->data);

		// If the queue contains a tree.
		if ((*queue_head)->tree_root!=NULL) tree_one = (tree *)((*queue_head)->tree_root);
		// If the queue doesn't have a queue.
		else tree_one = createTree((*queue_head)->data, (*queue_head)->chr, NULL);
		queueDelete(queue_head);

		// If last node has a tree.
		if ((*queue_head) == NULL) {*tree_root = tree_one; break;}

		// Second tree node, same operation as the first.
		if ((*queue_head)->tree_root!=NULL) tree_two = (tree *)((*queue_head)->tree_root);
		else tree_two = createTree((*queue_head)->data, (*queue_head)->chr, NULL);
		queueDelete(queue_head);

		// printf("one %c %d\n", tree_one->chr, tree_one->data);
		// printf("two %c %d\n", tree_two->chr, tree_two->data);

		// Create new tree root.
		tree_temp = createTree(tree_one->data+tree_two->data, '\0', NULL);
		// Attach leaves.
		// Same frequency, compare char ascii number.
		if (tree_one->data == tree_two->data) {
			if (tree_one->chr < tree_two->data) {
				// puts("Insert same smaller");
				tree_temp->left = tree_one;
				tree_temp->right = tree_two;
			}
			if (tree_one->chr > tree_two->data) {
				// puts("Insert same greater");
				tree_temp->left = tree_two;
				tree_temp->right = tree_one;
			}
		}
		// Different frequency
		if (tree_one->data < tree_two->data) {
			// puts("Insert smaller");
			tree_temp->left = tree_one;
			tree_temp->right = tree_two;
		}
		if (tree_one->data > tree_two->data) {
			// puts("Insert greater");
			tree_temp->left = tree_two;
			tree_temp->right = tree_one;
		}
		if ((*queue_head) == NULL) {
			*tree_root = tree_temp;
			return;
		}
		insertQueueTree(queue_head, tree_temp);

		// Next loop
		// queue_temp = queue_temp->next;
	}
}

// TODO: Apply code prefix fasion, pass previos code and new postfix as string and then concat.
void treeCodePrefix(tree **tree_root, char *code, char *suffix) {
	if (*tree_root == NULL) return;
	(*tree_root)->code = (char *) malloc(strlen(code)+strlen(suffix)+1);
	// printf("====> %s %s\n", suffix, code);
	strcpy((*tree_root)->code, code);
	strcat((*tree_root)->code, suffix);
	if ((*tree_root)->chr != '\0') printf("=> %c %s\n", ((*tree_root)->chr=='\n')?'_':(*tree_root)->chr, (*tree_root)->code);
	treeCodePrefix(&((*tree_root)->left), (*tree_root)->code, "0");
	treeCodePrefix(&((*tree_root)->right), (*tree_root)->code, "1");
}

void treeSaveToFile(tree **tree_root, FILE *file_code) {
	if (*tree_root == NULL) return;
	if ((*tree_root)->chr != '\0') {
		fprintf(file_code, "%d %s\n", (*tree_root)->chr, (*tree_root)->code);
		printf("%c %s\n", (*tree_root)->chr, (*tree_root)->code);

	}
	treeSaveToFile(&((*tree_root)->left), file_code);
	treeSaveToFile(&((*tree_root)->right), file_code);
}

char *treeFindCode(tree **tree_root, char chr) {
	if (*tree_root == NULL) return NULL;
	if ((*tree_root)->chr == chr) return (*tree_root)->code;

	char *char_temp = NULL;
	char_temp = treeFindCode(&((*tree_root)->left), chr);
	if (char_temp != NULL) return char_temp;
	char_temp = treeFindCode(&((*tree_root)->right), chr);
	return char_temp;
}

#endif