#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simplec.h"

extern HT_item* item_ptr;

unsigned long hash_f(char* key) /* hash function */
{
	unsigned long total = 0;
	for (int j = 0; key[j]; j++) {
		total += key[j];
	}
	return total % CAPACITY;
}


HT_item* create_item(Token tok, char* lexm)
{
	HT_item* item  = (HT_item*)malloc(sizeof(HT_item));
	item->lexeme   = (char*)malloc(sizeof(lexm));

	item->token = tok;
	strcpy(item->lexeme, lexm);

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

void ht_insert(H_table* table, Token tok, char* lexm)
{
	HT_item* item = create_item(tok, lexm);
	item_ptr = item;
	unsigned long index = hash_f(lexm);
	HT_item* current_item = table->items[index];

	/* if item key does not exist */

	if (current_item == NULL) { 

	    /* hash table full */  

		if (table->count == table->size) {  
			free_item(item);
			return;
		}

		/* insert directly */

		table->items[index] = item;
		table->count++;	
	}
	else {
		if (strcmp(current_item->lexeme, lexm) == 0) {
			/* do nothing */
			;
		}
		else {
			handle_collision(table, index, item);
		}
	}
}

Token get_token(H_table* table, char* lexm)
{
	unsigned int index = hash_f(lexm);
	HT_item* item = table->items[index];
	Linked_List* head  = table->buckets[index];
	while (item != NULL) {
		if (strcmp(item->lexeme, lexm) == 0) {
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

void handle_collision(H_table* table, unsigned long index, HT_item* item)
{
	/* handle duplicates */
	
	if (item->token == get_token(table, item->lexeme)) {
		return;
	}

	Linked_List* head = table->buckets[index];

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
			fprintf(fp, "%13sIdentifier: %8s,  Index: %3lu\n", " ",
				table->items[index]->lexeme,
				hash_f(table->items[index]->lexeme));

			if (table->buckets[index] != NULL) {
				Linked_List* head = table->buckets[index];

				while (head) {
					fprintf(fp, "%10sToken: %2d, Lexeme: %8s, Index: %3lu ==> from overflow bucket\n", " ",
						head->item->token,
						head->item->lexeme,
						hash_f(head->item->lexeme));
					head = head->next;
				}
			}
		}
	}
	fprintf(fp, "%15s*****************************\n\n", " ");
}

Linked_List* create_list()
{
	Linked_List* list = (Linked_List*)malloc(sizeof(Linked_List));
	return list;
}

Linked_List* list_insert(Linked_List* list, HT_item* item)
{
	/* walk list and insert */
	Linked_List* current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	Linked_List* new_list = create_list();
	new_list->item        = item;
	new_list->next        = NULL;

	current->next = new_list;
	return new_list;
}

Linked_List** create_buckets(H_table* table)
{
	Linked_List** buckets = (Linked_List**)calloc(table->size, 
							sizeof(Linked_List*));
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

void free_item(HT_item* item)
{
	free(item->lexeme);
	free(item);
}

void free_list(Linked_List* list)
{
	while (list != NULL) {
		Linked_List* temp = list;
		list = list->next;
		free(temp->item->lexeme);
		free(temp->item);
		free(temp);
	}
}

void free_buckets(H_table* table)
{
	Linked_List** buckets = table->buckets;
	for (unsigned int i = 0; i < table->size; i++) {
		free_list(buckets[i]);
	}
}