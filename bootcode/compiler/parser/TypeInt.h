#ifndef __TYPE_INT__
#define __TYPE_INT__

#include <iostream>
#include <string>
#include <utility>
#include "Type.h"

using namespace std;

class TypeInt : public Type {
public:
	TypeInt() : Type("INT") {
	}

	string toString();

};

#endif
