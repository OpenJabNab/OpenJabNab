#ifndef __DECLARATION_FUNCTION__
#define __DECLARATION_FUNCTION__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Declaration.h"
#include "VariableTable.h"

class DeclarationFunction : public Declaration {
public:
	DeclarationFunction(string name) : Declaration(name, NULL, NULL) {
	}

	string toString();
	
	string getDeclarationType();

	void addArg(string name);

private: 
	static string declarationType;

	list<string> argList;
};

#endif
