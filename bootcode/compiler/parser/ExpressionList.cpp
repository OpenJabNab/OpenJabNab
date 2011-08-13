#include "ExpressionList.h"

void ExpressionList::addElt(Expression *e) {
	elements.push_front(e);
}

string ExpressionList::toString(void) {
	string s; 

	list<Expression *>::iterator it = elements.begin();

	while(1) {
		s += (*it)->toString();
		it++;
		if(it == elements.end()) {
			break;
		}

		s += "::";
	}

	return s;
}
