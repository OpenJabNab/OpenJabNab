#ifndef __EXPRESSION_TUPLE__
#define __EXPRESSION_TUPLE__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionTuple : public Expression {
public:

	ExpressionTuple() : Expression() {}

	void addElt(Expression *e);
	string toString();
	
private:
	//vector<Expression *> eVector;
	list<Expression *> elements;
};

#endif
