#include "DeclarationConstant.h"

string DeclarationConstant::declarationType = "constant";

string DeclarationConstant::getDeclarationType() {
	return declarationType;
}

string DeclarationConstant::toString() {
	return "const " + name + " = " + e->toString() + ";;";
}
