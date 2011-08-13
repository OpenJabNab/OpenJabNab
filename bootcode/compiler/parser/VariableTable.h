#ifndef __VARIABLE_TABLE__
#define __VARIABLE_TABLE__

#include <iostream>
#include <set>
#include <string>
#include <utility>
#include "Debug.h"
#include "Expression.h"

using namespace std;

class VariableTable {
public:
	//VariableTable();

	void addVariable(string name);
	
	void removeVariable(string name);

	bool isVariable(string name);

private:
	set<string> variables;
};

#endif
