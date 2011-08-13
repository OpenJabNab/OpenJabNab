#include "ExpressionUnaryOp.h"

string ExpressionUnaryOp::toString() {
	return this->op + e->toString();
}

void ExpressionUnaryOp::addExpression(Expression *e, string op) {
	this->e = e;
	this->op = op;
}
