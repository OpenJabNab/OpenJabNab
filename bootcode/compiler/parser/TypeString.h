#ifndef __TYPE_STRING__
#define __TYPE_STRING__

#include <iostream>
#include <string>
#include <utility>
#include "Type.h"

using namespace std;

class TypeString : public Type {
public:
	TypeString() : Type("STRING") {
	}

	string toString();


};

#endif
