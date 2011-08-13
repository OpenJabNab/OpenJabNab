%{
/* This parser strips all comments.*/
#include <stdio.h>
%}

%%
\".*\"				{printf("%s", yytext); /* Make sure we dont grab strings*/} 
"/*"				{comment();}
"//".*\n			{printf("\n");}

%%

main(int argc, char **argv) {
	yylex();
}

comment()
{
	char c, c1;

loop:
	while((c = input()) != '*' && c != 0);		
	
	c1 = input();
	if(c1 != '/' && c1 != 0) {
		unput(c1);
		goto loop;
	}
}

