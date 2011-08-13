#include "ExpressionString.h"

void ExpressionString::setValue(string value) {
	this->value = value;
}

string ExpressionString::getValue() {
	return this->value;
}

string ExpressionString::toString() {
	return this->value;
}
