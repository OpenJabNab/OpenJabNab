#include "ExpressionIf.h"

string ExpressionIf::toString() {
	string s = "if " + e1->toString() + " then " + e2->toString();

	if(e3 != NULL) {
		s += " else " + e3->toString();
	}

	return s;
}

void ExpressionIf::addExpression(Expression *e1, Expression *e2, Expression *e3) {
	this->e1 = e1;
	this->e2 = e2;
	this->e3 = e3;
}
