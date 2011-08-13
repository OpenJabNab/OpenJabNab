#include "ExpressionSumElement.h"

string ExpressionSumElement::toString(void) {
	string s = e1->toString();

	if(e2 != NULL) {
		s += " " + e2->toString();
	}

	return s;
}

void ExpressionSumElement::addElements(Expression *e1, Expression *e2) {
	this->e1 = e1;
	this->e2 = e2;
}

bool ExpressionSumElement::hasConstructor() {
	if(e2 != NULL) 
		return true;

	return false;

}
