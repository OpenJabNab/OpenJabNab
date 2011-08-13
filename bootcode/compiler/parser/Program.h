#ifndef __PROGRAM__
#define __PROGRAM__

#include <cstdlib>
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

//Declarations
#include "Declaration.h"
#include "DeclarationConstant.h"
#include "DeclarationFunction.h"
#include "DeclarationProto.h"
#include "DeclarationType.h"
#include "DeclarationVariable.h"
#include "DeclarationVMFunction.h"

//Expressions
#include "Expression.h"
#include "ExpressionArray.h"
#include "ExpressionBinaryOp.h"
#include "ExpressionCall.h"
#include "ExpressionChar.h"
#include "ExpressionCons.h"
#include "ExpressionDot.h"
#include "ExpressionFor.h"
#include "ExpressionFun.h"
#include "ExpressionFunCall.h"
#include "ExpressionIf.h"
#include "ExpressionInt.h"
#include "ExpressionLet.h"
#include "ExpressionList.h"
#include "ExpressionMatch.h"
#include "ExpressionMatchElement.h"
#include "ExpressionNil.h"
#include "ExpressionParenth.h"
#include "ExpressionPointer.h"
#include "ExpressionSet.h"
#include "ExpressionString.h"
#include "ExpressionTuple.h"
#include "ExpressionSum.h"
#include "ExpressionSumElement.h"
#include "ExpressionStruct.h"
#include "ExpressionStructCons.h"
#include "ExpressionUnaryOp.h"
#include "ExpressionUndef.h"
#include "ExpressionVal.h"
#include "ExpressionWhile.h"

//Types
#include "Type.h"
#include "TypeArray.h"
#include "TypeInt.h"
#include "TypeString.h"
#include "TypeTable.h"

//Utils
#include "Debug.h"
#include "VariableTable.h"
#include "VMFunctions.h"

using namespace std;

class Program {
public:
	static Program* getInstance();

	//Utils
	
	/**
	 * Checks if a label is already exists
	 * throws exceptioin if true
	 */
	void addOperator(string op);
	void existsDeclarationName(string name, bool all);
	void initScope(void);
	void addScope(void);
	void removeScope(void);
	void addLocalVariable(string name);
	void removeLocalVariable(string name);
	void addGlobalVariable(string name);
	bool isVariable(string name, bool local=false);
	bool isGlobalVariable(string name);
	bool isLocalVariable(string name);
	bool isStructureField(string name);
	bool isSumField(string name);
	void addStructureField(string name);
	void addSumField(string name);
	int getFunctionNbArgs(string name);
	int isSumTypeField(string name);
	//Utils end

	//Variable Declaration
	/**
	 * Add a variable with its expression
	 */
	void addDeclarationVariable(string name, bool expression);
	void addDeclarationConstant(string name);
	void addDeclarationFunction(string name, int nbArg);
	void addDeclarationProto(string name, int nbArg);
	void addDeclarationVMFunction(string name, int nbArg, Type *t); 
	void addDeclarationType(string name);
	//Variable Declaration end

	//Expressions
	void addExpressionChar(string value);
	void addExpressionInt(int value);
	void addExpressionString(string value);
	void addExpressionNil();
	void addExpressionArray(int nbValue);
	void addExpressionTuple(int nbValue);
	void addExpressionParenth();
	void addExpressionUnaryOp(string op);
	void addExpressionBinaryOp();
	void addExpressionVal(int nbExpression);
	void addExpressionFun(int nbExpr);
	void addExpressionMultipleOp(int nbExpr);
	void addExpressionIf(int nb);
	void addExpressionWhile(void);
	void addExpressionFor(int nbExpr);
	void addExpressionList(int nbElts);
	void addExpressionUndef(void);
	void addExpressionLet(void);
	void addExpressionCall(void);
	void addExpressionPointer(void);
	void addExpressionSet(int nbExpr);
	void addExpressionStruct(int nbExpr);
	void addExpressionSumElement(int nbExpr);
	void addExpressionSum(int nbExpr);
	void addExpressionDot(int nbExpr);
	void addExpressionFunCall(int nbExpr);
	void addExpressionStructCons(int nbExpr);
	void addExpressionMatchElement(int nbExpr);
	void addExpressionMatch(int nbExpr);
	void addExpressionCons(void);
	//Expressions end

	//Operators

private:
	Program();

	static Program *instance;

	TypeTable *typeTable;

	map<string, DeclarationFunction *> functionMap;

	map<string, DeclarationVariable *> variableMap;

	map<string, DeclarationConstant *> constantMap;

	map<string, DeclarationVMFunction *> vmFunctionMap;

	map<string, DeclarationType *> typeMap;

	//Using a seperate map because declarationMap
	//already contains function names
        map<string, DeclarationProto *> protoMap;

	//Used to to store whether the sum type has a constructor or not
	map<string, bool> sumTypeFieldMap;
	
	/**
	 * This stack contains sub expressions
	 * that have been parsed
	 */
	stack<Expression *> expressionStack; 

	/**
	 * This stack contains operators
	 * of binary operation expressions
	 */
	stack<string> operatorStack;

	vector<VariableTable *>scopeTable;

	VariableTable *globalVariableTable;

	set<string> structureFieldsSet;

	set<string> sumFieldsSet;

	void checkStack(string name);
};

#endif 
