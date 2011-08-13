#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include "Parser.h"
#include <stdio.h>

extern "C" {
	int yylex(void);
}


extern int yylineno;
extern FILE *yyin;

using namespace std;

int main(int ac, char **av) {
	if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
		perror(av[1]);
		exit(1);
	}

	Parser *p = Parser::getInstance();
	while(1) {
		if(p->parse()) 
			exit(1);
	}

}
