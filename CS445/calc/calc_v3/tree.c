#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
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

void free_tree( tree_t *t )
{
	free( t );	/* buggy and v bad */
}

int eval_tree( tree_t *t )
{
	assert( t!= NULL );
	switch( t->type ) {
		case NUM: return t->attribute.ival;
		case ADDOP: return eval_tree( t->left ) + eval_tree( t->right );
		case MULOP: return eval_tree( t->left ) * eval_tree( t->right );
		default: fprintf( stderr, "ERROR: eval_tree unknown tree type = %d\n", t->type );
	}
}
