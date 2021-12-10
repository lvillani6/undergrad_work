#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* token constants */
#define EOS		256		/* end-of-stream */
#define ADDOP	257		/* additive operator */
#define MULOP	258		/* multiplicative operator */
#define NUM		259		/* number */

/* prototypes */
int isdigit( int );
int get_token();
int expr(), term(), factor();
void match( int );

/* globals */
int current_token;
int current_attribute;

/* main driver */
main()
{
	int value;

	/*read string, evaluate string */
	while(1) {
		current_token = get_token();
		if ( current_token == EOF ) exit(1);
		value = expr();
		fprintf( stderr, "\n>>> Value = %d\n", value);
		if ( current_token == EOS ) break;
	}
}

/* recursive-descent (top-down) parser */
int expr()	//E -> E + T | T => E -> T E' | E' -> T E' | empty
{
	int value = term();
	while( current_token == ADDOP ) {
		match( ADDOP );
		value += term();
	}
	return value;
}

int term() //T -> T * F | F => T -> T -> F T', T' -> * F T' | empty
{
	int value = factor();
	while( current_token == MULOP ) {
		match( MULOP );
		value *= factor();
	}
	return value;
}

int factor() //F -> ( E ) | NUM
{
	int value;
	if( current_token == '(' ) {
		match( '(' );
		value = expr();
		match( ')' );
		return value;
	}
	else if( current_token == NUM ) {
		value = current_attribute;
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
		case '+':
			fprintf( stderr, "[ADDOP:%c]", c );
			return ADDOP;
		case '*':
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
