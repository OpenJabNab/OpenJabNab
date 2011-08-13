#include "ExpressionVal.h"

string ExpressionVal::toString() {
	if(e2 == NULL)
		return e1->toString();
	else
		return e1->toString() + "::" + e2->toString();
}

void ExpressionVal::addExpression(Expression *e1) {
	this->e1 = e1;
}

void ExpressionVal::addExpression(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}
