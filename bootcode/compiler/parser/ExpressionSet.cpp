#include "ExpressionSet.h"

void ExpressionSet::addExpression(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}

string ExpressionSet::toString(void) {
	return "set " + e1->toString() + " = " + e2->toString();
}
