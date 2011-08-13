#ifndef __TYPE_ARRAY__
#define __TYPE_ARRAY__

#include <string>
#include <utility>
#include <iostream>
#include "Type.h"

class TypeArray : public Type {
public:
	TypeArray() : Type("ARRAY") {
	}

	string toString();
	void setType(Type *t);

private:
	Type *t;

};
#endif
