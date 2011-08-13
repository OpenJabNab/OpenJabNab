#ifndef __EXPRESSION_FOR__
#define __EXPRESSION_FOR__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionFor : public Expression {
public:
	ExpressionFor() : Expression() {
		e1 = NULL;
		e2 = NULL;
		e3 = NULL;
		e4 = NULL;
	}

	string toString(void);
	void addExpression(Expression *id, Expression *e1, Expression *e2, Expression *e3, Expression *e4);

private:
	Expression *id;
	Expression *e1;
	Expression *e2;
	Expression *e3;
	Expression *e4;
};

#endif
