#ifndef __DECLARATION__
#define __DECLARATION__

#include <string>
#include <utility>
#include <iostream>
#include "Debug.h"
#include "Expression.h"
#include "Type.h"

using namespace std;

class Declaration {
public:
	Declaration(string name, Expression *e, Type *type) {
		this->name = name;
		this->e = e;
		this->type = type;
	}

	string getName();

	void setExpression(Expression *e);

	Expression* getExpression();

	void setType(Type *type);

	Type* getType();

	virtual string getDeclarationType() = 0;

	virtual string toString() = 0;



protected:
	string name;
	Expression *e;
	Type *type;
};

#endif
