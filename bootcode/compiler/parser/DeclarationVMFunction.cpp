#include "DeclarationVMFunction.h"

string DeclarationVMFunction::declarationType = "vmfunction";

int DeclarationVMFunction::getNbArgs() {
	return this->nbArgs;
}

string DeclarationVMFunction::getDeclarationType(){
	return DeclarationVMFunction::declarationType;
}

string DeclarationVMFunction::toString() {
	return "";
}
