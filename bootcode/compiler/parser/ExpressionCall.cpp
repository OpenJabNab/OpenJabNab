#include "ExpressionCall.h"

void ExpressionCall::addExpression(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}

string ExpressionCall::toString(void) {
	string s = "call " + e1->toString();

	if(e2 != NULL) {
		s += " " + e2->toString();
	}
	return s;
}
