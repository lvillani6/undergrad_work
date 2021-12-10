%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "list.h"
#include "y.tab.h"

int yylex();
int yyerror( char *s );
int eval_tree( tree_t * );

list_t *symbol_table;
%}

%union {
	int ival;		/* attribute for NUM */
	int opval;		/* attribute for ADDOP, MULOP, ASSIGNOP */
	char *sval;		/* attribute for ID */

	tree_t *tval;
}

%token EOS
%token <ival> NUM
%token <sval> ID
%token <opval> ASSIGNOP
%token <opval> ADDOP
%token <opval> MULOP

%type <tval> stmt
%type <tval> expr

%%

stmt_list: stmt_list stmt '\n'	{ fprintf( stderr, ">>> Value = %d\n", eval_tree( $2 )); print_tree( $2, 0 ); }
	| stmt '\n'					{ fprintf( stderr, ">>> Value = %d\n", eval_tree( $1 )); print_tree( $1, 0 ); }
	;

stmt: ID ASSIGNOP expr	{ $$ = make_tree( ASSIGNOP, make_ident( symbol_table = insert_list( symbol_table, $1 )), $3 ); }
	| expr				{ $$ = $1; }
	;

expr: expr ADDOP expr	{ $$ = make_tree( ADDOP, $1, $3); $$->attribute.opval = $2; }
	| expr MULOP expr	{ $$ = make_tree( MULOP, $1, $3); $$->attribute.opval = $2; }
	| '(' expr ')'		{ $$ = $2; }
	| NUM				{ $$ = make_tree( NUM, NULL, NULL); $$->attribute.ival = $1; }
	| ID				{ $$ = make_ident( search_list( symbol_table, $1 )); }
	;

/* expr: expr ADDOP term
	| term
	;
term: term MULOP factor
	| factor
	;
factor: '(' expr ')'
	| NUM
	; */

%%

int main()
{
	symbol_table = NULL;
	yyparse();
}
