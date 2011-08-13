#ifndef __PARSER__
#define __PARSER__

#include <string>
#include <utility>
#include <iostream>
#include "metalp.tab.h"
#include "Debug.h"
#include "Program.h"

extern "C" {
	int yylex(void);
}

using namespace std;

class Parser {

public:

	static Parser* getInstance(void);

	/**
	 * Read a token from the lexer 
	 */
	int getToken();

	/**
	 * Give back look ahead
	 */
	void giveBack();

	/**
	 * Parser entry point
	 */
	int parse(void);

	int parseFun(void);
	int parseFunExpression(void); //parseprogram()
	int parseExpression(void);
	int parseArithm(void);
	int parseA1(void);
	int parseA2(void);
	int parseA3(void);
	int parseA4(void);
	int parseA5(void);
	int parseA6(void);
	
	int parseTerm(void);

	int parseIf(void);
	int parseLet(void);
	int parseLocals(void);
	int parseSet(void);
	int parseSetPoint(int *nbExprs);
	int parseWhile(void);
	int parseFor(void);
	int parseMatch(void);
	int parseCall(void);
	int parseUpdate(void);
	int parseUpdateVals(void);
	int parsePntFun(void);

	int parseMatchCons(int *);
	
	int parseFields();
	int parseRef(void);
	int parseGetPoint(void);


	int parseType(void);
	int parseStruct(string name);
	int parseSum(string name);

	int parseConst(void);

	int parseProto(void);

	//Functions to parse variables
	/**
	 * Entry point to variable parsing
	 */
	int parseVar(void);
	int parseVal(void);
	int parseVal3(void);
	int parseVal4(void);
	int parseVal5(void);
	int parseVal6(void);
	int parseVal7(void);
	
	/**
	 * char* value from lexer
	 */
	string sVal;

	/**
	 * int value from lexer
	 */
	int iVal;
private:
	static Parser *_instance;

	Program *program;

	/**
	 * Actual token
	 */
	int token;

	bool giveBack_;

	Parser() {
		giveBack_ = false;
		program = Program::getInstance();
		VMFunctions::addVMFunctionsToProgram();

	}
};

#endif
