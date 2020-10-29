#include "simplec.h"
#include <stdlib.h>

AST_Node* create_AST_node()
{
	AST_Node* new_node = (AST_Node*)malloc(sizeof(AST_Node));
	new_node = NULL;

	return new_node;
}

void add_to_stmt_node_list(AST_Node** head, AST_Node* stmt)
{
	if (*head == NULL) {
		(*head) = stmt;
		return;
	}

	AST_Node* temp = (*head);
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = stmt;
}

AST_Node* create_AST_ID_node(HT_item* item)
{
	AST_ID_Node* new_node = malloc(sizeof(AST_ID_Node));

	new_node->type = ID_NODE;
	new_node->item = item;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_const_node(Data_Type data_type, char* value)
{
	AST_Const_Node* new_node = malloc(sizeof(AST_Const_Node));

	new_node->type = CONST_NODE;
	new_node->data_type = data_type;
	new_node->value = value;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_if_node(AST_Node* condition, AST_Node* if_branch, AST_Node* else_branch)
{
	AST_If_Node* new_node = malloc(sizeof(AST_If_Node));

	new_node->type = IF_NODE;
	new_node->condition = condition;
	new_node->if_branch = if_branch;
	new_node->else_branch = else_branch;

	new_node->next = NULL;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_while_node(AST_Node* condition, AST_Node* while_branch)
{
	AST_While_Node* new_node = malloc(sizeof(AST_While_Node));

	new_node->type = WHILE_NODE;
	new_node->condition = condition;
	new_node->while_branch = while_branch;
	new_node->next = NULL;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_assign_node(HT_item* item, AST_Node* value)
{
	AST_Assign_Node* new_node = malloc(sizeof(AST_Assign_Node));

	new_node->type = ASSIGN_NODE;
	AST_Node* temp = create_AST_ID_node(item);

	new_node->loperand = temp;
	new_node->value = value;
	new_node->next = NULL;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_arithm_node(Arithm_Op operator, AST_Node* left, AST_Node* right)
{
	AST_Arithm_Node* new_node = malloc(sizeof(AST_Arithm_Node));

	new_node->type = ARITHM_NODE;
	new_node->operator = operator;
	new_node->left = left;
	new_node->right = right;
	new_node->next = NULL;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_relat_node(Relat_Op operator, AST_Node* left, AST_Node* right)
{
	AST_Relat_Node* new_node = malloc(sizeof(AST_Relat_Node));

	new_node->type = RELAT_NODE;
	new_node->operator = operator;
	new_node->left = left;
	new_node->right = right;

	return (AST_Node*)new_node;
}

AST_Node* create_AST_return_node(int ret_type, char* ret_val)
{
	AST_Return_Node* new_node = malloc(sizeof(AST_Return_Node));

	new_node->type = RETURN_NODE;
	new_node->ret_type = ret_type;
	new_node->ret_val = ret_val;

	return (AST_Node*)new_node;
}

void print_AST(AST_Node* node, FILE* fp)
{
	if (node == NULL) {
		return;
	}

	// assign node
	if (node->type == ASSIGN_NODE) {
		AST_Assign_Node* assign_temp = (AST_Assign_Node*)node;
		fprintf(fp, "%11s>>> ASSIGN NODE \'=\' HAS CHILDREN <<<\n", " ");
		print_AST(assign_temp->loperand, fp);
		print_AST(assign_temp->value, fp);

		if (assign_temp->next != NULL) {
			fprintf(fp, "\n");
			print_AST(assign_temp->next, fp);
		}
	}

	// arithm node
	if (node->type == ARITHM_NODE) {
		AST_Arithm_Node* arithm_temp = (AST_Arithm_Node*)node;
		char op;
		if (arithm_temp->operator == ADD_OP) {
			op = '+';
		}
		else if (arithm_temp->operator == SUB_OP) {
			op = '-';
		}
		else if (arithm_temp->operator == MUL_OP) {
			op = '*';
		}
		else if (arithm_temp->operator == DIV_OP) {
			op = '/';
		}
		fprintf(fp, "%11s>>> ARITHMIC OPERATOR \'%c\' HAS CHILDREN <<<\n", " ", op);
		print_AST(arithm_temp->left, fp);
		print_AST(arithm_temp->right, fp);

		if (arithm_temp->next != NULL) {
			fprintf(fp, "\n");
			print_AST(arithm_temp->next, fp);
		}
	}

	// relat node
	else if (node->type == RELAT_NODE) {
		AST_Relat_Node* relat_temp = (AST_Relat_Node*)node;
		char* op;
		if (relat_temp->operator == LT_OP) {
			op = "<";
		}
		else if (relat_temp->operator == LE_OP) {
			op = "<=";
		}
		else if (relat_temp->operator == EQ_OP) {
			op = "==";
		}
		else if (relat_temp->operator == GE_OP) {
			op = ">=";
		}
		else if (relat_temp->operator == GT_OP) {
			op = ">";
		}
		else if (relat_temp->operator == NE_OP) {
			op = "!=";
		}
		fprintf(fp, "%11s>>> RELATIONAL OPERATOR \'%s\' HAS CHILDREN <<<\n", " ", op);
		print_AST(relat_temp->left, fp);
		print_AST(relat_temp->right, fp);
	}

	// if node
	else if (node->type == IF_NODE) {
		AST_If_Node* if_temp = (AST_If_Node*)node;
		fprintf(fp,"\n%11s### IF NODE CONDITION ###\n", " ");
		print_AST(if_temp->condition, fp);

		fprintf(fp, "\n%11s### IF NODE BRANCHES ###\n", " ");
		print_AST(if_temp->if_branch, fp);

		if (if_temp->else_branch != NULL) {
			fprintf(fp, "\n%11s### ELSE NODE BRANCHES ###\n", " ");
			print_AST(if_temp->else_branch, fp);
		}

		if (if_temp->next != NULL) {
			fprintf(fp, "\n");
			print_AST(if_temp->next, fp);
		}
	}

	// while node
	else if (node->type == WHILE_NODE) {
		AST_While_Node* while_temp = (AST_While_Node*)node;
		fprintf(fp, "\n%11s### WHILE NODE CONDITION ###\n", " ");
		print_AST(while_temp->condition, fp);

		fprintf(fp, "\n%11s### WHILE NODE BRANCHES ###\n"," ");
		print_AST(while_temp->while_branch, fp);

		if (while_temp->next != NULL) {
			fprintf(fp, "\n");
			print_AST(while_temp->next, fp);
		}
	}

	// ID node
	else if (node->type == ID_NODE) {
		AST_ID_Node* id_temp = (AST_ID_Node*)node;
		fprintf(fp, "%15sID NODE %s\n", " ", id_temp->item->lexeme);
	}

	// const node
	else if (node->type == CONST_NODE) {
		AST_Const_Node* const_temp = (AST_Const_Node*)node;
		char* type;
		if (const_temp->data_type == T_NUM) {
			type = "number";
		}
		else if (const_temp->data_type == T_CHAR) {
			type = "character";
		}
		fprintf(fp, "%15sCONST NODE %s VALUE %s\n", " ", type, const_temp->value);
	}

	// return node
	else if (node->type == RETURN_NODE) {
		AST_Return_Node* return_temp = (AST_Return_Node*)node;
		fprintf(fp, "%11s>>> MAIN EXITED WITH RETURN VALUE %s <<<\n", " ", return_temp->ret_val);
	}
}

void free_AST(AST_Node* head)
{
	AST_Node* node = head;

	if (node == NULL) {
		return;
	}

	if (node->type == ARITHM_NODE) {
		AST_Arithm_Node* arithm_temp = (AST_Arithm_Node*)node;
		free_AST(arithm_temp->left);
		free_AST(arithm_temp->right);
		free(arithm_temp);
	}
	else if (node->type == RELAT_NODE) {
		AST_Relat_Node* relat_temp = (AST_Relat_Node*)node;
		free_AST(relat_temp->left);
		free_AST(relat_temp->right);
		free(relat_temp);
	}
	else if (node->type == IF_NODE) {
		AST_If_Node* if_temp = (AST_If_Node*)node;
		free_AST(if_temp->condition);
		free_AST(if_temp->if_branch);
		free_AST(if_temp->else_branch);
		free(if_temp);
	}
	else if (node->type == ID_NODE) {
		free(node);
	}
	else if (node->type == CONST_NODE) {
		free(node);
	}
	else if (node->type == WHILE_NODE) {
		AST_While_Node* while_temp = (AST_While_Node*)node;
		free_AST(while_temp->condition);
		free_AST(while_temp->while_branch);
		free(while_temp);
	}
	else if (node->type == ASSIGN_NODE) {
		AST_Assign_Node* assign_temp = (AST_Assign_Node*)node;
		free_AST(assign_temp->loperand);
		free_AST(assign_temp->value);
		free(assign_temp);
	}
	else if (node->type == RETURN_NODE) {
		AST_Return_Node* return_temp = (AST_Return_Node*)node;
		free(return_temp);
	}
}