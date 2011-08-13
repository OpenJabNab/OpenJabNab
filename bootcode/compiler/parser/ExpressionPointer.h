#ifndef __EXPRESSION_POINTER__
#define __EXPRESSION_POINTER__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionPointer : public Expression {
public:
	ExpressionPointer() : Expression() {
	}

	string toString();
	void addExpression(Expression *e);

private:
	Expression *e;

};
#endif
