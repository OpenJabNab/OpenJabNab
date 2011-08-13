#include "DeclarationType.h"

string DeclarationType::declarationType = "type";

string DeclarationType::getDeclarationType() {
	return declarationType;
}

string DeclarationType::toString(void) {
	return "type " + this->name + " = " + e->toString() + ";;";

}
