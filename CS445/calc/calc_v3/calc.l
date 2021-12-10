%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree.h"
#include "y.tab.h"
%}

whitespace	[ \t]+
number 		[0-9]+

%%

{whitespace}	{ ; }
{number}			{ fprintf( stderr, "[NUM:%d]", yylval.ival = (int)atoi(yytext) ); return NUM; }
"("						{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
")"						{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
"+"						{ fprintf( stderr, "[ADDOP:%c]", yytext[0] ); return ADDOP; }
"*"						{ fprintf( stderr, "[MULOP:%c]", yytext[0] ); return MULOP; }
\n						{ fprintf( stderr, "\n" ); return yytext[0]; }
.							{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }

%%

int yyerror( char *s )
{
	fprintf( stderr, "ERROR: %s\n", s );
}
