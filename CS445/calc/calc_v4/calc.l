%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tree.h"
#include "y.tab.h"
%}

whitespace	[ \t]+
number 		[0-9]+
ident 		[a-zA-Z]+

%%

{whitespace}	{ ; }
{number}		{ fprintf( stderr, "[NUM:%d]", yylval.ival = (int)atoi(yytext) ); return NUM; }
{ident}			{ fprintf( stderr, "[ID:%s]", yylval.sval = strdup(yytext) ); return ID; }
"("				{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
")"				{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
"="				{ fprintf( stderr, "[ASSIGNOP:%c]", yylval.opval = yytext[0] ); return ASSIGNOP; }
"+"				{ fprintf( stderr, "[ADDOP:%c]", yylval.opval = yytext[0] ); return ADDOP; }
"*"				{ fprintf( stderr, "[MULOP:%c]", yylval.opval = yytext[0] ); return MULOP; }
\n				{ fprintf( stderr, "\n" ); return yytext[0]; }
.				{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }

%%

int yyerror( char *s )
{
	fprintf( stderr, "ERROR: %s\n", s );
}
