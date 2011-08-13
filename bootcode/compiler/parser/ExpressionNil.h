#ifndef __EXPRESSION_NIL__
#define __EXPRESSION_NIL

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

class ExpressionNil : public Expression {
public:
	ExpressionNil() : Expression() {
	}
	string toString();
};
#endif
