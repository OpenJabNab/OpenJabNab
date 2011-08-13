#ifndef __EXPRESSION_STRUCT_CONS__
#define __EXPRESSION_STRUCT_CONS__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionStructCons : public Expression {
public:
	ExpressionStructCons() : Expression() {}

	string toString();
	void addExpression(Expression *e1, Expression *e2);

private:
	list< vector<Expression *> *> eList;
};
#endif
