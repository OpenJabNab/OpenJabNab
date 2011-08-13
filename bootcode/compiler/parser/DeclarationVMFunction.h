#ifndef __DECLARATION_VM_FUNCTION__
#define __DECLARATION_VM_FUNCTION__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Declaration.h"
#include "Type.h"

using namespace std;

class DeclarationVMFunction : public Declaration {
public:
	DeclarationVMFunction(string name, int nbArgs, Type *type) : Declaration(name, NULL, type) {
		this->nbArgs = nbArgs;
	}

	int getNbArgs(void);
	string toString();
	string getDeclarationType();

private:
	int nbArgs;
	static string declarationType;
};

#endif
