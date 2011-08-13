#ifndef __TYPE_TABLE__
#define __TYPE_TABLE__

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "Type.h"
#include "TypeInt.h"
#include "TypeString.h"

using namespace std;

class TypeTable {
public:
	static TypeTable* getInstance();

	Type* getScalar(string name);
	
private:
	TypeTable();

	map<string, Type *> scalarType;
	vector<Type *> arrayType;

	static TypeTable* instance;
};

#endif
