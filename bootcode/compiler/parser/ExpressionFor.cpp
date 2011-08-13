#include "ExpressionFor.h"

string ExpressionFor::toString(void) {
	string s = "for " + id->toString() + "=" + e1->toString() + "; " + e2->toString();

	if(e3 != NULL) {
		s += "; " + e3->toString();
	}

	s += " do " + e4->toString();
	return s;
}

void ExpressionFor::addExpression(Expression *id, Expression *e1, Expression *e2, Expression *e3, Expression *e4) {
	this->id = id;
	this->e1 = e1;
	this->e2 = e2;
	this->e3 = e3;
	this->e4 = e4;
}
