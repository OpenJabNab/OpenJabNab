#ifndef __EXPRESSION_CONS__
#define __EXPRESSION_CONS__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionCons : public Expression {
public:	
	ExpressionCons(Expression *e1, Expression *e2);

	string toString(void);

private:
	Expression *e1;
	Expression *e2;
};
#endif
