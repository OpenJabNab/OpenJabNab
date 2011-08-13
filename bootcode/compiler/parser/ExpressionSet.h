#ifndef __EXPRESSION_SET__
#define __EXPRESSION_SET__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionSet : public Expression {
public:
	ExpressionSet() : Expression() {
		e1=NULL;
		e2=NULL;
	}
	
	string toString(void);

	void addExpression(Expression *e1, Expression *e2);

private:
	Expression *e1;
	Expression *e2;
};

#endif
