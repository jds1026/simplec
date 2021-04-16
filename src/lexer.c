#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "simplec.h"

/* 
state and start are used for
transitioning between different
starting states depending
on the current input symbol
*/

static unsigned int state = 0;
static unsigned int start = 0;

Literal_Type literal;

/* switch starting state */

static int fail()
{
	switch (state) {
		case 0: {

			/* character check */

			start = 3;
			break;
		}
		case 3: {

			/* number check */

			start = 7;
			break;
		}
		case 7: {

			/* arithop check */

			start = 17;
			break;
		}
		case 17: {

			/* relop check */

			start = 24;
			break;
		}
		case 24: {

			/* punct check */

			start = 36;
			break;
		}
		case 36: {

			/* token error */
			
			fprintf(stderr, "lexer error: invalid token\n");
			exit(1);
		}
	}
	return start;
}

/* Lexer */

Token lexan()
{
	char c;
	Token tok;
	char lexbuff[BUFFSIZE];

	while (1) {
		static unsigned int idx = 0; 
		switch (state) {
			case 0: {
				c = fgetc(source);
				if (c == BLANK || c == TAB || c == NEWLINE) {
					state = 0;

					if (c == NEWLINE) {
						lineno += 1;
					}
					
					break;
				}
				else if (isalpha(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 1;
					break;
				}
				else if (c == EOF) {
					return 260;
				}
				else {
					state = fail();
					break;
				}
			}
			case 1: {
				c = fgetc(source);
				if (isalnum(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 1;
					break;
				}
				else {
					state = 2;
				}
			}
			case 2: {

				/* ID or keyword */

				ungetc(c, source);
				lexbuff[idx] = '\0';

				if (!token_is_keyword(lexbuff, &tok)) {
					ht_insert(ht, TK_ID, lexbuff);
					tok = get_token(ht, lexbuff);
				}

				if (tok == TK_MAIN) {
					ht_insert(ht, TK_MAIN, lexbuff);
				}

				state = 0;
				idx = 0;
				return tok;
			}
			case 3: {
				if (c == '\'') {
					state = 4;
					break;
				}
				else {
					state = fail();
					break;
				}
			}
			case 4: {
				c = fgetc(source);
				if (isalpha(c)) {
					literal.type = T_CHAR;
					literal.val.c = c;

					state = 5;
					break;
				}
			}
			case 5: {
				c = fgetc(source);
				if (c == '\'') {
					state = 6;
					break;
				}
			}
			case 6: {
				state = 0;
				return TK_CHARACTER;
			}
			case 7: {
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 8;
					break;
				}
				else {
					state = fail();
					break;
				}
			}
			case 8: {
				c = fgetc(source);
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 8;
					break;
				}
				else if (c == '.') {
					lexbuff[idx] = c;
					idx++;
					state = 9;
					break;
				}
				else {
					state = 15;
					break;
				}
			}
			case 9: {
				c = fgetc(source);
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 10;
					break;
				}
			}
			case 10: {
				c = fgetc(source);
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 10;
					break;
				}
				else if (c == 'E') {
					lexbuff[idx] = c;
					idx++;
					state = 11;
					break;
				}
				else {
					state = 16;
					break;
				}
			}
			case 11: {
				c = fgetc(source);
				if (c == '+' || c == '-') {
					lexbuff[idx] = c;
					idx++;
					state = 12;
					break;
				}
			}
			case 12: {
				c = fgetc(source);
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 13;
					break;
				}
			}
			case 13: {
				c = fgetc(source);
				if (isdigit(c)) {
					lexbuff[idx] = c;
					idx++;
					state = 13;
					break;
				}
				else {
					state = 14;
					break;
				}
			}
			case 14: {

				/* float with exponent */

				ungetc(c, source);
				lexbuff[idx] = '\0';
				literal.type = T_EXP;
				literal.val.f = atof(lexbuff);

				state = 0;
				idx = 0;
				return TK_NUM;
			}
			case 15: {

				/* integer */

				ungetc(c, source);
				lexbuff[idx] = '\0';
				literal.type = T_INT;
				literal.val.i = atoi(lexbuff);

				state = 0;
				idx = 0;
				return TK_NUM;
			}
			case 16: {

				/* float */

				ungetc(c, source);
				lexbuff[idx] = '\0';
				literal.type = T_FLOAT;
				literal.val.f = atof(lexbuff);

				state = 0;
				idx = 0;
				return TK_NUM;
			}
			case 17: {
				if (c == '+') {
					state = 18;
					break;
				}
				else if (c == '-') {
					state = 19;
					break;
				}
				else if (c == '*') {
					state = 20;
					break;
				}
				else if (c == '/') {
					state = 21;
					break;
				}
				else {
					state = fail();
					break;
				}
			}
			case 18: {

				/* '+' */

				state = 0;
				return TK_ADD;
			}
			case 19: {

				/* '-' */

				state = 0;
				return TK_SUB;
			}
			case 20: {

				/* '*' */

				state = 0;
				return TK_MUL;
			}
			case 21: {
				c = fgetc(source);
				if (c == '/') {
					state = 22;
					break;
				}
				else {
					state = 23;
					break;
				}
			}
			case 22: {

				/* comments */

				while (c != NEWLINE) {
					c = fgetc(source);
					continue;
				}
				state = 0;
				break;
			}
			case 23: {

				/* '/' */

				ungetc(c, source);

				state = 0;
				return TK_DIV;
			}
			case 24: {
				if (c == '<') {
					state = 25;
					break;
				}
				else if (c == '=') {
					state = 28;
					break;
				}
				else if (c == '>') {
					state = 31;
					break;
				}
				else if (c == '!') {
					state = 34;
					break;
				}
				else {
					state = fail();
					break;
				}
			}
			case 25: {
				c = fgetc(source);
				if (c == '=') {
					state = 26;
					break;
				}
				else {
					state = 27;
					break;
				}
			}
			case 26: {

				/* '<=' */

				state = 0;
				return TK_LE;
			}
			case 27: {

				/* '<' */

				ungetc(c, source);

				state = 0;
				return TK_LT;
			}
			case 28: {
				c = fgetc(source);
				if (c == '=') {
					state = 29;
					break;
				}
				else {
					state = 30;
					break;
				}
			}
			case 29: {

				/* '==' */

				state = 0;
				return TK_EQ;
			}
			case 30: {

				/* '=' */

				ungetc(c, source);

				state = 0;
				return TK_ASSIGN;
			}
			case 31: {
				c = fgetc(source);
				if (c == '=') {
					state = 32;
					break;
				}
				else {
					state = 33;
					break;
				}
			}
			case 32: {

				/* '>=' */

				state = 0;
				return TK_GE;
			}
			case 33: {

				/* '>' */

				ungetc(c, source);

				state = 0;
				return TK_GT;
			}
			case 34: {
				c = fgetc(source);
				if (c == '=') {
					state = 35;
					break;
				}
			}
			case 35: {

				/* '!=' */

				state = 0;
				return TK_NE;
			}
			case 36: {
				if (c == ';') {
					state = 37;
					break;
				}
				if (c == '(') {
					state = 38;
					break;
				}
				if (c == ')') {
					state = 39;
					break;
				}
				if (c == '{') {
					state = 40;
					break;
				}
				if (c == '}') {
					state = 41;
					break;
				}
				else {
					state = fail();
					break;
				}
			}
			case 37: {

				/* ';' */

				state = 0;
				return TK_SMCOLON;
			}
			case 38: {

				/* '(' */

				state = 0;
				return TK_LPAREN;
			}
			case 39: {

				/* ')' */

				state = 0;
				return TK_RPAREN;
			}
			case 40: {

				/* '{' */

				state = 0;
				return TK_LBRACE;
			}
			case 41: {

				/* '}' */

				state = 0;
				return TK_RBRACE;
			}
		}	
	}
}

static bool token_is_keyword(char* input_buff, Token* tok)
{
	bool is_keyword = true;

	if (strcmp(input_buff, "main") == 0) {
		*tok = TK_MAIN;
	}
	else if (strcmp(input_buff, "return") == 0) {
		*tok = TK_RETURN;
	}
	else if (strcmp(input_buff, "while") == 0) {
		*tok = TK_WHILE;
	}
	else if (strcmp(input_buff, "if") == 0) {
		*tok = TK_IF;
	}
	else if (strcmp(input_buff, "else") == 0) {
		*tok = TK_ELSE;
	}
	else if (strcmp(input_buff, "int") == 0) {
		*tok = TK_INT;
	}
	else if (strcmp(input_buff, "float") == 0) {
		*tok = TK_FLOAT;
	}
	else if (strcmp(input_buff, "char") == 0) {
		*tok = TK_CHAR;
	}
	else if (strcmp(input_buff, "void") == 0) {
		*tok = TK_VOID;
	}
	else {
		is_keyword = false;
	}

	return is_keyword;
}
