#include "Program.h"

Program* Program::instance = NULL;

Program* Program::getInstance() {
	if(instance == NULL) {
		instance = new Program();
	}
	return instance;
}

Program::Program() {
	typeTable = TypeTable::getInstance();
	initScope();
}

// *******************************************************************************************
// *											     *
// *					Utils						     *
// *											     *
// *******************************************************************************************
void Program::addOperator(string op) {
	DEBUG(cout << "Adding operator " << op);
	operatorStack.push(op);
}

void Program::existsDeclarationName(string name, bool all=true) {
	map<string, DeclarationVMFunction *>::iterator itvm  = vmFunctionMap.find(name);
	if(itvm != vmFunctionMap.end()) {
		throw 0;
	}

	map<string, DeclarationFunction *>::iterator itf = functionMap.find(name);
	if(itf != functionMap.end()) {
		throw 0;
	}

	map<string, DeclarationVariable *>::iterator itv = variableMap.find(name);
	if(itv != variableMap.end()) {
		throw 0;
	}

	map<string, DeclarationConstant *>::iterator itc = constantMap.find(name);
	if(itc != constantMap.end()) {
		throw 0;
	}

	map<string, DeclarationType *>::iterator itt = typeMap.find(name);
	if(itt != typeMap.end()) {
		throw 0;
	}

	if(all) {
		map<string, DeclarationProto *>::iterator it = protoMap.find(name);
		if(it != protoMap.end()) {
			throw 0;
		}
	}
}

void Program::initScope(void) {
	//create the global variable scope 
	//and add it to the table
	globalVariableTable = new VariableTable();
	scopeTable.push_back(globalVariableTable);
}

void Program::addScope(void) {
	DEBUG(cout << "Adding a new scope");
	VariableTable *v = new VariableTable();
	scopeTable.push_back(v);
}

void Program::removeScope(void) {
	DEBUG(cout << "Removing a scope");
	delete scopeTable.back();
	scopeTable.pop_back();
}

void Program::addLocalVariable(string name) {
	DEBUG(cout << "Adding local variable " << name << endl);
	scopeTable.back()->addVariable(name);
}

void Program::removeLocalVariable(string name) {
	DEBUG(cout << "Removing local variable " << name << endl);
	scopeTable.back()->removeVariable(name);
}

void Program::addGlobalVariable(string name) {
	DEBUG(cout << "Adding global variable " << name <<endl);
	globalVariableTable->addVariable(name);
}

bool Program::isVariable(string name, bool local) {
	int vectorStart = scopeTable.size() - 1;
	int vectorEnd = 0;

	if(local) vectorEnd = 1;
	
	int i=0;
	for(i=vectorStart; i >= vectorEnd; i--) {
		if(scopeTable[i]->isVariable(name)) {
			return true;
		}
	}
	return false;
}

bool Program::isGlobalVariable(string name) {
	return globalVariableTable->isVariable(name);
}

bool Program::isLocalVariable(string name) {
	return isVariable(name, true);
}

bool Program::isStructureField(string name) {
	set<string>::iterator it = structureFieldsSet.find(name);

	if(it != structureFieldsSet.end()) 
		return true;
	
	return false;
}

bool Program::isSumField(string name) {
	set<string>::iterator it = sumFieldsSet.find(name);
	
	if(it != sumFieldsSet.end())
		return true;
	
	return false;
}

void Program::addStructureField(string name) {
	structureFieldsSet.insert(name);
}

void Program::addSumField(string name) {
	sumFieldsSet.insert(name);
}

int Program::getFunctionNbArgs(string name) {
	map<string, DeclarationVMFunction *>::iterator vmIt = vmFunctionMap.find(name);
	if(vmIt != vmFunctionMap.end()) { return vmIt->second->getNbArgs(); }

	map<string, DeclarationProto *>::iterator pIt = protoMap.find(name);
	if(pIt != protoMap.end()) { return pIt->second->getNbArgs(); }

	return -1;
}
/**
 * Used to check if a label is a sum type field
 * returns: -1 if it's not
 * 	     0 if it doesnt have any constructor
 * 	     1 if it has a constructor
 */
int Program::isSumTypeField(string name) {
	map<string, bool>::iterator it = sumTypeFieldMap.find(name);		
	if(it == sumTypeFieldMap.end())
		return -1;
	
	if(it->second)
		return 1;
	
	return 0;
}

void Program::checkStack(string name) {
	if(expressionStack.size() != 0) {
		cerr << "Expression stack is not empty, odd.. " << expressionStack.size() << " expressions on the stack, parsing " << name << endl;
		cerr << "Stack dump" << endl;
		while(expressionStack.size() != 0) {
			Expression *e = expressionStack.top();
			cerr << e->toString() << endl;
			expressionStack.pop();
		}

		exit(-1);
	}
}

//end of utils

// *******************************************************************************************
// *											     *
// *					Declaration creation				     *
// *											     *
// *******************************************************************************************

//The last expresion on the stack defines the
//variable
//take it and assign it to the variable
void Program::addDeclarationVariable(string name, bool expression) {
	try {
		existsDeclarationName(name);

		DeclarationVariable *dv = new DeclarationVariable(name);
		DEBUG(cout << "Adding Global variable (" << dv->getName() << ")");
		variableMap[dv->getName()] = dv;

		if(expression) {
			dv->setExpression(expressionStack.top());
			expressionStack.pop();
		}

		//expression stack should be empty here
		checkStack(name);

		DEBUG(cout << "Added Global variable : " << dv->toString());
	} catch (int e) {
		cerr << "Global label (" << name << ") already exists" << endl;
	}
}

void Program::addDeclarationConstant(string name) {
	try {
		existsDeclarationName(name);

		DeclarationConstant *dc = new DeclarationConstant(name);
		DEBUG(cout << "Adding constant (" << dc->getName() << ")");
		constantMap[dc->getName()] = dc;

		dc->setExpression(expressionStack.top());
		expressionStack.pop();

		//expression stack should be empty at this point
		checkStack(name);

		DEBUG(cout << "Added const : " << dc->toString());
	} catch (int e) {
		cerr << "Label (" << name << ") already exists" << endl;
	}
}

void Program::addDeclarationFunction(string name, int nbArg) {
	try {
		existsDeclarationName(name, false);
		
		map<string, DeclarationProto *>::iterator it = protoMap.find(name);

		if(it != protoMap.end()) {
			if(it->second->getNbArgs() != nbArg) {
				cerr << "Function " << name << " prototype doesnt match earlier proto declaration" <<endl;
				return;
			}
		}



		DeclarationFunction *df = new DeclarationFunction(name);

		DEBUG(cout << "Adding Function (" << df->getName() << ")");
		functionMap[df->getName()] = df;

		DEBUG(cout << "Nb function " << nbArg << " stack size " << expressionStack.size());

		df->setExpression(expressionStack.top());
//		DEBUG(cout << "------" << endl << df->toString());
		expressionStack.pop();

		int i=0;
		for(i=0;i<nbArg;i++) {
			Expression *e = expressionStack.top();
			expressionStack.pop();

			ExpressionChar *ec = dynamic_cast<ExpressionChar *>(e);
			df->addArg(ec->getValue());
		}

		checkStack(name);
		DEBUG(cout << "Added function : " << df->toString());
	} catch(int e) {
		cerr << "Label (" << name << ") already exists" << endl;
	}
}

void Program::addDeclarationProto(string name, int nbArg) {
	DEBUG(cout << "Adding proto");
	try {
		map<string, DeclarationProto *>::iterator it = protoMap.find(name);
		if(it != protoMap.end()) {
			throw 0;
		}

		DeclarationProto *dp = new DeclarationProto(name, nbArg);
		protoMap[dp->getName()] = dp;
		DEBUG(cout << "Added proto: " << dp->toString());
	} catch (int e) {
		cerr << "Proto for " << name << " already exists" << endl;
	}

}

void Program::addDeclarationVMFunction(string name, int nbArg, Type *t) {
	DeclarationVMFunction *df = new DeclarationVMFunction(name, nbArg, t);
	vmFunctionMap[df->getName()] = df;

	DEBUG(cout << "Added VM Function: (" << df->getName() << ")");
	return;
}

void Program::addDeclarationType(string name) {
	try {
		existsDeclarationName(name);
		DeclarationType *dt = new DeclarationType(name);

		typeMap[dt->getName()] = dt;

		dt->setExpression(expressionStack.top());
		expressionStack.pop();
		
		checkStack(name);

		DEBUG(cout << "Added Type : " << dt->toString());

	} catch(int e) {
		cerr << "Label (" << name << ") already exists" << endl;
	}
}
//End of Declaration creation

// *******************************************************************************************
// *											     *
// *					Expression creation				     *
// *											     *
// *******************************************************************************************

//Creates a new char expression (IDs)
void Program::addExpressionChar(string value) {
	DEBUG(cout << "Adding char : " << value);
	ExpressionChar *e = new ExpressionChar(value);
	expressionStack.push(e);
}

//Creates a new integer expression
//and initializes its value
void Program::addExpressionInt(int value) {
	DEBUG(cout << "Adding int : " << value);
	ExpressionInt *e = new ExpressionInt(value, typeTable->getScalar("INT"));
	expressionStack.push(e);
}

//Creates a new string expression
//with its value
void Program::addExpressionString(string value) {
	DEBUG(cout << "Adding string : " << value);
	ExpressionString *e = new ExpressionString(value, typeTable->getScalar("STRING"));
	expressionStack.push(e);
}

//Nil expression
void Program::addExpressionNil() {
	DEBUG(cout << "Adding nil");
	ExpressionNil *e = new ExpressionNil();
	expressionStack.push(e);
}

//Create a new array expression by taking
//nbValue expressions from stack
//push result back on expression stack
void Program::addExpressionArray(int nbValue) {
	DEBUG(cout << "Adding Array..");
	ExpressionArray *e = new ExpressionArray();

	int i=0;
	for(i=0; i<nbValue; i++) {
		e->addElt(expressionStack.top());
		expressionStack.pop();
	}

	expressionStack.push(e);
	DEBUG(cout << "Array added : " << e->toString());
	//Infere type
}

//Create a new tuple expression by taking
//nbValue expressions from stack
//push result back on expression stack
void Program::addExpressionTuple(int nbValue) {
	DEBUG(cout << "Adding Tuple");
	ExpressionTuple *e = new ExpressionTuple();

	int i=0;
	for(i=0; i<nbValue; i++) {
		e->addElt(expressionStack.top());
		expressionStack.pop();
	}

	expressionStack.push(e);
	DEBUG(cout << "Tuple added : " << e->toString());
	//Infere type
}

//create parenth expression by taking one
//expression from stack
//and push result back
void Program::addExpressionParenth() {
	DEBUG(cout << "Adding parenth expression");

	ExpressionParenth *e = new ExpressionParenth();
	e->addExpression(expressionStack.top());
	expressionStack.pop();

	expressionStack.push(e);

	DEBUG(cout << "Added parenth expression : " << e->toString());
}

//Create a new unary expression from last expression from stack
//and operator given in arg
//Push the result expression back onto the stack
void Program::addExpressionUnaryOp(string op) {
	DEBUG(cout << "Adding unary op expression");

	ExpressionUnaryOp *e = new ExpressionUnaryOp();
	e->addExpression(expressionStack.top(), op);
	expressionStack.pop();
	
	expressionStack.push(e);
	DEBUG(cout << "Added unary op expression : " << e->toString());
}

//Take the two last expressions from expression stack
//the operator from operator stack
//Create a binary expression
//push the expression back onto the expression stack
void Program::addExpressionBinaryOp() {
	DEBUG(cout << "Adding binary op expression");

	ExpressionBinaryOp *e = new ExpressionBinaryOp();

	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	e->addExpression(e1, e2, operatorStack.top());
	operatorStack.pop();

	expressionStack.push(e);

	DEBUG(cout << "Added binary expression : " << e->toString());
}

//int nbExpressions is the number of expressions on the
//stack to add to ExpressionVall
//Push the result back onto the expression stack
void Program::addExpressionVal(int nbExpression) {
	DEBUG(cout << "Adding Expression val");

	ExpressionVal *e = new ExpressionVal();

	if(nbExpression == 1) {
		DEBUG(cout << "test");
		e->addExpression(expressionStack.top());
		expressionStack.pop();
	} else if(nbExpression == 2) {
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addExpression(e1, e2);
	}
	
	expressionStack.push(e);

	DEBUG(cout << "Added expression val : " << e->toString());
}

void Program::addExpressionFun(int nbExpr) {
	int i=0;

	ExpressionFun *ef = new ExpressionFun();

	for(i=0;i<nbExpr;i++) {
		ef->addExpression(expressionStack.top());
		expressionStack.pop();
	}
	expressionStack.push(ef);
}

void Program::addExpressionMultipleOp(int nbExpr) {
	stack<Expression *> eStack;
	stack<string> sStack;

	DEBUG();

	int i =0;

	//Get expressions, and operators, and revert the stack
	for(i=0; i<nbExpr; i++) {
		eStack.push(expressionStack.top());
		expressionStack.pop();

		sStack.push(operatorStack.top());
		operatorStack.pop();
	}

	//Get the first expression

	eStack.push(expressionStack.top());
	expressionStack.pop();

	for(i=0; i<nbExpr; i++) {
		ExpressionBinaryOp *e = new ExpressionBinaryOp();

		Expression *e1 = eStack.top();
		eStack.pop();

		Expression *e2 = eStack.top();
		eStack.pop();

		e->addExpression(e1, e2, sStack.top());
		sStack.pop();

		eStack.push(e);
	}
	
	expressionStack.push(eStack.top());
	eStack.pop();
}

void Program::addExpressionIf(int nb) {
	if(nb < 2) {
		throw 0;
	}

	DEBUG(cout << "Adding expression if ");

	ExpressionIf *e = new ExpressionIf();

	if(nb == 2) {
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addExpression(e1, e2, NULL);
	} else if(nb == 3) {

		Expression *e3 = expressionStack.top();
		expressionStack.pop();
	
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addExpression(e1, e2, e3);
	}

	DEBUG(cout << "Added expression if: " << e->toString());
	
	expressionStack.push(e);
}

void Program::addExpressionWhile(void) {
	DEBUG(cout << "Adding expression while");
	ExpressionWhile *e = new ExpressionWhile();

	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	e->addExpression(e1, e2);

	expressionStack.push(e);
	DEBUG(cout << "Added expression while :" << e->toString());
}

void Program::addExpressionFor(int nbExpr) {
	DEBUG(cout << "Adding expression for");

	ExpressionFor *e = new ExpressionFor();

	if(nbExpr == 3) {
		Expression *e4 = expressionStack.top();
		expressionStack.pop();

		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		Expression *id = expressionStack.top();
		expressionStack.pop();

		e->addExpression(id, e1, e2, NULL, e4);

	} else {
		Expression *e4 = expressionStack.top();
		expressionStack.pop();

		Expression *e3 = expressionStack.top();
		expressionStack.pop();

		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		Expression *id = expressionStack.top();
		expressionStack.pop();

		e->addExpression(id, e1, e2, e3, e4);
	}

	expressionStack.push(e);

	DEBUG(cout << "Added expression for: " << e->toString());
}

void Program::addExpressionList(int nbElts) {
	DEBUG(cout << "Adding list expression");

	ExpressionList *e = new ExpressionList();

	int i;
	for(i=0;i<nbElts; i++) {
		e->addElt(expressionStack.top());
		expressionStack.pop();
	}

	expressionStack.push(e);

	DEBUG(cout << "Added list expression : " << e->toString());

}

void Program::addExpressionUndef(void) {
	DEBUG(cout << "Adding undef expression");

	ExpressionUndef *e = new ExpressionUndef();

	expressionStack.push(e);
	
	DEBUG(cout << "Added undef expression");

}

void Program::addExpressionLet(void) {
	DEBUG(cout << "Adding let expression");

	ExpressionLet *e = new ExpressionLet();

	Expression *e3 = expressionStack.top();
	expressionStack.pop();

	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	e->addExpression(e1, e2, e3);

	expressionStack.push(e);

	DEBUG(cout << "Added let expression : " << e->toString());
}

void Program::addExpressionCall() {
	DEBUG(cout << "Adding call expression");

	ExpressionCall *e = new ExpressionCall();
	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	e->addExpression(e1, e2);

	expressionStack.push(e);

	DEBUG(cout << "Added call expression : " << e->toString());
}

void Program::addExpressionPointer() {
	DEBUG(cout << "Adding pointer expression");

	ExpressionPointer *e = new ExpressionPointer();
	e->addExpression(expressionStack.top());
	expressionStack.pop();
	expressionStack.push(e);

	DEBUG(cout << "Added pointer expression : " << e->toString());
}

void Program::addExpressionSet(int nbExpr) {
	DEBUG(cout << "Adding set expression");

	ExpressionSet *e = new ExpressionSet();

	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	addExpressionDot(nbExpr);

	Expression *e1 = expressionStack.top();
	expressionStack.pop();


	e->addExpression(e1,e2);

	expressionStack.push(e);

	DEBUG(cout << "Added set expression : " << e->toString());
}

void Program::addExpressionStruct(int nbExpr) {
	DEBUG(cout << "Adding struct expression");
	ExpressionStruct *e = new ExpressionStruct();

	int i=0;
	for(i=0; i<nbExpr; i++) {
		e->addElement(expressionStack.top());
		expressionStack.pop();
	}

	expressionStack.push(e);
	DEBUG(cout << "struct type added : " << e->toString());
}

void Program::addExpressionSumElement(int nbExpr) {
	DEBUG(cout << "Adding sum element");
	ExpressionSumElement *e = new ExpressionSumElement();

	if(nbExpr == 1) {
		//String name
		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addElements(e1, NULL);
		sumTypeFieldMap[e1->toString()] = false;
	} else if(nbExpr == 2) {
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addElements(e1, e2);
		sumTypeFieldMap[e1->toString()] = true;
	}

	expressionStack.push(e);
	DEBUG(cout << "Added sum element: " << e->toString());
}

void Program::addExpressionSum(int nbExpr) {
	DEBUG(cout << "Adding sum expression " << nbExpr);

	ExpressionSum *e = new ExpressionSum();

	int i=0;
	for(i=0; i<nbExpr; i++) {
		e->addElement(expressionStack.top());
		expressionStack.pop();
	}

	expressionStack.push(e);
	DEBUG(cout << "Added sum expression: " << e->toString());
}

void Program::addExpressionDot(int nbExpr) {
	DEBUG(cout << "Adding dot expression ");
	int i=0;

	for(i=0; i<nbExpr; i++) {
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		ExpressionDot *e = new ExpressionDot();
		e->addExpression(e1, e2);

		expressionStack.push(e);
	}
	DEBUG(cout << "Added dot expression: " << (expressionStack.top())->toString());
}

void Program::addExpressionFunCall(int nbExpr) {
	DEBUG(cout << "Adding function call expression" << endl);

	ExpressionFunCall *e = new ExpressionFunCall();

	int i=0;
	for(i=0;i<nbExpr;i++) {
		e->addExpression(expressionStack.top());
		expressionStack.pop();
	}

	e->setFunction(expressionStack.top());
	expressionStack.pop();

	expressionStack.push(e);

	DEBUG(cout << "added function call: " << e->toString());
}

void Program::addExpressionStructCons(int nbExpr) {
	DEBUG(cout << "Adding struct constructor expression");

	int i=0;

	ExpressionStructCons *e = new ExpressionStructCons();
	
	for(i=0; i<nbExpr; i++) {
		Expression *e2 = expressionStack.top();
		expressionStack.pop();

		Expression *e1 = expressionStack.top();
		expressionStack.pop();

		e->addExpression(e1, e2);
	}
	
	expressionStack.push(e);
	
	DEBUG(cout << "Added struct constructor: " << e->toString());
}

void Program::addExpressionMatchElement(int nbExpr) {
	DEBUG(cout << "Adding match element");

	Expression *e3 = expressionStack.top();
	expressionStack.pop();

	Expression *e2;

	if(nbExpr == 1) {
		e2 = NULL;
	} else if(nbExpr == 2) {
		e2 = expressionStack.top();
		expressionStack.pop();
	}

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	ExpressionMatchElement *e = new ExpressionMatchElement(e1, e2, e3);
	expressionStack.push(e);
	DEBUG(cout << "Added match element " << e->toString());
}

void Program::addExpressionMatch(int nbExpr) {
	DEBUG(cout << "Adding match expression");

	DEBUG(cout << "nbExpr " << nbExpr);

	ExpressionMatch *e = new ExpressionMatch();

	int i=0;
	for(i=0;i<nbExpr;i++) {
		e->addElement(expressionStack.top());
		expressionStack.pop();
	}
	
	e->setExpressionE1(expressionStack.top());
	expressionStack.pop();

	expressionStack.push(e);
	DEBUG(cout << "Added match expression " << e->toString());
}

void Program::addExpressionCons() {
	DEBUG(cout << "Adding structure constructor");

	Expression *e2 = expressionStack.top();
	expressionStack.pop();

	Expression *e1 = expressionStack.top();
	expressionStack.pop();

	ExpressionCons *e = new ExpressionCons(e1, e2);

	expressionStack.push(e);

	DEBUG(cout << "Added structure constructor: " << e->toString());
}
//End of expression creation
