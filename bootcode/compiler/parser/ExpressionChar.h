#ifndef __EXPRESSION_CHAR__
#define __EXPRESSION_CHAR__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"
#include "Type.h"

class ExpressionChar : public Expression {
public:
	ExpressionChar(string value) : Expression() {
		this->value = value;
	}

	string toString();

	string getValue();



private:
	string value;
};

#endif
