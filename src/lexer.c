#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "simplec.h"

// state and start are used for 
// transitioning between different
// starting states depending
// on the current input symbol
static unsigned int state = 0;
static unsigned int start = 0;

extern H_table* ht;
extern FILE* source;

// change starting state
int fail()
{
	switch (state) {
		case 0: {
			// character check
			start = 3;
			break;
		}
		case 3: {
			// number check
			start = 7;
			break;
		}
		case 7: {
			// arithop check
			start = 17;
			break;
		}
		case 17: {
			// relop check
			start = 24;
			break;
		}
		case 24: {
			// punct check
			start = 36;
			break;
		}
		case 36: {
			// error 
			fprintf(stderr, "lexer error: invalid token\n");
			exit(1);
		}
	}
	return start;
}

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
					break;
				}
				// if ID or keyword
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
				// ID or keyword insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_ID, lexbuff);
				tok = get_token(ht, lexbuff);
				
				state = 0;
				idx = 0;
				return tok;
			}
			case 3: {
				if (c == '\'') {
					lexbuff[idx] = c;
					idx++;
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
					lexbuff[idx] = c;
					idx++;
					state = 5;
					break;
				}
			}
			case 5: {
				c = fgetc(source);
				if (c == '\'') {
					lexbuff[idx] = c;
					idx++;
					state = 6;
					break;
				}
			}
			case 6: {
				// char insert
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_CHARACTER, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;	
				return tok;
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
				// exponential insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_NUM, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 15: {
				// int insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_NUM, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 16: {
				// float insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_NUM, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
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
				// '+' insert
				lexbuff[idx] = c;
				idx++;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_ADD, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 19: {
				// '-' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_SUB, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 20: {
				// '*' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_MUL, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 21: {
				lexbuff[idx++] = c;
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
				// handle comments
				while (c != NEWLINE) {
					c = fgetc(source);
					continue;
				}

				idx = 0;
				state = 0;
				break;
			}
			case 23: {
				// '/' insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_DIV, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
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
				lexbuff[idx++] = c;
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
				// '<=' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_LE, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 27: {
				// '<' insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_LT, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 28: {
				lexbuff[idx++] = c;
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
				// '==' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_EQ, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 30: {
				// '=' insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_ASSIGN, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 31: {
				lexbuff[idx++] = c;
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
				// '>=' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_GE, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 33: {
				// '>' insert
				ungetc(c, source);
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_GT, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 34: {
				lexbuff[idx++] = c;
				c = fgetc(source);
				if (c == '=') {
					state = 29;
					break;
				}
			}
			case 35: {
				// '!=' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_NE, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
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
				// ';' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_SMCOLON, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 38: {
				// '(' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_LPAREN, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 39: {
				// ')' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_RPAREN, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 40: {
				// '{' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_LBRACE, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
			case 41: {
				// '}' insert
				lexbuff[idx++] = c;
				lexbuff[idx] = '\0';
				ht_insert(ht, lexbuff, TK_RBRACE, lexbuff);
				tok = get_token(ht, lexbuff);

				state = 0;
				idx = 0;
				return tok;
			}
		}	
	}
}