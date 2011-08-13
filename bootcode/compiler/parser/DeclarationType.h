#ifndef __DECLARATION_TYPE__
#define __DECLARATION_TYPE__

#include <iostream>
#include <string>
#include <utility>
#include "Debug.h"
#include "Declaration.h"

class DeclarationType : public Declaration {
public:
	DeclarationType(string name) : Declaration(name,NULL,NULL) {
	}	

	string toString(void);
	string getDeclarationType();

private:
	static string declarationType;
};
#endif
