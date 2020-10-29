#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "simplec.h"

extern H_table* ht;
extern FILE* 	source;
extern FILE* 	output;

// global list head for program ast
AST_Node* prog_stmts;

void verify_c_file(char* file_arg)
{
	char* dotc = strchr(file_arg, '.');
	if (strncmp(dotc, ".c", 2) != 0) {
		fprintf(stderr, "simplec can only read .c files\n");
		exit(1);
	}
}

FILE* open_file(char* path)
{
	FILE* fp = fopen(path, "rb");
	if (fp == NULL) {
		perror(path);
		exit(1);
	}

	return fp;
}

void print_to_output_file()
{
	output = fopen("compilation.txt", "w");

	fprintf(output, "\n*************** SIMPLEC COMPILATION RESULTS ***************\n");
	fprintf(output, "\n%15s******* PROGRAM TREES *******\n", " ");
	print_AST(prog_stmts, output);
	fprintf(output, "%15s*****************************\n\n", " ");
	print_table(ht, output);
	fprintf(output, "\n***********************************************************\n");

	printf("compilation successful - output sent to \"./compilation.txt\"\n");
}

int main(int argc, char** argv)
{
	// verify file is a .c file
	if (argc > 1) {
		verify_c_file(argv[1]);
	}

	source = open_file(argv[1]);

	// initialize symbol table with reserved keywords
	ht = init_symtable();

	prog_stmts = create_AST_node();
	parse(&prog_stmts);

	print_to_output_file();

	free_AST(prog_stmts);
	free_table(ht);
}