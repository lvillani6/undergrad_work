#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

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

