#ifndef __EXPRESSION_SUM__
#define __EXPRESSION_SUM__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionSum : public Expression {
public:
	ExpressionSum() : Expression() {
	}

	string toString(void);
	void addElement(Expression *e);

private:
	list<Expression *> sumElements;
};

#endif
