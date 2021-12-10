%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "y.tab.h"

int yylex();
int yyerror( char *s );
int eval_tree( tree_t * );
%}

%union {
	int ival;
	int opval;
	tree_t *tval;
}

%token EOS
%token <ival> NUM
%token ADDOP
%token MULOP

%type <tval> expr

%%

expr_list: expr_list expr '\n'	{ fprintf( stderr, ">>> Value = %d\n", eval_tree( $2 )); }
	| expr '\n'										{ fprintf( stderr, ">>> Value = %d\n", eval_tree( $1 )); }
	;

expr: expr ADDOP expr	{ $$ = make_tree( ADDOP, $1, $3); }
	| expr MULOP expr		{ $$ = make_tree( MULOP, $1, $3); }
	| '(' expr ')'			{ $$ = $2; }
	| NUM								{ $$ = make_tree( NUM, NULL, NULL); $$->attribute.ival = $1; }
	;

%%

int main()
{
	yyparse();
}
