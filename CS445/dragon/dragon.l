%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "tree.h"
#include "y.tab.h"
%}

whitespace	[ \t]+

letter 		[a-zA-Z]
digit 		[0-9]
id			{letter}({letter}|{digit})*

digits		{digit}+
inum		{digits}
rnum		{digits}"."{digits}

comment		"(*"[^*)\n]*"*)"\n?|"{"[^}\n]*"}"\n?

%%

{whitespace}		{;}
{comment}			{;}

"program"			{ fprintf( stderr, "[PROGRAM]" ); return PROGRAM; }
"begin"				{ fprintf( stderr, "[BEGIN]" ); return BBEGIN; }
"end"				{ fprintf( stderr, "[END]" ); return END; }
"if"				{ fprintf( stderr, "[IF]" ); return IF; }
"then"				{ fprintf( stderr, "[THEN]" ); return THEN; }
"else"				{ fprintf( stderr, "[ELSE]" ); return ELSE; }
"while"				{ fprintf( stderr, "[WHILE]" ); return WHILE; }
"do"				{ fprintf( stderr, "[DO]" ); return DO; }
"for"				{ fprintf( stderr, "[FOR]" ); return FOR; }
"to"				{ fprintf( stderr, "[TO]" ); return TO; }

"var"				{ fprintf( stderr, "[VAR]" ); return VAR; }
"array"				{ fprintf( stderr, "[ARRAY]" ); return ARRAY; }
"of"				{ fprintf( stderr, "[OF]" ); return OF; }
".."				{ fprintf( stderr, "[DOTDOT]" ); return DOTDOT; }

"integer"			{ fprintf( stderr, "[INTEGER]" ); return INTEGER; }
"real"				{ fprintf( stderr, "[REAL]" ); return REAL; }

"function"			{ fprintf( stderr, "[FUNCTION]" ); return FUNCTION; }
"procedure"			{ fprintf( stderr, "[PROCEDURE]" ); return PROCEDURE; }

":="				{ fprintf( stderr, "[ASSIGNOP]" ); return ASSIGNOP; }

"<"					{ fprintf( stderr, "[RELOP:%d]", yylval.opval = LT ); return RELOP; }
"<="				{ fprintf( stderr, "[RELOP:%d]", yylval.opval = LE ); return RELOP; }
">"					{ fprintf( stderr, "[RELOP:%d]", yylval.opval = GT ); return RELOP; }
">="				{ fprintf( stderr, "[RELOP:%d]", yylval.opval = GE ); return RELOP; }
"="					{ fprintf( stderr, "[RELOP:%d]", yylval.opval = EQ ); return RELOP; }
"<>"				{ fprintf( stderr, "[RELOP:%d]", yylval.opval = NE ); return RELOP; }

"+"					{ fprintf( stderr, "[ADDOP:%d]", yylval.opval = PLUS ); return ADDOP; }
"-"					{ fprintf( stderr, "[ADDOP:%d]", yylval.opval = MINUS ); return ADDOP; }
"or"				{ fprintf( stderr, "[ADDOP:%d]", yylval.opval = OR ); return ADDOP; }

"*"					{ fprintf( stderr, "[MULOP:%d]", yylval.opval = STAR ); return MULOP; }
"/"					{ fprintf( stderr, "[MULOP:%d]", yylval.opval = SLASH ); return MULOP; }
"mod"				{ fprintf( stderr, "[MULOP:%d]", yylval.opval = MOD ); return MULOP; }
"and"				{ fprintf( stderr, "[MULOP:%d]", yylval.opval = AND ); return MULOP; }

"not"				{ fprintf( stderr, "[NOT]" ); return NOT; }

{inum}				{ fprintf( stderr, "[INUM:%d]", yylval.ival = atoi(yytext) ); return INUM; }
{rnum}				{ fprintf( stderr, "[RNUM:%f]", yylval.rval = atof(yytext) ); return RNUM; }
{id}				{ fprintf( stderr, "[ID:%s]", yylval.sval = strdup(yytext) ); return ID; }
\n					{ fprintf( stderr, "[EOL]\n" ); }

.					{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }

%%

int yyerror( char *msg )
{
	fprintf( stderr, "ERROR: %s\n", msg );
	exit(1);
}
