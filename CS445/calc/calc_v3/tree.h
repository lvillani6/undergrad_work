#ifndef TREE_H
#define TREE_H

typedef struct tree_s {
	int type;		/* token type */
	union {
		int ival;	/* NUM */
		int opval;	/* ADDOP, MULOP */
	} attribute;
	struct tree_s *left;
	struct tree_s *right;
}
tree_t;

tree_t *make_tree( int type, tree_t *left, tree_t *right);
void free_tree( tree_t *t );
void print_tree( tree_t *, int );	/* visual debug: sanity check */

#endif
