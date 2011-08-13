#include "TypeArray.h"

string TypeArray::toString() {
	return "{" + t->toString() + "}";
}

void TypeArray::setType(Type *t) { 
	this->t = t;
}
