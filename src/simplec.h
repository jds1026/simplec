#ifndef _SIMPLEC_H_
#define _SIMPLEC_H_

#include <stdio.h>
#include <stdbool.h>

#define CAPACITY 200
#define BUFFSIZE 128
#define BLANK ' '
#define TAB '\t'
#define NEWLINE '\n'
#define END 260

/* 
token values that will be used by the lexer and parser 
to identify and define the symbol read in the source file
*/

typedef enum {
	TK_ID, TK_NUM, TK_CHARACTER,                  // identifier, number, character
	TK_MAIN, TK_RETURN, TK_IF, TK_ELSE, TK_WHILE, // reserved keywords
	TK_INT, TK_FLOAT, TK_CHAR, TK_VOID,						
	TK_ADD, TK_SUB, TK_MUL, TK_DIV,               // arithmetic operators
	TK_LT, TK_LE, TK_EQ, TK_GE, TK_GT, TK_NE,     // relational operators
	TK_ASSIGN, TK_SMCOLON, TK_LPAREN, TK_RPAREN,  // punctuation
	TK_LBRACE, TK_RBRACE
} Token;

/* attributes for token literals */

typedef enum Data_Type {
	T_INT, T_FLOAT, T_EXP, T_CHAR
} Data_Type;

/* for token literals */

typedef union {
	int   i;
	float f;
	char  c;
} Literal_Value;

typedef struct littype {
	Data_Type     type;
	Literal_Value val;
} Literal_Type;

/* 
hash table entry that contains the 
attributes of each symbol read 
*/

typedef struct HT_item {
	char*  lexeme;
	Token  token;
} HT_item;

/* 
singly linked list used to handle hash
collision overflow in the hashtable
*/

typedef struct Linked_List {
	HT_item*            item;
	struct Linked_List* next;
} Linked_List;

/* symbol table */

typedef struct H_table {
	HT_item**     items;
	Linked_List** buckets;
	unsigned int  size;
	unsigned int  count;
} H_table;

/* 
ast node values that will be used by the parser to 
create a root node for each statement parsed 
*/

typedef enum Node_Type {
	ID_NODE, CONST_NODE,
	IF_NODE, WHILE_NODE,
	ARITHM_NODE, RELAT_NODE,
	ASSIGN_NODE, RETURN_NODE,
} Node_Type;

/* arithmetic op attributes */

typedef enum Arithm_Op {
	ADD_OP, SUB_OP, MUL_OP, DIV_OP
} Arithm_Op;

/* relational op attributes */

typedef enum Relat_Op {
	LT_OP, LE_OP, EQ_OP, 
	GE_OP, GT_OP, NE_OP
} Relat_Op;

/* node definitions */

typedef struct AST_Node {
	Node_Type        type;
	struct AST_Node* left;
	struct AST_Node* right;
	struct AST_Node* next;
} AST_Node;

typedef struct AST_If_Node {
	Node_Type type;
	AST_Node* condition;
	AST_Node* if_branch;
	AST_Node* next;
	AST_Node* else_branch;
} AST_If_Node;

typedef struct AST_While_Node {
	Node_Type type;
	AST_Node* condition;
	AST_Node* while_branch;
	AST_Node* next;
} AST_While_Node;

typedef struct AST_Assign_Node {
	Node_Type type;
	AST_Node* loperand;
	AST_Node* value;
	AST_Node* next;
} AST_Assign_Node;

typedef struct AST_Arithm_Node {
	Node_Type type;
	Arithm_Op operator;
	AST_Node* left;
	AST_Node* right;
} AST_Arithm_Node;

typedef struct AST_Relat_Node {
	Node_Type type;
	Relat_Op  operator;
	AST_Node* left;
	AST_Node* right;
} AST_Relat_Node;

typedef struct AST_ID_Node {
	Node_Type type;
	HT_item*  item;
} AST_ID_Node;

typedef struct AST_Const_Node {
	Node_Type     type;
	Data_Type     data_type;
	Literal_Value value;
} AST_Const_Node;

typedef struct AST_Return_Node {
	Node_Type type;
	int       ret_type;
	char*     ret_val;
} AST_Return_Node;

/* program globals */

extern H_table*      ht;
extern HT_item*      item_ptr;
extern FILE*         source;
extern FILE*         output;
extern Literal_Type  literal;
extern unsigned int  lineno;

/* main.c */

static void  verify_c_file(char* file_arg);
static FILE* open_file(char* path);
static void  print_to_output_file();

/* lexer.c */

static int   fail();
Token lexan();
static bool  token_is_keyword(char* input_buff, Token* tok);

/* parser.c */

void 	  parse(AST_Node** stmts_head);
void      func_block(AST_Node** stmts_head);
void 	  block(AST_Node** stmts_head);
void 	  parse_stmts(AST_Node** node);
AST_Node* parse_assign_stmt();
AST_Node* parse_expr_stmt();
AST_Node* parse_expr_p(AST_Node* loperand);
AST_Node* parse_term();
AST_Node* parse_factor();
AST_Node* parse_term_p(AST_Node* loperand);
AST_Node* parse_relop_expr();
AST_Node* parse_relop_expr_p(AST_Node* loperand);
AST_Node* parse_if_stmt();
void      parse_optional_tail(AST_Node** else_stmts);
AST_Node* parse_while_stmt();
AST_Node* parse_condition();
AST_Node* parse_return_stmt();
void 	  create_AST_node_list(AST_Node** head);
void      match(unsigned int symbol);
int       expect(unsigned int symbol);

/* ast.c */

AST_Node* create_AST_node();
void      add_to_stmt_node_list(AST_Node** head, AST_Node* stmt);
AST_Node* create_AST_ID_node(HT_item*);
AST_Node* create_AST_const_node(Data_Type data_type, Literal_Type literal);
AST_Node* create_AST_if_node(AST_Node* condition, AST_Node* if_branch, AST_Node* else_branch);
AST_Node* create_AST_while_node(AST_Node* condition, AST_Node* while_branch);
AST_Node* create_AST_assign_node(HT_item* item, AST_Node* value);
AST_Node* create_AST_arithm_node(Arithm_Op operator, AST_Node* left, AST_Node* right);
AST_Node* create_AST_relat_node(Relat_Op operator, AST_Node* left, AST_Node* right);
AST_Node* create_AST_return_node(int ret_type, char* ret_val);
void      print_AST(AST_Node* node, FILE* fp);
void      free_AST(AST_Node* node);

/* table.c */

H_table*      init_symtable();
unsigned long hash_f(char* key);
HT_item*      create_item(Token token, char* lexm);
H_table*      create_table(unsigned int size);
void          ht_insert(H_table* table, Token tok, char* lexm);
Token         get_token(H_table* table, char* lexm);
void 	      handle_collision(H_table* table, unsigned long index, HT_item* item);
void          print_table(H_table* table, FILE* fp);
Linked_List*  create_list();
Linked_List*  list_insert(Linked_List* list, HT_item* item);
Linked_List** create_buckets(H_table* table);
void          free_table(H_table* table);
void          free_item(HT_item* item);
void          free_list(Linked_List* list);
void          free_buckets(H_table* table);

#endif
