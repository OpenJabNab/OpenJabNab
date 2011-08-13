#include "ExpressionMatch.h"

string ExpressionMatch::toString() {
	string s = "match " + e1->toString() + " " + "with ";

	list<Expression *>::iterator it = eList.begin();

	while(1) {
		s += (*it)->toString();
		it++;
		if(it != eList.end()) {
			s += "|";
		} else {
			break;
		}
	}
	return s;
}

void ExpressionMatch::addElement(Expression *e) {
	eList.push_front(e);
}

void ExpressionMatch::setExpressionE1(Expression *e) {
	this->e1 = e;
}
