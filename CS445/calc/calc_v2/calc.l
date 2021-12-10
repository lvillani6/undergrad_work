%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define EOS	256
#define NUM	257
#define ADDOP	258
#define MULOP	259

%}

whitespace	[ \t]+
number 		[0-9]+

%%

{whitespace}			{ ; }
{number}			{ fprintf( stderr, "[NUM:%d]", (int)atoi(yytext) ); return NUM; }
"("				{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
")"				{ fprintf( stderr, "[%c]", yytext[0] ); return yytext[0]; }
"+"				{ fprintf( stderr, "[ADDOP:%c]", yytext[0] ); return ADDOP; }
"*"				{ fprintf( stderr, "[MULOP:%c]", yytext[0] ); return MULOP; }
\n				{ fprintf( stderr, "\n" ); return EOS; }
.				{ fprintf( stderr, "{%c}", yytext[0] ); return yytext[0]; }

%%

int main()
{
	while(1) {
		yylex();
	}

}
