#include "DeclarationFunction.h"

string DeclarationFunction::declarationType = "function";

string DeclarationFunction::toString() {
	string s = "fun " + this->name + " ";

	list<string>::iterator it;
	
	for(it = argList.begin(); it != argList.end(); it++) {
		s += *it;
		s += " ";
	}
	
	s += "= ";

	if(this->e != NULL) {
		s += e->toString();
	}
	s += ";;";

	return s;
}

string DeclarationFunction::getDeclarationType() {
	return DeclarationFunction::declarationType;
}

void DeclarationFunction::addArg(string name) {
	argList.push_front(name);
}
