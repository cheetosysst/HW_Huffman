#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tree.h"
#include "queue.h"
#include "list.h"

#define flag puts("flag!") // For debug purpose

// Modes
void compress_file();
void decompress_file();

int main(int argc, char *argv[]) {

	// 檢查 arg
	if (argc<2) {
		puts("Argument error, exiting.");
		exit(1);
	}

	// 選擇要做的任務
	if (!strcmp(argv[1], "compress")) compress_file();
	if (!strcmp(argv[1], "extract"))  decompress_file();

	return 0;
}

/**
 * @brief Compress the text file using huffman encoding tree.
 * 
 */
void compress_file() {

	// Data and variables
	FILE  *file_source = fopen("input.txt", "r");
	FILE  *file_code   = fopen("code.txt",  "w");
	FILE  *file_binary = fopen("compress",  "wb");
	list  *list_head   = NULL;
	list  *list_temp   = NULL;
	queue *queue_head  = NULL;
	tree  *tree_root   = NULL;
	char   c;
	unsigned char  char_temp    = 0;
	char  *bin_code    = NULL;
	int   remain       = 8;
	int   total        = 0;
	int   i            = 0;

	// Read from file.
	while ((c=getc(file_source))!=EOF) {
		total++;

		// Check if the letter already exists.
		list_temp = findList(&list_head, c);
		if (list_temp != NULL) {
			list_temp->times++;
			continue;
		}

		pushList(&list_head, c, NULL, 1);
	}

	list_temp = list_head;
	while (list_temp!=NULL) {
		// total++;
		queueInsert(&queue_head, list_temp->times, list_temp->letter);
		list_temp = list_temp->next;
	}

	treeBuild(&tree_root, &queue_head);
	treeCodePrefix(&tree_root, "", "1");
	fprintf(file_code, "%d\n", total);
	printf("total: %d\n", total);
	treeSaveToFile(&tree_root, file_code);
	
	rewind(file_source);
	while ((c=getc(file_source))!=EOF) {
		// printf("=>%c %s\n", c, );
		bin_code = treeFindCode(&tree_root, c);
		// printf("s: %s\n", bin_code);
		for (i=0; i< (int)strlen(bin_code); i++) {
			char_temp += (bin_code[i]=='1');
			// printf("b: %d %d\n", bin_code[i]=='1', char_temp);
			remain--;
			if (!remain) {
				fwrite(&char_temp, sizeof(char_temp), 1, file_binary);
				remain = 8;
				char_temp = 0;
			}
			char_temp = char_temp << 1;
		}
	}
	while (remain-1) {
		char_temp = char_temp<<1;
		remain--;
	}
	fwrite(&char_temp, sizeof(char_temp), 1, file_binary);

	// Close all files.
	fclose(file_source);
	fclose(file_binary);
	fclose(file_code);
}

void decompress_file() {
	FILE *file_binary = fopen("compress", "rb");
	FILE *file_code   = fopen("code.txt", "r");
	FILE *file_output = fopen("output.txt", "w");

	list *list_head   = NULL;
	int corrupt_barrier[100];

	int letter_len;

	unsigned char letter;
	unsigned char *chr;

	char letter_temp;
	char *letter_code = NULL;
	char *binary_code = NULL;
	char *char_temp   = NULL;
	// int bit_index = 0;
	int binary_len    = 0;

	letter_code = (char *)malloc(1024);

	fscanf(file_code, "%d", &letter_len);
	printf("==%d\n", letter_len);
	while (fscanf(file_code, "%d%s", (int *)&letter, letter_code)!=EOF) {
		char *binary_temp = (char *)malloc(strlen(letter_code)+1);
		strcpy(binary_temp, letter_code);
		pushList(&list_head, letter, binary_temp, 0);
	}
	list *list_temp = list_head;
	while (list_temp !=NULL) {
		printf("%c %d %s\n", list_temp->letter, list_temp->times, list_temp->binary);
		list_temp = list_temp->next;
	}
	
	fseek(file_binary, 0, SEEK_END);
	binary_len = ftell(file_binary);
	rewind (file_binary);
	binary_code = "";
	chr = malloc(sizeof(unsigned char)*1);
	for (int i=0; i<binary_len; i++) {
		fread(&letter, 1, 1, file_binary);
		*chr = letter;
		for (int j=0; j<8; j++) {
			char_temp = binary_code;
			binary_code = (char *) malloc (strlen(binary_code)+2);
			strcpy(binary_code, char_temp);
			strcat(binary_code, (((*chr)>>7)&1)?"1":"0");
			printf("> %d %s\n", ((*chr)>>7)&1, binary_code);
			printf("letter: %d\n", *chr);
			(*chr) <<= 1;
			binary_code[strlen(binary_code)+1] = 0;
			// free(char_temp);
			letter_temp = findListLetter(&list_head, binary_code);
			
			printf("==%d \n", letter_temp == '\0');
			if (letter_temp!='\0') {
				printf("%c\n", letter_temp);
				fprintf(file_output, "%c", letter_temp);
				free(binary_code);
				binary_code = "";
				letter_len--;
			}
			printf("letter shift: %d\n", letter_len);
		}
			// if (!letter_len) break;
	}
	fclose(file_binary);
	fclose(file_code);
	fclose(file_output);
}

