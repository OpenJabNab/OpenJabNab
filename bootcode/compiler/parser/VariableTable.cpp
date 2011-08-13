#include "VariableTable.h"

void VariableTable::addVariable(string name) {
	DEBUG(cout << "Adding variable (" << name << ")");
	variables.insert(name);
}

void VariableTable::removeVariable(string name) {
	DEBUG(cout << "Removing variable (" << name << ")");
	variables.erase(name);
}

bool VariableTable::isVariable(string name) {
	set<string>::iterator it = variables.find(name);
	if(it != variables.end()) {
		return true;
	}
	return false;
}
