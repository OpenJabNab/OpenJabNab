#include "ExpressionBinaryOp.h"

string ExpressionBinaryOp::toString() {
	return e1->toString() + " " + op + " " + e2->toString();
}

void ExpressionBinaryOp::addExpression(Expression *e1, Expression *e2, string op) {
	this->e1 = e1;
	this->e2 = e2;
	this->op = op;
}
