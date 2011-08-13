#ifndef __EXPRESSION_IF__
#define __EXPRESSION_IF__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionIf : public Expression {
public:
	ExpressionIf() : Expression() {
		e1 = NULL;
		e2 = NULL;
		e3 = NULL;
	}

	string toString(void);	
	void addExpression(Expression *e1, Expression *e2, Expression *e3);

private:
	// if (e1) then (e2) else (e3)
	Expression *e1; 
	Expression *e2;
	Expression *e3;

};

#endif
