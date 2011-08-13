#ifndef __EXPRESSION_FUN__
#define __EXPRESSION_FUN__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionFun : public Expression {
public:
	ExpressionFun() : Expression() {
	}

	void addExpression(Expression *e);
	string toString(void);

private:
	list<Expression *> eList;

};
#endif
