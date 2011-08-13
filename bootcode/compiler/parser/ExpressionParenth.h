#ifndef __EXPRESSION_PARENTH__
#define __EXPRESSION_PARENTH__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionParenth : public Expression {
public:
	ExpressionParenth() : Expression() { 
		e = NULL;
	}

	string toString();
	void addExpression(Expression *e);
private:
	Expression *e;
};

#endif
