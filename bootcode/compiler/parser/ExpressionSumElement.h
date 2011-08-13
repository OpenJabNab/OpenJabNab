#ifndef __EXPRESSION_SUM_ELEMENT
#define __EXPRESSION_SUM_ELEMENT

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionSumElement : public Expression {
public:
	ExpressionSumElement() : Expression() {
	}

	string toString();
	void addElements(Expression *e1, Expression *e2);
	bool hasConstructor();

private:
	Expression *e1;
	Expression *e2;
};

#endif
