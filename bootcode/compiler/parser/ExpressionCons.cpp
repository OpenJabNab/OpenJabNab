#include "ExpressionCons.h"

ExpressionCons::ExpressionCons(Expression *e1, Expression *e2) : Expression() {
	this->e1 = e1;
	this->e2 = e2;
}

string ExpressionCons::toString() {
	return e1->toString() + " " + e2->toString();
}
