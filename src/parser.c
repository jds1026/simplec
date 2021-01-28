#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simplec.h"

extern FILE* source;
extern unsigned int lineno;
extern HT_item* item_ptr;
extern Literal_Type literal;


static unsigned int lookahead; /* next token returned by lexer */
static unsigned int op;        /* used for ast construction */

void parse(AST_Node** stmts_head)
{
	lineno = 1;
	lookahead = lexan();
	func_block(stmts_head);
	match(END);
}

void func_block(AST_Node** stmts_head)
{
	match(TK_INT);
	match(TK_MAIN);
	match(TK_LPAREN);
	match(TK_VOID);
	match(TK_RPAREN);
	block(stmts_head);
}
 
void block(AST_Node** stmts_head)
{
	if (expect(TK_LBRACE)) {
		match(TK_LBRACE);
		parse_stmts(stmts_head);
		match(TK_RBRACE);
	}
	else {
		fprintf(stderr, "error on line %d\n", lineno);
		exit(1);
	}
}

void parse_stmts(AST_Node** stmts_head)
{
	while (1) {
		switch (lookahead) {
			case TK_ID: {
				AST_Node* assign_node = parse_assign_stmt();
				add_to_stmt_node_list(stmts_head, assign_node);
				break;
			}
			case TK_INT: {
				match(TK_INT);
				AST_Node* assign_node = parse_assign_stmt();
				add_to_stmt_node_list(stmts_head, assign_node);
				break;
			}
			case TK_FLOAT: {
				match(TK_FLOAT);
				AST_Node* assign_node = parse_assign_stmt();
				add_to_stmt_node_list(stmts_head, assign_node);
				break;
			}
			case TK_CHAR: {
				match(TK_CHAR);
				AST_Node* assign_node = parse_assign_stmt();
				add_to_stmt_node_list(stmts_head, assign_node);
				break;
			}
			case TK_IF: {
				AST_Node* if_node = parse_if_stmt();
				add_to_stmt_node_list(stmts_head, if_node);
				break;
			}
			case TK_WHILE: {
				AST_Node* while_node = parse_while_stmt();
				add_to_stmt_node_list(stmts_head, while_node);
				break;
			}
			case TK_RETURN: {
				AST_Node* return_node = parse_return_stmt();
				add_to_stmt_node_list(stmts_head, return_node);
				break;
			}
			default: {
				return;
			}
		} 
	}
}


AST_Node* parse_assign_stmt()
{
	HT_item* id = item_ptr;
	match(TK_ID);
	match(TK_ASSIGN);

	/* 
	linecheck is used in the case of a
	missing semicolon with a newline present
	*/
	
	unsigned int linecheck = lineno;

	AST_Node* expr = parse_expr_stmt();

	if (expect(TK_SMCOLON)) {
		match(TK_SMCOLON);
		AST_Node* assign_node = create_AST_assign_node(id, expr);
		return assign_node;
	}
	else if (linecheck == lineno) {
		fprintf(stderr, "error on line %d\n", lineno);
		exit(1);
	}
	else {
		fprintf(stderr, "error on line %d\n", linecheck);
		exit(1);
	}
}

AST_Node* parse_expr_stmt()
{
	AST_Node* term = parse_term();
	AST_Node* expr_p = parse_expr_p(term);

	if (expr_p == NULL) {
		return term;
	}
	else {
		return expr_p;
	}
}

AST_Node* parse_expr_p(AST_Node* loperand)
{
	AST_Node* arithm = NULL;

	switch (lookahead) {
		case TK_ADD: {
			match(TK_ADD);
			AST_Node* term = parse_term();
			AST_Node* expr_p = parse_expr_p(term);
			if (expr_p == NULL) {
				arithm = create_AST_arithm_node(ADD_OP, loperand, term);
			}
			else {
				arithm = create_AST_arithm_node(ADD_OP, loperand, expr_p);
			}
			break;
		}
		case TK_SUB: {
			match(TK_SUB);
			AST_Node* term = parse_term();
			AST_Node* expr_p = parse_expr_p(term);
			arithm = create_AST_arithm_node(SUB_OP, loperand, term);
			if (expr_p == NULL) {
				arithm = create_AST_arithm_node(SUB_OP, loperand, term);
			}
			else {
				arithm = create_AST_arithm_node(SUB_OP, loperand, expr_p);
			}
			break;
		}
		default: {
			break;
		}
	}

	return arithm;
}

AST_Node* parse_term()
{
	AST_Node* factor = parse_factor();
	AST_Node* term_p = parse_term_p(factor);

	if (term_p == NULL) {
		return factor;
	}
	else {
		return term_p;
	}
}

AST_Node* parse_factor()
{
	switch (lookahead) {
		case TK_LPAREN: {
			match(TK_LPAREN);
			AST_Node* expr = parse_expr_stmt();
			match(TK_RPAREN);
			return expr;
		}
		case TK_ID: {
			AST_Node* id = create_AST_ID_node(item_ptr);
			match(TK_ID);
			return id;
		}
		case TK_NUM: {
			AST_Node* num;
			if (literal.type == T_INT) {
				num = create_AST_const_node(T_INT, literal);
			}
			else if (literal.type == T_EXP) {
				num = create_AST_const_node(T_EXP, literal);
			}
			else if (literal.type == T_FLOAT) {
				num = create_AST_const_node(T_FLOAT, literal);
			}
			match(TK_NUM);
			return num;
		}
		case TK_CHARACTER: {
			AST_Node* chr = create_AST_const_node(T_CHAR, literal);
			match(TK_CHARACTER);
			return chr;
		}
		default: {
			fprintf(stderr, "error on line %d\n", lineno); 
			exit(1);
		}
	}
}

AST_Node* parse_term_p(AST_Node* loperand)
{
	AST_Node* arithm = NULL;

	switch (lookahead) {
		case TK_MUL: {
			match(TK_MUL);
			AST_Node* factor = parse_factor();
			AST_Node* term_p = parse_term_p(factor);
			if (term_p == NULL) {
				arithm = create_AST_arithm_node(MUL_OP, loperand, factor);
			}
			else {
				arithm = create_AST_arithm_node(MUL_OP, loperand, term_p);
			}
			break;
		}
		case TK_DIV: {
			match(TK_DIV);
			AST_Node* factor = parse_factor();
			AST_Node* term_p = parse_term_p(factor);
			if (term_p == NULL) {
				arithm = create_AST_arithm_node(DIV_OP, loperand, factor);
			}
			else {
				arithm = create_AST_arithm_node(DIV_OP, loperand, term_p);
			}
			break;
		}
		default: {
			break;
		}
	}

	return arithm;
}

AST_Node* parse_relop_expr()
{
	AST_Node* expr_stmt = parse_expr_stmt();
	AST_Node* relop_expr = parse_relop_expr_p(expr_stmt);

	if (relop_expr == NULL) {
		return expr_stmt;
	}
	else {
		return relop_expr;
	}
}

AST_Node* parse_relop_expr_p(AST_Node* loperand)
{
	switch (lookahead) {
		case TK_LT: {
			match(TK_LT);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(LT_OP, loperand, expr_stmt);
			return relat;
		}
		case TK_LE: {
			match(TK_LE);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(LE_OP, loperand, expr_stmt);
			return relat;
		}
		case TK_EQ: {
			match(TK_EQ);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(EQ_OP, loperand, expr_stmt);
			return relat;
		}
		case TK_GE: {
			match(TK_GE);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(GE_OP, loperand, expr_stmt);
			return relat;
		}
		case TK_GT: {
			match(TK_GT);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(GT_OP, loperand, expr_stmt);
			return relat;
		}
		case TK_NE: {
			match(TK_NE);
			AST_Node* expr_stmt = parse_expr_stmt();
			AST_Node* relat = create_AST_relat_node(NE_OP, loperand, expr_stmt);
			return relat;
		}
		default: { 
			return NULL; 
		}
	}
}

AST_Node* parse_if_stmt()
{
	match(TK_IF);
	match(TK_LPAREN);
	AST_Node* condition = parse_condition();
	match(TK_RPAREN);
	AST_Node* if_branch_head = NULL;
	block(&if_branch_head);
	AST_Node* else_branch_head = NULL;
	parse_optional_tail(&else_branch_head);

	AST_Node* if_stmt = create_AST_if_node(condition, if_branch_head, else_branch_head);
	return if_stmt;
}

void parse_optional_tail(AST_Node** else_stmts)
{
	if (expect(TK_ELSE)) {
		match(TK_ELSE);
		block(else_stmts);
	}
}

AST_Node* parse_while_stmt()
{
	match(TK_WHILE);
	match(TK_LPAREN);
	AST_Node* condition = parse_condition();
	match(TK_RPAREN);
	match(TK_LBRACE);
	AST_Node* while_stmts_head = NULL;
	parse_stmts(&while_stmts_head);
	match(TK_RBRACE);

	AST_Node* while_stmt = create_AST_while_node(condition, while_stmts_head);
	return while_stmt;
}

AST_Node* parse_condition()
{
	AST_Node* condition = parse_relop_expr();
	return condition;
}

AST_Node* parse_return_stmt()
{
	match(TK_RETURN);
	if (lookahead - 1 == 0) { /* TK_RETURN has value 1 */
		match(TK_NUM);
		match(TK_SMCOLON);
		AST_Node* return_stmt = create_AST_return_node(TK_NUM, "0");
		return return_stmt;
	}
	else {
		fprintf(stderr, "error on line %d\n", lineno);
		exit(1);
	}
}

void match(unsigned int symbol)
{
	if (symbol == lookahead) {
		lookahead = lexan();
		return;
	}
	else {
		fprintf(stderr, "error on line %d\n", lineno);
		exit(1);
	}
}

int expect(unsigned int symbol)
{
	if (symbol == lookahead) {
		return 1;
	}
	return 0;
}