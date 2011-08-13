#ifndef __EXPRESSION_MATCH__
#define __EXPRESSION_MATCH__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

//match expr with 
class ExpressionMatch : public Expression {
public:
	ExpressionMatch() : Expression() {

	}

	string toString();
	void setExpressionE1(Expression *e);
	void addElement(Expression *e);

private:
	Expression *e1;
	list<Expression *>eList;	
};
#endif
