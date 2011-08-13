#include "TypeTable.h"

TypeTable* TypeTable::instance = NULL;

TypeTable* TypeTable::getInstance() {
	if(instance == NULL) {
		instance = new TypeTable();
	}
	return instance;
}

TypeTable::TypeTable() {
	TypeInt *ti = new TypeInt();
	TypeString *ts = new TypeString();

	scalarType["INT"] = ti;
	scalarType["STRING"] = ts;
}

Type* TypeTable::getScalar(string name) {
	return scalarType[name];
}

