#include "ExpressionArray.h"

void ExpressionArray::addElt(Expression *e) {
	elements.push_front(e);
}

string ExpressionArray::toString() {
	string s = "{ ";

	list<Expression *>::iterator it;

	for(it = elements.begin(); it != elements.end(); it++) {
		s += (*it)->toString();
		s += " ";
	}

	s += "}";
	DEBUG(cout << s);
	return s;
}
