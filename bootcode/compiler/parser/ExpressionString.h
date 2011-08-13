#ifndef __EXPRESSION_STRING__
#define __EXPRESSION_STRING__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class ExpressionString : public Expression {
public:
	ExpressionString(string value, Type *td) : Expression(td) {
		this->value = value;
	}

	void setValue(string value);
	string getValue();
	string toString();

private:
	string value;
};

#endif
