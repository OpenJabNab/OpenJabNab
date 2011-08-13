#include "ExpressionTuple.h"

void ExpressionTuple::addElt(Expression *e) {
	elements.push_front(e);
}

string ExpressionTuple::toString() {
	string s = "[ ";

	list<Expression *>::iterator it;

	for(it = elements.begin(); it != elements.end(); it++) {
		s += (*it)->toString();
		s += " ";
	}

	s += "]";
	return s;
}
