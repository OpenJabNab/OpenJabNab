%option noyywrap
%option yylineno
%{
#include "metalp.tab.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include "Parser.h"

int a = 1;

extern "C" {
	int yylex(void);
	string singleQuoteString(void);
	string doubleQuoteString(void);
	int htoi(char *);
}

using namespace std;

Parser *parser = Parser::getInstance();

%}

%x FUN_START
%x FUN_ARG_START
%x VAR_START
%x TYP_START
%x PRO_START
%x CON_START
%x SET_START

%%
\"							{parser->sVal=doubleQuoteString(); return STRING;}
"'"							{parser->sVal=singleQuoteString(); return STRING;}
[ \t\n\r]						{;}
"("							{return yytext[0];}
")"							{return yytext[0];}
"{"							{return yytext[0];}
"}"							{return yytext[0];}
"["							{return yytext[0];}
"]"							{return yytext[0];}
";;"							{a = 1; return DOUBLE_POINT_VIRGULE;}
";"							{return yytext[0];}
","							{return yytext[0];}
"::"							{return DOUBLE_DEUX_POINTS;}
":"							{return yytext[0];}
<FUN_ARG_START>"="					{BEGIN 0; return yytext[0];}
"=="							{return EQ;}
"!="							{return NE;}
"<="							{return LE;}
">="							{return GE;}
"<<"							{return LS;}
">>"							{return RS;}
"&&"							{return ANDAND;}
"||"							{return OROR;}
"<"							{return yytext[0];}
">"							{return yytext[0];}
"&"							{return yytext[0];}
"|"							{return yytext[0];}
"!"							{return yytext[0];}
"="							{return yytext[0];}
"+"							{return yytext[0];}
"-"							{return yytext[0];}
"*"							{return yytext[0];}
"/"							{return yytext[0];}
"^"							{return yytext[0];}
"%"							{return yytext[0];}
"~"							{return yytext[0];}
"."							{return yytext[0];}
"_"							{return yytext[0];}
"->"							{return FLECHE;}
"#"							{return yytext[0];}
"fun"							{BEGIN FUN_START; a=0; return FUN;}
"let"							{return LET;}
"in"							{return IN;}
"proto"							{BEGIN PRO_START; a=0; return PROTO;}
"var"							{BEGIN VAR_START; a=0; return VAR;}
"const"							{BEGIN CON_START; a=0; return CONST;}
"if"							{return IF;}
"then"							{return THEN;}
"else"							{return ELSE;}
"for"							{return FOR;}
"while"							{return WHILE;}
"do"							{return DO;}
"set"							{BEGIN SET_START; return SET;}
"type"							{if( a == 1 ) {a = 0; BEGIN TYP_START; return TYPE; } else { parser->sVal=yytext; return ID;}}
"match"							{return MATCH;}
"with"							{return WITH;}
"nil"							{return NIL;}
"call"							{return CALL;}
"tabnew"						{return TABNEW;}
"update"						{return UPDATE;}
<FUN_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; BEGIN FUN_ARG_START; parser->sVal=yytext; return FUN_ID;}
<VAR_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; parser->sVal=yytext; return VAR_ID;}
<TYP_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; parser->sVal=yytext; return TYPE_ID;}
<PRO_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; parser->sVal=yytext; return PROTO_ID;}
<CON_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; parser->sVal=yytext; return CONST_ID;}
<SET_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*			{BEGIN 0; parser->sVal=yytext; return VAR_ID;}
<FUN_ARG_START>[a-zA-Z_]{1}[a-zA-Z0-9_]*		{parser->sVal=yytext; return FUN_ARG_ID;}
[a-zA-Z_]{1}[a-zA-Z0-9_]*				{parser->sVal=yytext; return ID;}
"0x"[a-zA-Z0-9]+					{parser->iVal=htoi(yytext+2); return NOMBRE;}
[0-9]+							{parser->iVal=atoi(yytext); return NOMBRE;}
0b[0-1]+						{cout << "NOMBRE BINAIRE" << endl; return NOMBRE;}
0o[0-7]+						{cout << "NOMBRE OCTA" << endl; return NOMBRE;}


%%

int htoi(char* src)
{
	int x,c;
	x=0;
	while(c=*src++)
	{
		if ((c>='0')&&(c<='9')) x=(x<<4)+c-'0';
		else if ((c>='A')&&(c<='F')) x=(x<<4)+c-'A'+10;
		else if ((c>='a')&&(c<='f')) x=(x<<4)+c-'a'+10;
		else return x;
	}
	return x;
}

string singleQuoteString()
{
	char c, c0;
	c0 = -1;
	string s = "'";

loop:
	while((c = yyinput()) != 0x27 && c != 0) {
		c0 = c;
		s += c;
	}
	
	if(c0 == 0x5c && c0 != 0) {
		c0 = c;
		s += c;
		goto loop;
	}
	s += "'";
	return s;
}

string doubleQuoteString() 
{
	char c, c0;
	c0 = -1;
	string s = "\"";

loop:
	while((c = yyinput()) != '"' && c != 0) {
		c0 = c;
		s += c;
	}
	
	if(c0 == 0x5c && c0 != 0) {
		c0 = c;
		s += c;
		goto loop;
	}
	s += "\"";
	return s;
}


