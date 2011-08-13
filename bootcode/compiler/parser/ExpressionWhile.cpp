#include "ExpressionWhile.h"

string ExpressionWhile::toString() {
	return "while " + e1->toString() + " do " + e2->toString();
}

void ExpressionWhile::addExpression(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}
