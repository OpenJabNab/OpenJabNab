#include "ExpressionStructCons.h"

string ExpressionStructCons::toString(void) {
	string s = "[ ";

	list< vector<Expression *> *>::iterator it;

	for(it=eList.begin(); it != eList.end(); it++) {
		vector<Expression *> *e = (*it);
		Expression *e1 = (*e)[0];
		Expression *e2 = (*e)[1];
		s += e1->toString() + ":" + e2->toString();
		s += " ";
	}

	s += "]";

	return s;
}

void ExpressionStructCons::addExpression(Expression *e1, Expression *e2) {
	vector<Expression *> *e;
	e = new vector<Expression*>();
	e->push_back(e1);
	e->push_back(e2);
	
	eList.push_front(e);
}
