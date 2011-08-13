#include "ExpressionPointer.h"

string ExpressionPointer::toString(void) {
	return "#" + e->toString();
}

void ExpressionPointer::addExpression(Expression *e) {
	this->e = e;
}
