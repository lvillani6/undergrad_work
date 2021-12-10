#ifndef TREE_H
#define TREE_H

#include "list.h"

typedef struct tree_s {
	int type;		/* token type */
	union {
		int ival;	/* NUM */
		int opval;	/* ADDOP, MULOP */
		list_t *sval;	/* ID: pointer to symbol table */
	} attribute;
	struct tree_s *left;
	struct tree_s *right;
}
tree_t;

tree_t *make_tree( int type, tree_t *left, tree_t *right);
tree_t *make_ident( list_t * );
void free_tree( tree_t *t );
int eval_tree( tree_t * );
void print_tree( tree_t *, int );	/* visual debug: sanity check */

#endif
