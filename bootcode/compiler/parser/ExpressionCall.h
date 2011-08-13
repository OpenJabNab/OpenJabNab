#ifndef __EXPRESSION_CALL__
#define __EXPRESSION_CALL_

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionCall : public Expression {
public:
	ExpressionCall() : Expression() {
		e1 = NULL;
		e2 = NULL;
	}

	string toString(void);

	void addExpression(Expression *e1, Expression *e2);

private:
	Expression *e1;
	Expression *e2;

};

#endif
