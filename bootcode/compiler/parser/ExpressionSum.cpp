#include "ExpressionSum.h"

string ExpressionSum::toString(void) {
	string s;

	list<Expression *>::iterator it = sumElements.begin();

	while(1) {
		s += (*it)->toString();
		it++;
		if(it != sumElements.end()) {
			s += " | ";
		} else {
			break;
		}
	}

	return s;
}

void ExpressionSum::addElement(Expression *e) {
	sumElements.push_front(e);
}
