#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"

/* token constants */
#define EOS		256		/* end-of-stream */
#define ADDOP	257		/* additive operator */
#define MULOP	258		/* multiplicative operator */
#define NUM		259		/* number */

/* prototypes */
int isdigit( int );
int get_token();
void match( int );
tree_t *expr(), *term(), *factor();
int eval_tree( tree_t * );

/* globals */
int current_token;
int current_attribute;

/* main driver */
main()
{
	tree_t *value;

	/*read string, evaluate string */
	while(1) {
		current_token = get_token();
		if ( current_token == EOF ) exit(1);
		value = expr();
		fprintf( stderr, "\n>>> Value = %d\n", eval_tree(value) );
		if ( current_token == EOS ) break;
	}
}

int eval_tree( tree_t *t )
{
	assert( t != NULL );
	switch( t->type ) {
	case NUM:
		return t->attribute.ival;
	case ADDOP:
		if( t->attribute.opval == '+' )
			return eval_tree( t->left ) + eval_tree( t->right );
		else if( t->attribute.opval == '-' )
			return eval_tree( t->left ) - eval_tree( t->right );
		else {
			fprintf( stderr, "ERROR: eval_tree(), unknown ADDOP opval = %d\n", t->attribute.opval );
			exit(1);
		}
	case MULOP:
		if( t->attribute.opval == '*' )
			return eval_tree( t->left ) * eval_tree( t->right );
		else if( t->attribute.opval == '/' )
			return eval_tree( t->left ) / eval_tree( t->right );
		else {
			fprintf( stderr, "ERROR: eval_tree(), unknown MULOP opval = %d\n", t->attribute.opval );
			exit(1);
		}
	default:
		fprintf( stderr, "ERROR: eval_tree(), unknown tree type = %d\n", t->type );
		exit(1);
	}
}

/* recursive-descent (top-down) parser */
tree_t *expr()	//E -> E + T | T => E -> T E' | E' -> T E' | empty
{
	int opval;
	tree_t *value = term();
	while( current_token == ADDOP ) {
		opval = current_attribute;
		match( ADDOP );
		value = make_tree( ADDOP, value, term() );
		value->attribute.opval = opval;
	}
	return value;
}

tree_t *term() //T -> T * F | F => T -> T -> F T', T' -> * F T' | empty
{
	int opval;
	tree_t *value = factor();
	while( current_token == MULOP ) {
		opval = current_attribute;
		match( MULOP );
		value = make_tree( MULOP, value, factor() );
		value->attribute.opval = opval;
	}
	return value;
}

tree_t *factor() //F -> ( E ) | NUM
{
	tree_t *value;
	if( current_token == '(' ) {
		match( '(' );
		value = expr();
		match( ')' );
		return value;
	}
	else if( current_token == NUM ) {
		value = make_tree( NUM, NULL, NULL );
		value->attribute.ival = current_attribute;
		match( NUM );
		return value;
	}
	else {
		fprintf(stderr, "ERROR: factor(): unexpected token %d\n", current_token);
		exit(1);
	}
}


/* scanner: read token from input stream */
int get_token()
{
	int c;
	int value;
	
	while(1) {
		switch( c = getchar() ) {
		case ' ': case '\t': /* whitespaces */
			continue;
		case '(': case ')':
			fprintf( stderr, "[%c]", c );
			return c;
		case '+': case '-':
			current_attribute = c;
			fprintf( stderr, "[ADDOP:%c]", c );
			return ADDOP;
		case '*': case '/':
			current_attribute = c;
			fprintf( stderr, "[MULOP:%c]", c );
			return MULOP;
		default:
			if ( isdigit(c) ) {
				/* fetch a multidigit number */
				value = 0;
				do {
					value = 10*value + (c - '0');
				}
				while( isdigit( c = getchar() ));
				ungetc( c, stdin );
				current_attribute = value;
				fprintf( stderr, "[NUM:%d]", value );
				return NUM;
			}
			else if ( c == '\n' ) {
				fprintf( stderr, "\n" );
				return EOS;
			}
			else if ( c == EOF ) {
				return c;
			}
			else {
				fprintf( stderr, "ERROR: unexpected token in get_token(): %c\n", c );
				exit(1);
			}
		}
	}
}

void match( int expected_token )
{
	if( current_token == expected_token ) {
		current_token = get_token();
	}
	else {
		fprintf(stderr, "ERROR: match(): expected_token = %d, current_token = %d\n", expected_token, current_token );
		exit(1);
	}
}
