#ifndef __EXPRESSION_ARRAY__
#define __EXPRESSION_ARRAY__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionArray : public Expression {
public:
	void addElt(Expression *e);
	string toString();

private:
	list<Expression *> elements;
};
#endif
