#ifndef __EXPRESSION_UNDEF__
#define __EXPRESSION_UNDEF__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

//TODO set undef type at create time.

class ExpressionUndef : public Expression {
public:
	ExpressionUndef() : Expression() {
	}

	string toString(void);
};

#endif
