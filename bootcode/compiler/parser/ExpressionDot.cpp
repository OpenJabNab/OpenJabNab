#include "ExpressionDot.h"

string ExpressionDot::toString(void) {
	return e1->toString() + "." + e2->toString();
}

void ExpressionDot::addExpression(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}
