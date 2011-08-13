#include "ExpressionStruct.h"

string ExpressionStruct::toString(void) {
	string s = "[ ";

	list<Expression *>::iterator it;

	for(it = structElements.begin(); it != structElements.end(); it++) {
		s += (*it)->toString();
		s += " ";
	}

	s += "]";

	return s;
}

void ExpressionStruct::addElement(Expression *e) {
	structElements.push_front(e);
}
