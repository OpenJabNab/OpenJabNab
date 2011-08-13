#include "ExpressionLet.h"

string ExpressionLet::toString(void) {
	string s;

	s = "let " + e1->toString() + " -> " + e2->toString() + " in " + e3->toString();

	return s;
}

void ExpressionLet::addExpression(Expression *e1, Expression *e2, Expression *e3) {
	this->e1 = e1;
	this->e2 = e2;
	this->e3 = e3;
}
