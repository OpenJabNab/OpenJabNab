#include "ExpressionMatchElement.h"

ExpressionMatchElement::ExpressionMatchElement(Expression *e1, Expression *e2, Expression *e3) : Expression() {
	this->e1 = e1;
	this->e2 = e2;
	this->e3 = e3;
}

string ExpressionMatchElement::toString() {
	string s = "(" + e1->toString() + " ";

	if(e2 != NULL)
		s += e2->toString() + " "; 

	s += "->";
	
	s += e3->toString();

	s += ")";

	return s;
}
