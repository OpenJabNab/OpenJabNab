#ifndef __EXPRESSION_INT__
#define __EXPRESSION_INT__

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"
#include "Type.h"

using namespace std;

class ExpressionInt : public Expression {
public:
	ExpressionInt(int value, Type *td) : Expression(td) {
		this->value = value;
	}

	int getValue();

	string toString();

private: 
	int value;

};

#endif
