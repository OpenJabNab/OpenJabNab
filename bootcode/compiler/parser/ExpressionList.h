#ifndef __EXPRESSION_LIST__
#define __EXPRESSION_LIST__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionList : public Expression {
public:
	ExpressionList() : Expression() {
	}

	string toString(void);
	void addElt(Expression *e);

private:
	list<Expression *> elements;

};

#endif
