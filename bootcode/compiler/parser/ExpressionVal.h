#ifndef __EXPRESSION_VAL__
#define __EXPRESSION_VAL__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionVal : public Expression {
public:
	ExpressionVal() : Expression() {
		e1 = NULL;
		e2 = NULL;
	}

	string toString();
	void addExpression(Expression *e1);
	void addExpression(Expression *e1, Expression *e2);

private:
	Expression *e1;
	Expression *e2;
	string op;

};

#endif
