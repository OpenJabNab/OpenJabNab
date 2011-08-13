#ifndef __EXPRESSION_UNARY_OP__
#define __EXPRESSION_UNARY_OP__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionUnaryOp : public Expression {
public: 
	ExpressionUnaryOp() : Expression() {
		e = NULL;
	}

	string toString();
	void addExpression(Expression *e, string op);

private:
	Expression *e;
	string op;
};
#endif
