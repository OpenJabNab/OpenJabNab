#ifndef __DECLARATION_CONSTANT__
#define __DECLARATION_CONSTANT__

#include <iostream>
#include <string>
#include <utility>
#include "Declaration.h"

class DeclarationConstant : public Declaration {
public:
	DeclarationConstant(string name) : Declaration(name, NULL, NULL) {
	}

	string toString();
	string getDeclarationType();

private:
	static string declarationType;

};

#endif
