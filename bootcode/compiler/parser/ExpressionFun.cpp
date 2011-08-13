#include "ExpressionFun.h"

string ExpressionFun::toString(void) {
	string s;

	list<Expression *>::iterator it;

	it = eList.begin();

	while (1) {
		s += (*it)->toString();
		it++;
		if(it == eList.end()) {
			break;
		}

		s += ";";
	}
	return s;
}

void ExpressionFun::addExpression(Expression *e) {
	eList.push_front(e);
}


