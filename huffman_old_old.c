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
} queue;

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

/**
 * @brief A list to store charactor and its encoded binary.
 * 
 * @param letter Letter of this node.
 * @param binary Encoded binary of the letter.
 */
typedef struct list {
	char letter;
	char *binary;
	struct list *next;
} list;

void compress_file(char source[], char code[], char binary[]);
void decompress_file();


queue *createqueue(int data, char chr);
void queueInsert(queue **root, int data, char chr);
void queueDelete(queue **root);
queue *queueFind(queue **root, char alphabet);

tree *createtree(int data, char chr);
void treeBuild(tree **root, queue **head);
char *treeFindCode(tree **root, char c);


list *createList();
void pushList(list **listHead, char letter, char *binary);

int main(int argc, char *argv[]) {

	// 檢查有多少個 arg
	if (argc<2) {
		puts("Argument error, exiting.");
		exit(1);
	}

	// 選擇要做的任務
	if (!strcmp(argv[1], "compress")) compress_file("input.txt", "code.txt", "compress");
	if (!strcmp(argv[1], "extract")) decompress_file();

}

/**
 * @brief Compress file with huffman decoding tree. Only ascii character is supported. Parameters isn't necessary. I tried to use arg to determine file path, but gave up implementing it.
 * 
 * @param source Name of source file.
 * @param code   Name of code.txt file.
 * @param binary Name of encoded binary file.
 */
void compress_file(char source[], char code[], char binary[]) {

	// 會使用到的檔案
	FILE *file_source = fopen(source, "r" );
	FILE *file_code   = fopen(code  , "w" );
	FILE *file_binary = fopen(binary, "wb");
	FILE *file_pointer_r;

	// 會使用到的結構們
	queue *priority_queue = NULL;
	queue *queue_head     = NULL;
	queue *temp           = NULL;
	tree  *tree_root      = NULL;
	tree  *tree_temp      = NULL;

	// 計算實用到的變數
	int times = 0, file_len = 0;
	char c;
	unsigned char c_temp;
	char *bin_code;

	// 讀取原始檔案內容
	file_pointer_r = file_source;
	while ((c=getc(file_pointer_r))!=EOF) {
		// if (c=='\n') continue;
		temp = queueFind(&queue_head, c);

		if (temp!=NULL) {
			temp->data++;
			continue;
		}
		printf("%c %p\n", c, temp);
		queueInsert(&queue_head, 1, c);
	}

	// 排列成優先佇列
	temp = queue_head;
	while (temp != NULL) queueInsert(&priority_queue, temp->data, temp->chr), temp = temp->next;
	// queueInsert(&priority_queue, 1, '!');

	// 印出優先佇列
	temp = priority_queue;
	puts("Priority queue");
	puts("========================================");
	while (temp != NULL) printf("=> %d %c\n", temp->data, temp->chr), temp = temp->next;
	puts("========================================\n");

	// 將佇列內容建立為樹
	while (priority_queue != NULL) treeBuild(&tree_root, &priority_queue);

	// 對樹的各個節點插入編碼，並紀錄結果至 code.txt
	// rewind(file_source);
	fseek(file_source, 0L, SEEK_END);
	file_len=ftell(file_source);
	rewind(file_source);
	tree_temp = tree_root;
	// Save full length
	fprintf(file_code, "%d\n", file_len);
	while (tree_temp!=NULL) {


		// Prepare code
		bin_code = malloc(sizeof(char)*(times+2));
		memset(bin_code, '0', times+2);
		bin_code[times+1] = 0;

		// Write code
		bin_code[times] = '1';
		fprintf(file_code, "%d %s\n", tree_temp->right->chr, bin_code);
		tree_temp->right->code = bin_code;

		// Write last code
		if (tree_temp->left->chr != '_') {
			bin_code = malloc(sizeof(char)*(times+2));
			memset(bin_code, '0', times+2);
			bin_code[times+1] = 0;
			bin_code[times] = '0';
			fprintf(file_code, "%d %s\n", tree_temp->left->chr, bin_code);
			tree_temp->left->code = bin_code;

			break;
		}

		times++;
		tree_temp = tree_temp->left;
	}
	tree_temp = tree_root;

	// 印出編碼對照表
	puts("Code");
	puts("========================================");
	while (tree_temp->left!=NULL) {
		printf("=> %c %s\n", tree_temp->right->chr, tree_temp->right->code);
		if (tree_temp->left->chr != '_') printf("=> %c %s\n", tree_temp->left->chr, tree_temp->left->code);
		tree_temp = tree_temp->left;
	}
	puts("========================================\n");

	// 寫入二進位壓縮檔
	puts("Writing to binary");
	puts("========================================");
	rewind(file_source);
	times = 0;
	int remain = 8;
	c_temp = 0;
	while ((c=getc(file_source))!=EOF) {
		bin_code = treeFindCode(&tree_root, c);
		printf("=> %c %s\n", c, bin_code);
		times = strlen(bin_code);
		for (int i=0; i<times; i++) {
			if (remain) {
				printf("%d %d\n", (bin_code[i]=='1') & 1, remain);
				c_temp = c_temp<<1;
				c_temp += ((bin_code[i]=='1') & 1);
				remain--;
				continue;
			}
			printf("c_temp: %d\n", (int)c_temp);
			fwrite(&c_temp, sizeof(c_temp), 1, file_binary);
			c_temp = 0;
			remain = 8;
			for (; i<times; i++) {
				printf("%d %d\n", (bin_code[i]=='1') & 1, remain);
				c_temp = c_temp<<1;
				c_temp += ((bin_code[i]=='1') & 1);
				remain--;
			}
		}
	}
	while (remain) {
		// printf("%d %d=\n", 0, remain);
		c_temp = c_temp<<1;
		remain--;
		continue;
	}
	fwrite(&c_temp, sizeof(c_temp), 1, file_binary);
	puts("========================================\n");

	// 關閉檔案
	fclose(file_source);
	fclose(file_binary);
	fclose(file_code);
}

/**
 * @brief Decode the binary file with code.txt.
 */
void decompress_file() {

	// 會用到的檔案
	FILE *file_binary = fopen("compress", "rb");
	FILE *file_code   = fopen("code.txt", "r");
	FILE *file_output = fopen("output.txt", "w");
	
	// 計算使用的變數
	unsigned char temp;
	char letter;
	long long int len;
	int i, j, k, max_code_len, input_len, printed_len, temp_len;
	list *code_list=NULL;
	char binary_code[256];

	// 讀取 code.txt 中的編碼對照表
	puts("Reading code file");
	puts("========================================");
	fscanf(file_code, "%d", &temp_len);
	printf("input len: %d\n", temp_len);
	input_len = temp_len;
	// fgetc(file_code);
	while (fscanf(file_code, "%d%s", &letter, binary_code)!=EOF) {
		char *temp_binary = malloc(sizeof(char)*256);
		strcpy(temp_binary, binary_code);
		printf("=> %c\t%s\n", letter, binary_code);
		pushList(&code_list, letter, temp_binary);
		// printf("=>=> %d\t%s\n", code_list->letter, code_list->binary);
	}
	puts("========================================\n");

	// 印出對照表 list 內容，並計算最長的編碼長度（最後沒用到這個）。
	puts("List content");
	puts("========================================");
	list *list_temp = code_list;
	max_code_len = 0;
	while (list_temp != NULL) {
		printf("=> %d\t%s\n", list_temp->letter, list_temp->binary);
		if (strlen(list_temp->binary)>max_code_len) max_code_len = strlen(list_temp->binary);
		list_temp = list_temp->next;
	}
	printf("max: %d\n", max_code_len);
	puts("========================================\n");

	// 讀取二進位壓縮檔
	puts("Reading binary file");
	puts("========================================");
	fseek(file_binary, 0, SEEK_END); // 設置指標
	len = ftell(file_binary); // 取得檔案長度
	k = 0;
	printed_len = 0;
	memset(binary_code, 0, sizeof(char)*256);
	rewind(file_binary);
	for (i=0; i<len; i++) {
		fread(&temp, 1, 1, file_binary);
		printf("=> %d\t", temp);
		for (j=0; j<8; j++) {
			binary_code[k] = (int)'0'+((temp>>7)&1);
			list_temp = code_list;
			while (list_temp != NULL) {
				if (!strcmp(list_temp->binary, binary_code)) {
					if (printed_len == input_len) {printf("\nflag: %d %d\n", printed_len, input_len);break;}
					printf("%c", list_temp->letter);
					fprintf(file_output, "%c", list_temp->letter);
					printed_len++;
					k=-1;
					memset(binary_code, 0, sizeof(char)*256);
					break;
				}
				list_temp = list_temp->next;
			}
			if (printed_len==input_len) break;
			k++;
			temp <<= 1;
		} 
		puts("");
	}
	puts("========================================\n");

	// 關閉檔案
	fclose(file_binary);
	fclose(file_output);
	fclose(file_code);
}

/**
 * @brief Create a priority queue object.
 * 
 * @param data 
 * @param chr 
 * @return queue* 
 */
queue *createqueue(int data, char chr) {
	queue *temp = (queue *) malloc(sizeof(queue));
	temp->next = NULL;
	temp->data = data;
	temp->chr  = chr;
	return temp;
}

/**
 * @brief Insert data into a queue.
 * 
 * @param root The head of the queue.
 * @param times 
 * @param chr 
 */
void queueInsert(queue **root, int times, char chr) {
	
	queue *temp = createqueue(times, chr);

	if ((*root) == NULL) {
		(*root) = temp;
		return;
	}

	if ((*root)->data > times) {
		temp->next = (*root);
		(*root) = temp;
		return;
	}

	if ((*root)->next == NULL && (*root)->data == times) {
		if ((int)((*root)->chr) < (int)chr) {
			(*root)->next = temp;
			return;
		} else {
			temp->next = (*root);
			(*root) = temp;
			return;
		}
	}

	queue *ptr = (*root);
	while (ptr->next != NULL && ptr->next->data <= times) {
		ptr = ptr->next;
	}
	
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
 * @brief Create an tree node.
 * 
 * @param data 
 * @param chr 
 * @return tree* 
 */
tree *createtree(int data, char chr) {
	tree *temp = (tree *) malloc(sizeof(tree));
	temp->left = temp->right = NULL;
	temp->data = data;
	temp->chr  = chr;
	return temp;
}

/**
 * @brief Build tree with given priority queue.
 * 
 * @param root Tree root node.
 * @param head Queue head node.
 */
void treeBuild(tree **root, queue **head) {
	tree *tree_root = *root;
	queue *queue_head = *head;
	tree *left, *right;
	
	tree *new_tree = createtree(0, '_');

	// Get left tree node, either from the old tree root, or creating a new node.
	if (tree_root == NULL) {
		left = createtree(queue_head->data, queue_head->chr);
		queueDelete(head);
	}
	else
		left = tree_root;


	queue_head = *head;

	// Get right node, which is always a new node from queue.
	right = createtree(queue_head->data, queue_head->chr);
	queueDelete(head);
	queue_head = *head;

	// Assign tree branches to new tree root.
	new_tree->left = left;
	new_tree->right = right;
	new_tree->data = left->data + right->data;
	*root = new_tree;
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

/**
 * @brief Find a letters encoded binary in the tree.
 * 
 * @param root Root node of the tree.
 * @param c    The letter to find.
 * @return char* 
 */
char *treeFindCode(tree **root, char c) {
	tree *tree_temp = *root;
	while (tree_temp->left!=NULL) {
		// printf("looking@%c %c %s\n", tree_temp->right->chr, c, tree_temp->right->code);
		if (tree_temp->right->chr == c) return tree_temp->right->code;
		tree_temp = tree_temp->left;
	}
	if (tree_temp->chr == c) return tree_temp->code;

	return NULL;
}

/**
 * @brief Create a List object
 * 
 * @return list* 
 */
list *createList() {
	list *temp = (list *)malloc(sizeof(list));
	temp->next = NULL;
	temp->letter = 0;
	temp->binary = 0;
	return temp;
}

/**
 * @brief Push a node to the head of the list.
 * 
 * @param listHead The head of the list
 * @param letter   The letter of the new node.
 * @param binary   The encoded binary of the letter.
 */
void pushList(list **listHead, char letter, char *binary) {
	list *temp = createList();
	temp->letter = letter;
	temp->binary = binary;

	if (listHead == NULL) {
		temp->next = NULL;
		*listHead = temp;
		return;
	}

	temp->next = *listHead;
	*listHead = temp;
}