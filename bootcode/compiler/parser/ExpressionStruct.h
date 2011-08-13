#ifndef __EXPRESSION_TYPE__
#define __EXPRESSION_TYPE__

#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionStruct : public Expression {
public:
	ExpressionStruct() : Expression() {
	}

	string toString(void);
	void addElement(Expression *e);

private:
	list<Expression *> structElements;	

};

#endif
