#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "hash.h"
#include "tree.h"
#include "y.tab.h"

/* semantic type checker: returns type of tree */
int eval_tree( tree_t *t )
{
	int left_value, right_value;
	assert( t!= NULL );
	switch( t->type ) {
		case INUM:		return INTEGER;	//t->attribute.ival;
		case RNUM:		return REAL;	//t->attribute.rval;
		case ID:
			if( t->attribute.sval == NULL) {
				fprintf(stderr, "\nERROR: local object undefined\n" );
				exit(1);
			}
			return t->attribute.sval->type_value;
		case ASSIGNOP:	return ( t->left->attribute.sval->type_value = eval_tree( t->right ) );
		case ADDOP:
		case MULOP:
			left_value = eval_tree( t->left );
			right_value = eval_tree( t->right );
			if( left_value == right_value )	/* potentially buggy */
				return left_value;	/* strict type checking */
			else return ERROR_TYPE;
		case RELOP:
			left_value = eval_tree( t->left );
			right_value = eval_tree( t->right );
			if( left_value == right_value )	/* potentially buggy */
				return BOOLEAN_TYPE;	/* implicit bool type */
			else return ERROR_TYPE;
		case FUNCTION_CALL:		return t->left;
		default: fprintf( stderr, "ERROR: eval_tree() unknown tree type = %d\n", t->type );
	}
}

/* semantic action: inherited type attributes */
void set_identifier_types( tree_t *id_list, int type_val )
{
	if( id_list == NULL ) return;
	if( id_list->type == COMMA ) {
		(id_list->right)->attribute.sval->type_value = type_val;	/* record ID type */
		set_identifier_types( id_list-> left, type_val );
	}
	else {
		id_list->attribute.sval->type_value = type_val;	/* record ID type */
	}
}




list_t* argument_type_list( tree_t *arguments )
{
    list_t* type_list = NULL;
    arg_type_list_helper( arguments, &type_list );
	reverse( &type_list );

    return type_list;
}

void arg_type_list_helper( tree_t *arguments, list_t **type_list )
{
	if( arguments == NULL ) return;

	arg_type_list_helper(arguments->left, type_list);
	if( arguments->type == ID ) {
		// fprintf( stderr, "(%s, %d)", arguments->attribute.sval->name, arguments->attribute.sval->type_value );
		list_t* p = (list_t *)malloc( sizeof(list_t) );
		p = arguments->attribute.sval;
		p->next = *type_list;
		*type_list = p;
	}
	arg_type_list_helper(arguments->right, type_list);
}

void reverse(list_t** top)
{
	list_t* prev = NULL;
	list_t* next = NULL;
	list_t* current = *top;
	while ( current != NULL ) {
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	*top = prev;
}
