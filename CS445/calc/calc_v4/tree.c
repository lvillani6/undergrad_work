#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "list.h"
#include "y.tab.h"

tree_t *make_tree( int type, tree_t *left, tree_t *right)
{
	tree_t *p = (tree_t *)malloc( sizeof( tree_t ));
	assert( p != NULL);
	p->type = type;
	p->left = left;
	p->right = right;
	return p;
}

tree_t *make_ident( list_t *symptr )
{
	tree_t *p = make_tree( ID, NULL, NULL );
	assert( p != NULL );
	p->attribute.sval = symptr;	/* should be a symbol table pointer */
	return p;
}

void free_tree( tree_t *t )
{
	free( t );	/* buggy and v bad */
}

void print_tree( tree_t *t, int spaces )
{
	int i;
	if( t == NULL ) return;
	for( i = 0; i < spaces; i++ ) {
		fprintf( stderr, " " );
	}
	switch( t->type ) {
		case NUM:		fprintf( stderr, "[NUM:%d]", t->attribute.ival );
			break;
		case ID:		fprintf( stderr, "[ID:%s]", t->attribute.sval->name );
			break;
		case ASSIGNOP:	fprintf( stderr, "[ASSIGNOP:%d]", t->attribute.opval );
			break;
		case ADDOP:		fprintf( stderr, "[ADDOP:%c]", t->attribute.opval );
			break;
		case MULOP:		fprintf( stderr, "[MULOP:%c]", t->attribute.opval );
			break;
		default:		fprintf( stderr, "ERROR: print_tree() unknown tree type = %d\n", t->type );
			exit(1);
	}
	fprintf( stderr, "\n" );

	print_tree( t->left, spaces+4 );
	print_tree( t->right, spaces+4 );
}

/* semantic evaluator */
int eval_tree( tree_t *t )
{
	assert( t!= NULL );
	switch( t->type ) {
		case NUM:		return t->attribute.ival;
		case ID:		return t->attribute.sval->value;
		case ASSIGNOP:	return ( t->left->attribute.sval->value = eval_tree( t->right ) );
		case ADDOP:		return eval_tree( t->left ) + eval_tree( t->right );
		case MULOP:		return eval_tree( t->left ) * eval_tree( t->right );
		default: fprintf( stderr, "ERROR: eval_tree() unknown tree type = %d\n", t->type );
	}
}
