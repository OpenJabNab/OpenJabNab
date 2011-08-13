#include "ExpressionParenth.h"

string ExpressionParenth::toString() {
	return "(" + e->toString() + ")";
}

void ExpressionParenth::addExpression(Expression *e) {
	this->e = e;
}
