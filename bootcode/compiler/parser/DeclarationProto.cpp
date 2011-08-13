#include "DeclarationProto.h"

string DeclarationProto::declarationType = "proto";

string DeclarationProto::toString() {
	stringstream out;
	out << this->nbArgs;

	return "proto " + name + " " + out.str() + ";;" ;
}

string DeclarationProto::getDeclarationType(void) {
	return declarationType;
}

int DeclarationProto::getNbArgs(void) {
	return this->nbArgs;
}

