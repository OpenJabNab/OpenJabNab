#include "ExpressionFunCall.h"

void ExpressionFunCall::setFunction(Expression *e) {
	this->function = e;
}

void ExpressionFunCall::addExpression(Expression *e) {
	argList.push_front(e);
}

string ExpressionFunCall::toString(void) {
	string s = function->toString() + " ";

	list<Expression *>::iterator it;
	for(it = argList.begin(); it != argList.end(); it++) {
		s += (*it)->toString();
		s += " ";
	}
	return s;
}
