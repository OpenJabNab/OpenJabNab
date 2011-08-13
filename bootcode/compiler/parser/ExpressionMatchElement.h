#ifndef __EXPRESSION_MATCH_ELEMENT__
#define __EXPRESSION_MATCH_ELEMENT__


#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionMatchElement : public Expression {
public:
	ExpressionMatchElement(Expression *e1, Expression *e2, Expression *e3); 

	string toString();

private:
	Expression *e1;
	Expression *e2;
	Expression *e3;
};
#endif
