#include "ExpressionInt.h"

int ExpressionInt::getValue() {
	return this->value;
}

string ExpressionInt::toString() {
	stringstream out;

	out << this->value;
	return out.str();
}
