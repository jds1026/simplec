#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simplec.h"

extern HT_item* item_ptr;

// load reserved keywords into symbol table
H_table* init_symtable()
{
	H_table* ht = create_table(CAPACITY);
	
	ht_insert(ht, "main",   TK_MAIN,    "main");
	ht_insert(ht, "return", TK_RETURN,  "return");
	ht_insert(ht, "while",  TK_WHILE,   "while");
	ht_insert(ht, "if",     TK_IF, 	    "if");
	ht_insert(ht, "else",   TK_ELSE,    "else");
	ht_insert(ht, "int",    TK_INT, 	"int");
	ht_insert(ht, "float",  TK_FLOAT,   "float");
	ht_insert(ht, "exp",    TK_EXP,     "exp");
	ht_insert(ht, "char",   TK_CHAR, 	"char");
	ht_insert(ht, "void",   TK_VOID, 	"void");
	ht_insert(ht, "+",      TK_ADD,     "+");
	ht_insert(ht, "-",      TK_SUB,     "-");
	ht_insert(ht, "*",      TK_MUL,	    "*");
	ht_insert(ht, "/",      TK_DIV,  	"/");
	ht_insert(ht, "<",      TK_LT,   	"<");
	ht_insert(ht, "<=",     TK_LE,	    "<=");
	ht_insert(ht, "==",     TK_EQ,  	"==");
	ht_insert(ht, ">=",     TK_GE, 	    ">=");
	ht_insert(ht, ">",      TK_GT,  	">");
	ht_insert(ht, "!=",     TK_NE,      "!=");
	ht_insert(ht, "=",      TK_ASSIGN,  "=");
	ht_insert(ht, ";",      TK_SMCOLON, ";");
	ht_insert(ht, "(",      TK_LPAREN,  "(");
	ht_insert(ht, ")",      TK_RPAREN,  ")");
	ht_insert(ht, "{",      TK_LBRACE,  "{");
	ht_insert(ht, "}",      TK_RBRACE,  "}");

	return ht;
}

// hash function
unsigned long hash_f(char* key)
{
	unsigned long total = 0;
	for (int j = 0; key[j]; j++) {
		total += key[j];
	}
	return total % CAPACITY;
}

// called by ht_insert
HT_item* create_item(char* key, Token tok, char* lexm)
{
	HT_item* item  = (HT_item*)malloc(sizeof(HT_item));
	item->lexeme = (char*)malloc(sizeof(lexm));
	item->key = (char*)malloc(sizeof(key));

	item->token = tok;
	strcpy(item->lexeme, lexm);
	strcpy(item->key, key);

	return item;
}

H_table* create_table(unsigned int size)
{
	H_table* table = (H_table*)malloc(sizeof(H_table));
	table->size    = size;
	table->count   = 0;
	table->items   = (HT_item**)calloc(table->size, sizeof(HT_item*));

	for (int i = 0; i < table->size; i++) {
		table->items[i] = NULL;
	}
	table->buckets = create_buckets(table);   

	return table;
}

void ht_insert(H_table* table, char* key, Token tok, char* lexm)
{
	HT_item* item = create_item(key, tok, lexm);
	item_ptr = item;
	unsigned long index = hash_f(key);
	HT_item* current_item = table->items[index];

	if (current_item == NULL) {
		// if item key does not exist.. yet
		if (table->count == table->size) {
			// hash table tull
			free_item(item);
			return;
		}

		// insert directly
		table->items[index] = item;
		table->count++;	
	}
	else {
		if (strcmp(current_item->key, key) == 0) {
			// do nothing
			;
		}
		else {
			handle_collision(table, index, item);
		}
	}
}

// called by lexer to return token to parser
Token get_token(H_table* table, char* key)
{
	unsigned int index = hash_f(key);
	HT_item* item = table->items[index];
	Linked_list* head = table->buckets[index];
	while (item != NULL) {
		if (strcmp(item->key, key) == 0) {
			return item->token;
		}
		if (head == NULL) {
			return 0;
		}
		else { 
			item = head->item;
			head = head->next;
		}
	}

	fprintf(stderr, "token error\n");
	exit(1);
}

// called by ht_insert
void handle_collision(H_table* table, unsigned long index, HT_item* item)
{
	// handle duplicates
	if (item->token == get_token(table, item->lexeme)) {
		return;
	}

	Linked_list* head = table->buckets[index];

	if (head == NULL) {
		head = create_list();
		head->item = item;
		head->next = NULL;
		table->buckets[index] = head;
	} else {
		table->buckets[index] = list_insert(head, item);
	}
}

void print_table(H_table* table, FILE* fp)
{
	fprintf(fp, "\n%15s******* SYMBOL TABLE ********\n", " ");
	for (unsigned int index = 0; index < table->size; index++) {
		if (table->items[index] != NULL) {
			fprintf(fp, "%10sToken: %2d, Lexeme: %8s, Index: %3lu\n", " ",
				table->items[index]->token,
				table->items[index]->lexeme,
				hash_f(table->items[index]->key));
			if (table->buckets[index] != NULL) {
				Linked_list* head = table->buckets[index];
				// printf("Token: %2d => overflow bucket => ", head->item->token);
				while (head) {
					fprintf(fp, "%10sToken: %2d, Lexeme: %8s, Index: %3lu ==> from overflow bucket\n", " ",
						head->item->token,
						head->item->lexeme,
						hash_f(head->item->key));
					head = head->next;
				}
			}
		}
	}
	fprintf(fp, "%15s*****************************\n\n", " ");
}

// called by list_insert for collision handling
Linked_list* create_list()
{
	Linked_list* list = (Linked_list*)malloc(sizeof(Linked_list));
	return list;
}

// called by handle_collision
Linked_list* list_insert(Linked_list* list, HT_item* item)
{
	// // empty list
	// if (list == NULL) {
	// 	Linked_list* head = create_list();
	// 	head->item = item;
	// 	head->next = NULL;

	// 	list = head;
	// 	return list;
	// }

	// walk list and insert
	Linked_list* current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	Linked_list* new_list = create_list();
	new_list->item = item;
	new_list->next = NULL;
	current->next = new_list;

	return new_list;
}

// called by create_table
Linked_list** create_buckets(H_table* table)
{
	Linked_list** buckets = (Linked_list**)calloc(table->size, 
								sizeof(Linked_list**));
	for (unsigned int i = 0; i < table->size; i++) {
		buckets[i] = NULL;
	}

	return buckets;
}

void free_table(H_table* table)
{
	for (int i = 0; i < table->size; i++) {
		HT_item* item = table->items[i];
		if (item != NULL) {
			free_item(item);
		}
	}

	free_buckets(table); 
	free(table->items);
	free(table);
}

// called by free table
void free_item(HT_item* item)
{
	free(item->lexeme);
	free(item->key);
	free(item);
}

// called by free_buckets
void free_list(Linked_list* list)
{
	while (list != NULL) {
		Linked_list* temp = list;
		list = list->next;
		free(temp->item->lexeme);
		free(temp->item->key);
		free(temp->item);
		free(temp);
	}
}

// called by free_table
void free_buckets(H_table* table)
{
	Linked_list** buckets = table->buckets;
	for (unsigned int i = 0; i < table->size; i++) {
		free_list(buckets[i]);
	}
}