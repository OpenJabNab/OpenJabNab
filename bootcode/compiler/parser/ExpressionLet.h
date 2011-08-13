#ifndef __EXPRESSION_LET__
#define __EXPRESSION_LET__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionLet : public Expression {
public:
	ExpressionLet() : Expression() {

	}

	string toString(void);
	void addExpression(Expression *e1, Expression *e2, Expression *e3);
	
private:

	Expression *e1;
	Expression *e2;
	Expression *e3;

};

#endif
