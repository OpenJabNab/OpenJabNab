#include "DeclarationVariable.h"

string DeclarationVariable::declarationType = "variable";

string DeclarationVariable::getDeclarationType() {
	return DeclarationVariable::declarationType;
}

string DeclarationVariable::toString() {
	string s = "var " + this->name;

	if(this->e != NULL) {
		s += "=" + e->toString();
	}

	s += ";;";
	return s;
}
