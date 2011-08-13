#ifndef __DECLARATION_VARIABLE__
#define __DECLARATION_VARIABLE__

#include <string>
#include <utility>
#include <iostream>
#include "Declaration.h"

using namespace std;

class DeclarationVariable : public Declaration {
public:
	DeclarationVariable(string name) : Declaration(name, NULL, NULL) {
	}

	string getDeclarationType();
	string toString();

private:
	static string declarationType;
};

#endif
