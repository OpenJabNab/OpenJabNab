#ifndef __TYPE__
#define __TYPE__

#include <string>
#include <utility>
#include <iostream>

using namespace std;


class Type {
public:
	Type(string name) {
		this->name = name;
	}

	virtual string toString() = 0;
	
private:
	string name;
};

#endif
