%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "hash.h"
#include "tree.h"
#include "y.tab.h"

int yylex();
int yyerror( char *s );
void set_identifier_types( tree_t *, int );
int eval_tree( tree_t *);

hash_t *symbol_table;
int left_tmp, right_tmp;
tree_t* subprogram_id;
list_t* subprogram_ptr;

%}

%union {
	int ival;		/* INUM */
	float rval;		/* RNUM */
	int opval;		/* RELOP, ADDOP, MULOP */
	int unval;		/* NOT */
	//int fval;	/* Value returned by function call. */
	char *sval;		/* ID */

	tree_t *tval;
}

%token <ival> INUM
%token <rval> RNUM
%token <sval> ID
%token <opval> RELOP
%token LT LE GT GE NE EQ
%token <opval> ADDOP
%token PLUS MINUS OR
%token <opval> MULOP
%token STAR SLASH MOD AND
%token <unval> NOT
%token ASSIGNOP

%token PROGRAM
%token BBEGIN END
%token IF THEN ELSE
%token WHILE DO
%token FOR TO
%token VAR
%token ARRAY OF
%token INTEGER REAL
%token FUNCTION
%token PROCEDURE
%token FUNCTION_CALL
%token DOTDOT

%token COMMA
%token ERROR_TYPE
%token BOOLEAN_TYPE

%type <tval> identifier_list
%type <tval> type
%type <tval> standard_type
%type <tval> id

%type <tval> variable
%type <tval> statement
%type <tval> statement_list

%type <tval> procedure_statement
%type <tval> compound_statement
%type <tval> optional_statements

%type <tval> subprogram_head

%type <tval> arguments
%type <tval> parameter_list

%type <tval> expression_list
%type <tval> expression
%type <tval> simple_expression
%type <tval> term
%type <tval> factor

%%

start: PROGRAM ID '(' identifier_list ')' ';'
	{ symbol_table = push_scope( symbol_table ); }
	declarations
	subprogram_declarations
	compound_statement
	{ symbol_table = pop_scope( symbol_table ); }
	'.'
	;


identifier_list: ID
		{ $$ = make_ident( insert_hash_check( symbol_table, $1 )); }
	| identifier_list ',' ID
		{ $$ = make_tree( COMMA, $1, make_ident( insert_hash_check( symbol_table, $3 ))); }
	;

declarations: declarations VAR identifier_list ':' type ';'
		{ set_identifier_types( $3, $5 ); }
	| /* empty */
	;

type: standard_type
		{ $$ = $1; }
	| ARRAY '[' INUM DOTDOT INUM ']' OF standard_type
		{ $$ = ARRAY; /*really incomplete */ }
	;

standard_type: INTEGER
		{ $$ = INTEGER; }
	| REAL
		{ $$ = REAL; }
	;

subprogram_declarations: subprogram_declarations subprogram_declaration ';'
	| /* empty */
	;

subprogram_declaration:
	subprogram_head
	declarations
	subprogram_declarations
	compound_statement
	{	/* Make this better. */
		/* fprintf( stderr, "here" ); */
		if( $1->type == FUNCTION ) {
			if( $4 == NULL ) {
				fprintf( stderr, "\nERROR: function missing return statement\n" );
				exit(1);
			}
			if( $4->type != ASSIGNOP ) {
				fprintf( stderr, "\nERROR: function return statement must be assignment\n" );
				exit(1);
			}
			if(symbol_table->scope_owner->name != $4->left->attribute.sval->name) {
				fprintf( stderr, "\nERROR: function return statement must match definition\n" );
				exit(1);
			}
		}
		if( $1->type == PROCEDURE ) {
			fprintf( stderr, "\n" );
		}
		int i;
		list_t* tmp;
		char* tmp_name = symbol_table->scope_owner->name;
		hash_t* tmp2 = find_scope( symbol_table, tmp_name );

		symbol_table = pop_scope( symbol_table );
	}
	;

subprogram_head: FUNCTION ID
		{
			subprogram_ptr = insert_hash_check( symbol_table, $2 );
			subprogram_ptr->type_value = FUNCTION;
			symbol_table = push_scope( symbol_table );
			symbol_table->scope_owner = subprogram_ptr;
		}
		arguments ':' standard_type ';'
		{
			subprogram_ptr = symbol_table->scope_owner;
			subprogram_ptr->arg_list = argument_type_list( $4 );
			subprogram_ptr->type_value = $6;

			$$ = make_tree( FUNCTION, NULL, NULL );
		}
	| PROCEDURE ID
		{
			subprogram_ptr = insert_hash_check( symbol_table, $2 );
			subprogram_ptr->type_value = PROCEDURE;
			symbol_table = push_scope( symbol_table );
			symbol_table->scope_owner = subprogram_ptr;
		}
		arguments ';'
		{
			subprogram_ptr = symbol_table->scope_owner;
			subprogram_ptr->arg_list = argument_type_list( $4 );

			$$ = make_tree( PROCEDURE, NULL, NULL );
		}
	;

arguments: '(' parameter_list ')'
		{ $$ = $2; }
	| /* empty */
	;

parameter_list: identifier_list ':' type
		{
			$$ = $1;
			set_identifier_types( $1, $3 );
		}
	| parameter_list ';' identifier_list ':' type
		{
			$$ = make_tree( COMMA, $1, $3);
			set_identifier_types( $3, $5 );
		}
	;


compound_statement:
	BBEGIN
		optional_statements
	END
		{ $$ = $2; }
	;

optional_statements: statement_list { $$ = $1; }
	| /* empty */
	;

statement_list: statement { $$ = $1; }
	| statement_list ';' statement
	;

statement: variable ASSIGNOP expression
		{	/* semantic type checking here */
			$$ = make_tree( ASSIGNOP, $1, $3 );
			left_tmp = eval_tree( $1 );
			right_tmp = eval_tree( $3 );
			if( left_tmp != right_tmp ) {	/* this is not exactly right */
				fprintf( stderr, "\nERROR: type mismatch ID[%s:%d] and expr[%d]\n",
					($1)->attribute.sval->name, ($1)->attribute.sval->type_value, right_tmp);
				exit(1);
			}
		}
	| procedure_statement
		{ $$ = $1; }
	| compound_statement
		{ $$ = $1; }
	| IF expression THEN statement ELSE statement
		{
			$$ = make_tree( IF, $2, make_tree( ELSE, $4, $6) );
			left_tmp = eval_tree( $2 );
			if( left_tmp != BOOLEAN_TYPE ) {
				fprintf( stderr, "\nERROR: while conditional must be of type BOOLEAN\n", left_tmp );
				exit(1);
			}
			/* fprintf( stderr, "\n"); */
			/* print_tree($$,0); */
		}
	| IF expression THEN statement
		{
			$$ = make_tree( IF, $2, make_tree( ELSE, $4, NULL) );
			left_tmp = eval_tree( $2 );
			if( left_tmp != BOOLEAN_TYPE ) {
				fprintf( stderr, "\nERROR: while conditional must be of type BOOLEAN\n", left_tmp );
				exit(1);
			}
			/* print_tree($$,0); */
		}
	| WHILE expression DO statement
		{
			$$ = make_tree( WHILE, $2, $4 );
			left_tmp = eval_tree( $2 );
			if( left_tmp != BOOLEAN_TYPE ) {
				fprintf( stderr, "\nERROR: while conditional must be of type BOOLEAN\n", left_tmp );
				exit(1);
			}
			/* print_tree($$,0); */
		}
	| FOR statement TO expression DO statement
		{
			$$ = make_tree( FOR, make_tree( TO, $2, $4 ), $6);
			left_tmp = eval_tree( $2 );
			right_tmp = eval_tree( $4 );
			/* if( left_tmp != right_tmp ) {
				fprintf( stderr, "\nERROR: type mismatch ID[%s:%d] and expr[%d]\n",
					($1)->attribute.sval->name, ($1)->attribute.sval->type_value, right_tmp);
				exit(1);
			} */
		}
	;

variable: id
		{
			$$ = $1;
			int is_local = search_hash(symbol_table, $$) == NULL;
			int is_scope = $$->attribute.sval == symbol_table->scope_owner;
			if(  is_local && !is_scope ) {
				fprintf( stderr, "\nERROR: function updating non-local variable\n" );
				exit(1);
			}
		}
	| id '[' expression ']'
		{
			$$ = NULL;	//TODO
			left_tmp = eval_tree( $3 );
			if( left_tmp != INTEGER ) {
				fprintf( stderr, "\nERROR: non-integer type for array index\n" );
				exit(1);
			}
		}
	;

procedure_statement: id
	| id '(' expression_list ')'
	{
		list_t* subroutine = search_scopes(symbol_table, $1->attribute.sval->name);
		list_t* required_args = argument_type_list( $3 );
		list_t* given_args = subroutine->arg_list;
		if( !compare_types(required_args, given_args) )
		{
			fprintf( stderr, "\nERROR: Procedure passed wrong number/type of arguments\n" );
			exit(1);
		}
	}
	;


expression_list: expression
		{ $$ = $1; }
	| expression_list ',' expression
		{ $$ = make_tree( COMMA, $1, $3); }
	;

expression: simple_expression
		{ $$ = $1; }
	| simple_expression RELOP simple_expression
		{ $$ = make_tree( RELOP, $1, $3); $$->attribute.opval = $2; }
	;

simple_expression: term
		{ $$ = $1; }
	| ADDOP term
		{ $$ = make_tree( ADDOP, $2, NULL); $$->attribute.opval = $1; }
	| simple_expression ADDOP term
		{ $$ = make_tree( ADDOP, $1, $3); $$->attribute.opval = $2; }
	;

term: factor
	| term MULOP factor
		{ $$ = make_tree( INUM, NULL, NULL); $$->attribute.ival = $1; }
	;

factor: id
		{ $$ = $1; }
	| id '[' expression ']'
		{
			$$ = NULL;	//TODO
			left_tmp = eval_tree( $3 );
			if( left_tmp != INTEGER ) {
				fprintf( stderr, "\nERROR: non-integer type for array index\n" );
				exit(1);
			}
		}
	| id '(' expression_list ')'
		{
			list_t* subroutine = search_scopes(symbol_table, $1->attribute.sval->name);
			if( subroutine->type_value == PROCEDURE ) {
				fprintf( stderr, "\nERROR: Procedure don't return values\n" );
				exit(1);
			}

			list_t* required_args = argument_type_list( $3 );
			list_t* given_args = subroutine->arg_list;
			if( !compare_types(required_args, given_args) )
			{
				fprintf( stderr, "\nERROR: Function passed wrong number/type of arguments\n" );
				exit(1);
			}
		}
	| INUM
		{ $$ = make_tree( INUM, NULL, NULL); $$->attribute.ival = $1; }
	| RNUM
		{ $$ = make_tree( RNUM, NULL, NULL); $$->attribute.rval = $1; }
	| '(' expression ')'
		{ $$ = $2; }
	| NOT factor
		{ $$ = make_tree( NOT, $2, NULL); }
	;

id: ID
		{
			$$ = make_ident( search_scopes( symbol_table, $1 ) );
		}

	;
%%

int main()
{
	/* symbol_table = NULL; */
	symbol_table = (hash_t *)malloc( sizeof(hash_t) );
	yyparse();
}
