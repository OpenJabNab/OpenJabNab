#ifndef __DECLARATION_PROTO__
#define __DECLARATION_PROTO__

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Declaration.h"

class DeclarationProto : public Declaration {
public: 
	DeclarationProto(string name, int nbArgs) : Declaration(name, NULL, NULL) {
		this->nbArgs = nbArgs;
	}

	string toString();

	string getDeclarationType();

	int getNbArgs(void);


private:
	int nbArgs;
	static string declarationType;
};

#endif
