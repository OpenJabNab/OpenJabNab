#include "Declaration.h"

string Declaration::getName() {
	return this->name;
}

void Declaration::setExpression(Expression *e) {
	this->e = e;
}

Expression * Declaration::getExpression() {
	return e;
}

void Declaration::setType(Type *type) {
	this->type = type;
}

Type* Declaration::getType() {
	return this->type;
}

