#ifndef __EXPRESSION_DOT__
#define __EXPRESSION_DOT__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionDot : public Expression {
public:
	ExpressionDot() : Expression() {
	}

	string toString(void);
	void addExpression(Expression *e1, Expression *e2);

private:
	Expression *e1;
	Expression *e2;

};
#endif
