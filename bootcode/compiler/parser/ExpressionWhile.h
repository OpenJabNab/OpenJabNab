#ifndef __EXPRESSION_WHILE__
#define __EXPRESSION_WHILE__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionWhile : public Expression {
public: 
	ExpressionWhile() : Expression() {
		e1 = NULL;
		e2 = NULL;
	}

	string toString();

	void addExpression(Expression *e1, Expression *e2);

private:
	Expression *e1;
	Expression *e2;
};

#endif
