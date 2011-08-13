#ifndef __EXPRESSION_BINARY_OP__
#define __EXPRESSION_BINARY_OP__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionBinaryOp : public Expression {
public:
	ExpressionBinaryOp() : Expression() {
		e1 = NULL;
		e2 = NULL;
	}

	string toString();
	void addExpression(Expression *e1, Expression *e2, string op);

private:
	Expression *e1;
	Expression *e2;
	string op;
};
#endif
