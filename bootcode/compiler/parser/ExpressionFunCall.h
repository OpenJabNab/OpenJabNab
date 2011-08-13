#ifndef __EXPRESSION_FUN_CALL
#define __EXPRESSION_FUN_CALL

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionFunCall : public Expression {
public:
	ExpressionFunCall() : Expression() {}

	string toString();
	void addExpression(Expression *e);
	void setFunction(Expression *e);

private:
	Expression *function;
	list<Expression *> argList;


};
#endif
