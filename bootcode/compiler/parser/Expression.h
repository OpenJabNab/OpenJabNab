#ifndef __EXPRESSION__
#define __EXPRESSION__

#include <string>
#include <utility>
#include <iostream>
#include "Debug.h"
#include "Type.h"

using namespace std;

class Expression {
public:
	Expression() {
		type = NULL;
	}

	Expression(Type *td) {
		type = td;
	}

	Type* getType();
	void setType(Type *td);

	virtual string toString() = 0;

private:
	Type *type;
};

#endif
