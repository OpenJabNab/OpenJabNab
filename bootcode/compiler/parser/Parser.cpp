#include "Parser.h"

Parser* Parser::_instance = NULL;

Parser* Parser::getInstance() {
	if(_instance == NULL) {
		_instance = new Parser();
	} 

	return _instance;
}

int Parser::getToken(void) {
	if(giveBack_) {
		giveBack_ = false;
		return token;
	}

	token = yylex();
	return token;
}

void Parser::giveBack(void) {
	giveBack_ = true;
}

int Parser::parse(void) {
	while(getToken())  {
		if(token == FUN) {
			if(parseFun()) {
				cerr << "Parse error, parseFun" << endl;
				return -1;
			}
		} else if(token == TYPE) {
			if(parseType()) {
				cerr << "Parse error, parseType" << endl;
				return -1;
			}
		} else if(token == VAR) {
			if(parseVar()) {
				cerr << "Parse error, parseVar" << endl;
				return -1;
			}
		} else if(token == CONST) {
			if(parseConst()) {
				cerr << "Parse error, parseConst" << endl;
				return -1;
			}
		} else if(token == PROTO) {
			if(parseProto()) {
				cerr << "Parse error, parseProto" << endl;
				return -1;
			}
		} else {
			DEBUG(cout << token);
			cerr << "Parse error, unexpected token." << endl;
			return -1;
		}
		return 0;
	}
}

// *******************************************************************************************
// *											     *
// *				Variable Declaration Parsing				     *
// *											     *
// *******************************************************************************************
int Parser::parseVar(void) {
	if((getToken()) != VAR_ID) {
		cerr << "Parser: Var name expected" << endl;
		throw VAR_ID;
	}
	string varName = this->sVal;

	getToken();
	if(token == DOUBLE_POINT_VIRGULE) {
		//Var declaration : var var_name ;;
		program->addDeclarationVariable(varName, false);
		program->addGlobalVariable(varName);
		return 0;
	} else if(token == '=') {
		if(!parseVal()) {
			getToken();
			if(token == DOUBLE_POINT_VIRGULE) {
				program->addDeclarationVariable(varName, true);
				program->addGlobalVariable(varName);
				return 0;
			} else {
				cerr << "Parser: Expected ;;" << endl;
				return -1;
			}
		}
	} else {
		cerr << "Parser: Unexpected token. Expected = or ;;" << endl;
		return -1;
	}
}
int Parser::parseVal(void) {
	int k;
	
	if(k=parseVal3()) return k;
	
	getToken();

	if(token == 0) return 0;

	if(token != DOUBLE_DEUX_POINTS) {
		giveBack();
		program->addExpressionVal(1);
		return 0;
	}

	if(k=parseVal()) return k;
	program->addExpressionVal(2);
	return 0;
}

int Parser::parseVal3(void) {
	int k;

	if(k=parseVal4()) return k;

	while(1) {

		getToken();

		if(token == 0) return 0;
		
		if(token == '+') { program->addOperator("+"); } 
		
		else if(token == '-') { program->addOperator("-"); }
		
		else { giveBack(); return 0; }

		//is this supposed to be parseVal3?
		// parseVal4 [+-] parseVal3 ?
		// 
		// Solved: the loop does this.
		if(k=parseVal4()) return k;

		program->addExpressionBinaryOp();
	}

}

int Parser::parseVal4(void) {
	int k;

	if(k=parseVal5()) return k;
	
	while(1) {

		getToken();
		
		if(token == 0) return 0;

		if(token == '*') { program->addOperator("*"); }
		
		else if(token == '/') { program->addOperator("/"); }

		else if(token == '%') { program->addOperator("%"); }

		else { giveBack(); return 0; }

		if(k=parseVal5()) return k;

		program->addExpressionBinaryOp();
	}
}

int Parser::parseVal5(void) {
	int k;

	if(k=parseVal6()) return k;

	while(1) {
		getToken();
		
		if(token == 0) return 0;

		if(token == '&') { program->addOperator("&"); }

		else if(token == '|') { program->addOperator("|"); }

		else if(token == '^') { program->addOperator("^"); }

		else if(token == LS) { program->addOperator("<<"); }

		else if(token == RS) { program->addOperator(">>"); }

		else { giveBack(); return 0; } 

		if(k=parseVal6()) return k;

		program->addExpressionBinaryOp();
	}
}

int Parser::parseVal6(void) {
	int k;

	//S. does: if(getToken() != 0) return parseVal7()
	//Why?
	
	getToken();


	if(token == 0) return parseVal7();

	if(token == '-') {
		//again he consumes a token and calls parseVal6
		//why? 
		getToken();
		if(token == NOMBRE) {
			program->addExpressionInt( - this->iVal );
			return 0;
		} 
		giveBack();
		//here it's an "- expression" type expression
		if(k=parseVal6()) return k;
		program->addExpressionUnaryOp("-");
		return 0;
	} else if(token == '~') {
		if(k=parseVal6()) return k;
		program->addExpressionUnaryOp("~");
		return 0;
	}
	giveBack();

	return parseVal7();

}

int Parser::parseVal7(void) {
	int k;

	getToken();

	if(token == 0) { cerr << "Parser: term expected (found EOF)" << endl; return -1;}

	if(token == '(') {
		if(k=parseVal()) return k;
		
		getToken();
		
		if(token == 0) { cerr << "Parser: ')' expected (found EOF)" << endl; return -1;} 
		if(token == ')')  { 
			program->addExpressionParenth();
			return 0; 
		}


		return -1;
	} else if(token == '[') {
		//Handle tuples
		int nval = 0;

		while(1) {
			getToken();

			if(token == 0) {
				cerr << "Parser: ']' expected (found EOF)" << endl; 
				return -1;
			} 

			if(token == ']') {
				/* create the tuple */
				program->addExpressionTuple(nval);
				return 0;
			}
			
			giveBack();

			if(k=parseVal()) return k;
			nval++;
		}
	} else if(token == '{') {
		//Handle arrays
		int nval=0;

		while(1) {
			getToken();

			if(token == 0) {
				cerr << "Parser: '}' expected (found EOF)" << endl;
				return -1;
			}

			if(token == '}') {
				/* create the array */
				program->addExpressionArray(nval);
				return 0;
			}
			
			giveBack();

			if(k=parseVal()) return k;
			nval++;
		}
	} else if(token == NIL) {
		program->addExpressionNil();
		return 0;
	} else if(token == STRING) {
		program->addExpressionString(this->sVal);
		return 0;
	} else if(token == NOMBRE) {
		program->addExpressionInt(this->iVal);
		return 0;
	} else if(token == ID) {
		program->addExpressionChar(this->sVal);
		return 0;
	} else {
		cerr << "Parser: expected term" << endl;
	}
} //Variable Declaration Parsing end

// *******************************************************************************************
// *											     *
// *				Const Parsing						     *
// *											     *
// *******************************************************************************************
int Parser::parseConst(void) {
	if((getToken()) != CONST_ID) {
		cerr << "Parser: Const name expected" << endl;
		throw CONST_ID;
	}
	string constName = this->sVal;

	getToken();
	if(token == '=') {
		if(!parseVal()) {
			getToken();
			if(token == DOUBLE_POINT_VIRGULE) {
				program->addGlobalVariable(constName);
				program->addDeclarationConstant(constName);
				return 0;
			} else {
				cerr << "Parser: Expected ;;" << endl;
				return -1;
			}
		}
		return -1;
	} else {
		cerr << "Parser: Unexpected token. Expected =" << endl;
		return -1;
	}
}

// *******************************************************************************************
// *											     *
// *				Proto Parsing						     *
// *											     *
// *******************************************************************************************

int Parser::parseProto(void) {
	int k;
	string protoName;

	getToken();

	if(token != PROTO_ID) {
		cerr << "Parser: proto id expected. " << token << endl;
		return -1;
	}

	protoName = this->sVal;

	if(!getToken()) {
		cerr << "Parser: expected integer or '=', found EOF" << endl;
		return -1;
	}

	if(token == NOMBRE) {
		if(!getToken()) {
			cerr << "Parser: expected ';;', found EOF" << endl;
			return -1;
		}

		if(token != DOUBLE_POINT_VIRGULE) {
			cerr << "Parser: expected ';;'" << endl;
			return -1;
		}
		program->addDeclarationProto(protoName, this->iVal);
		return 0;

	} else if(token == '=') {
		//TODO
		return 0;
	} else {
		cerr << "Parser: proto, unexpected token." << endl;
		return -1;
	}




}

// *******************************************************************************************
// *											     *
// *				Function Parsing					     *
// *											     *
// *******************************************************************************************

int Parser::parseFun(void) {
	if((getToken()) != FUN_ID) {
		cerr << "Parser: Function name expected." << endl;	
		throw FUN_ID;
	} 

	string functionName = this->sVal;

	int k;

	int narg=0;

	//initialize a new scope for the function
	program->addScope();

	do {
		if(!getToken()) {
			cerr << "Parser: expected argument or '=', found EOF" << endl;
			return -1;
		}

		if(token == FUN_ARG_ID) {
			program->addExpressionChar(this->sVal);
			//add the label to the local var table
			program->addLocalVariable(this->sVal);
			narg++;
		} else if(token != '=') {
			cerr << "Parser: expected function arguments or '='" << endl;
			return -1;
		}
	} while (token != '=');

	program->addDeclarationProto(functionName, narg);

	if(k=parseFunExpression()) return k;
	
	if(!getToken()) {
		cerr << "Parser: unexpected term, expected ';;'" << endl;
		return -1;
	}

	if(token != DOUBLE_POINT_VIRGULE) {
		cerr << "Parser: unexpected term, expected ';;'" << endl;
		return -1;
	}
	//add function declaration	
	program->addDeclarationFunction(functionName, narg);

	//delete scope
	program->removeScope();
	return 0;
}

int Parser::parseFunExpression() {
	int k;
	int nbExpressions = 0;

	while(1) {
		if(k=parseExpression()) return k;

		nbExpressions++;

		if(!getToken()) return 0;

		if(token != ';') {
			giveBack();
			break;
			//return 0;
		}


	}
	program->addExpressionFun(nbExpressions);
	return 0;
}

int Parser::parseExpression() {
	int k;
	int nbElts=1;

	if(k=parseArithm()) return k;

	if(!getToken()) return 0;
	giveBack();

	//List expression Arithm::Expr
	while(1) {
		if(!getToken()) return 0;
		if(token != DOUBLE_DEUX_POINTS) {
			giveBack();
			break;
		}

		if(k=parseArithm()) return k;

		nbElts++;
	}

	if(nbElts > 1) 
		program->addExpressionList(nbElts);
	return 0;
}

int Parser::parseArithm() {
	int k;

	if(k=parseA1()) return k;


	while(1) {
		if(!getToken()) return 0;

		if((token != ANDAND) && (token != OROR)) {
			giveBack();
			return 0;
		}

		if(token == ANDAND) { program->addOperator("&&"); /*return 0;*/ }

		else if(token == OROR) { program->addOperator("||"); /*return 0;*/ }

		if(k=parseA1()) return k;

		program->addExpressionBinaryOp();

	}

	//should never get here
	
	return -1;
}

int Parser::parseA1(void) {
	int k;

	if((getToken()) && (token == '!')) {
		if(k=parseA1()) return k;
		program->addExpressionUnaryOp("!");
		return 0;
	}
	giveBack();
	return parseA2();
}

int Parser::parseA2(void) {
	int k;

	if(k=parseA3()) return k;

	while(1) {
		if(!getToken()) return 0;
		
		if(token == EQ) { program->addOperator("=="); }
		
		else if(token == NE) { program->addOperator("!="); }
		
		else if(token == '<') { program->addOperator("<"); }

		else if(token == '>') { program->addOperator(">"); }

		else if(token == LE) { program->addOperator("<="); }

		else if(token == GE) { program->addOperator(">="); }

		else {
			giveBack();
			return 0;
		}

		if(k=parseA3()) return k;

		program->addExpressionBinaryOp();
	}

	//should not get to this point
	
	return -1;
}

int Parser::parseA3(void) {
	int k;

	if(k=parseA4()) return k;


	while(1) {
		if(!getToken()) return 0;
		
		if(token == '+') { program->addOperator("+"); }
		
		else if(token == '-') { program->addOperator("-"); }
		
		else {
			giveBack();
			return 0;
		}

		if(k=parseA4()) { return k; }

		program->addExpressionBinaryOp();

	}


	return -1;
}

int Parser::parseA4(void) {
	int k;

	if(k=parseA5()) { return k; }

	while(1) {
		if(!getToken()) return 0;

		if(token == '*') { program->addOperator("*"); }

		else if(token == '/') { program->addOperator("/"); }

		else if(token == '%') { program->addOperator("%"); }

		else {
			giveBack();
			return 0;
		}

		if(k=parseA5()) { return k; }

		program->addExpressionBinaryOp();
	}

	return -1;
}

int Parser::parseA5(void) {
	int k;

	if(k=parseA6()) { return k; }

	while(1) {
		if(!getToken()) return 0;

		if(token == '&') { program->addOperator("&"); }

		else if(token == '|') { program->addOperator("|"); }

		else if(token == '^') { program->addOperator("^"); }

		else if(token == LS) { program->addOperator("<<"); }

		else if(token == RS) { program->addOperator(">>"); }

		else {
			giveBack();
			return 0;
		}

		if(k=parseA6()) { return k; }

		program->addExpressionBinaryOp();

	}

	return -1;
}

int Parser::parseA6(void) {
	int k;

	if(!getToken()) return parseTerm();

	if(token == '-') {
		if(!getToken()) return parseA6();

		if(token == NOMBRE) { program->addExpressionInt(- this->iVal); return 0; }
		
		giveBack();
		if(k=parseA6()) return k;

		program->addExpressionUnaryOp("-");
		return 0;

	} else if(token == '~') {
		if(k=parseA6()) { return k;}
		program->addExpressionUnaryOp("~");
		return 0;
	}
	giveBack();
	return parseTerm();
}

int Parser::parseTerm(void) {
	int k;

	if(!getToken()) {
		cerr << "Parser: Term expected, found EOF" << endl;
		return -1;
	}

	if(token == '(') {
		if(k=parseFunExpression()) return k;
		
		if(!getToken()) {
			cerr << "Parser: ')' expected, found EOF" << endl;
			return -1;
		}
		if(token != ')') {
			cerr << "Parser ')' expected" << endl;
			return -1;
		}

		program->addExpressionParenth();
		return 0;

	} else if(token == '[') { 
		if(getToken()) {
			if(token == ID) {
				if(program->isGlobalVariable(this->sVal) && 
					program->isStructureField(this->sVal)) {

					program->addExpressionChar(this->sVal);
					return parseFields();
				}
			}
			giveBack();
		}

		int nval=0;

		while(1) {
			if(!getToken()) {
				cerr << "Parser: ']' expected, found EOF" << endl;
				return -1;
			}

			if(token == ']') {
				//create tuple
				program->addExpressionTuple(nval);
				return 0;
			}
			giveBack();

			if(k=parseExpression()) return k;

			nval++;
		}
	} else if(token == '{') {
		int nval=0;

		while(1) {
			if(!getToken()) {
				cerr << "Parser: '}' expected, found EOF" << endl;
				return -1;
			}

			if(token == '}') {
				//create tab
				program->addExpressionArray(nval);
				return 0;
			}
			
			giveBack();

			if(k=parseExpression()) return k;
			nval++;
		}
	} else if(token == IF) return parseIf();
	
	else if(token == LET) return parseLet();

	else if(token == SET) return parseSet();

	else if(token == WHILE) return parseWhile();

	else if(token == FOR) return parseFor();

	else if(token == MATCH) return parseMatch();

	else if(token == CALL) return parseCall();

	else if(token == UPDATE) return parseUpdate();

	else if(token == NIL) { program->addExpressionNil(); return 0; }

	else if(token == STRING) { program->addExpressionString(this->sVal); return 0; }

	else if(token == ID) {
		DEBUG();
		program->addExpressionChar(this->sVal);
		return parseRef();
	}

	else if(token == NOMBRE) { program->addExpressionInt(this->iVal); return 0; }

	else if(token == '#') return parsePntFun();

	else {
		cerr << "Parser: unexpected term " << token << endl;
		return -1;
	}
}

int Parser::parseIf(void) {
	int k;

	//if e1 then e2 (else e3)

	if(k=parseExpression()) return k;

	if(!getToken()) {
		cerr << "Parser: expected keyword: then, found EOF" << endl;
		return -1;
	}

	if(token != THEN) {
		cerr << "Parser: unexpected token, expected keyword: then." << endl;
		return -1;
	}
	
	if(k=parseExpression()) return k;

	if((getToken()) && (token == ELSE)) {
		if(k=parseExpression()) return k;
		program->addExpressionIf(3);
		return 0;
	} else {
		program->addExpressionIf(2);
		giveBack();
		return 0;
	}

	return -1;
}

int Parser::parseWhile(void) {
	int k;

	if(k=parseExpression()) return k;
	
	if(!getToken()) {
		cerr << "Parser: expected keyword: do, found EOF" << endl;
		return -1;
	}

	if(token != DO) {
		cerr << "Parser: unexpected token, expected keyword: do." << endl;
		return -1;
	}
	
	if(k=parseExpression()) return k;

	program->addExpressionWhile();
	return 0;
}

int Parser::parseFor(void) {
	int k;

	//create scope
	program->addScope();

	if(!getToken()) {
		cerr << "Parser: label expected, found EOF" << endl;
		return -1;
	}

	if (token != ID) {
		cerr << "Parser: unexpected token, label expected." << endl;
		return -1;
	}

	program->addExpressionChar(this->sVal);
	program->addLocalVariable(this->sVal);

	if(!getToken()) {
		cerr << "Parser: '=' expected, found EOF" << endl;
		return -1;
	}

	if(token != '=') {
		cerr << "Parser: unexpected token, expected '='" << endl;
		return -1;
	}

	if(k=parseExpression()) return k;

	if(!getToken()) {
		cerr << "Parser: ';' expected, found EOF" << endl;
		return -1;
	}

	if(token != ';') {
		cerr << "Parser: unexpected token, expected ';'" << endl;
		return -1;
	}

	if(k=parseExpression()) return k;
	
	if(!getToken()) {
		cerr << "Parser: expected keyword do, found EOF" << endl;
		return -1;
	}

	if(token == ';') {
		if(k=parseExpression()) return k;

		DEBUG(cout << "Accepting do");

		if(!getToken()) {
			cerr << "Parser: do keyword expected, found EOF" << endl;
			return -1;
		}

		if(token != DO) {
			cerr << "Parser: unexpected token, do keyword expected" << endl;
			return -1;
		}

		if(k=parseExpression()) return k;

		program->addExpressionFor(4);
		program->removeScope();
		return 0;

	} else {
		giveBack();

		if(!getToken()) {
			cerr << "Parser: do keyword expected, found EOF" << endl;
			return -1;
		}

		if(token != DO) {
			cerr << "Parser: unexpected token, do keyword expected." << endl;
			return -1;
		}

		if(k=parseExpression()) return k;
		
		program->addExpressionFor(3);
		program->removeScope();
		return 0;
	}

	return -1;
}

int Parser::parseMatch(void) {
	int k;

	if(k=parseExpression()) return k;

	if(!getToken()) {
		cerr << "Parser: with keyword expected, found EOF" << endl;
		return -1;
	}

	if(token != WITH) {
		cerr << "Parser: unexpected token, with keyword expected." << endl;
		return -1;
	}

	int count=0;

	if(k=parseMatchCons(&count)) return k;
	program->addExpressionMatch(count);
	return 0;
}

int Parser::parseMatchCons(int *count) {
	int k;

	if(!getToken()) {
		cerr << "Parser: '(' expected, found EOF" << endl;
		return -1;
	}

	if(token != '(') {
		cerr << "Parser: unexpected token, expected '('" << endl;
		return -1;
	}

	if(!getToken()) {
		cerr << "Parser: constructor expected (found EOF)" << endl;
		return -1;
	}

	if(token == '_') {
		program->addExpressionUndef();
		if(!getToken()) {
			cerr << "Parser: '->' expected, found EOF" << endl;
			return -1;
		}

		if(token != FLECHE) {
			cerr << "Parser: unexpected token, expected '->'" << endl;
			return -1;
		}

		if(k=parseFunExpression()) return k;


		if(!getToken()) {
			cerr << "Parser: ')' expected, found EOF" << endl;
			return -1;
		}

		if(token != ')') {
			cerr << "Parser: unexpected token, expected ')'" << endl;
			return -1;
		}
		(*count)++;
		program->addExpressionMatchElement(1);
		return 0;
	}

	if(token == ID) {
		int fType = program->isSumTypeField(this->sVal);
		DEBUG(cout << "Found id, " << fType);
		if(fType >= 0) {
			program->addExpressionChar(this->sVal);
			if(fType == 1) {
				//If field has constructor, parse.
				if(k=parseLocals()) return k;
			}

			if(!getToken()) {
				cerr << "Parser: '->' expected, found EOF" << endl;
				return -1;
			}

			if(token != FLECHE) {
				cerr << "Parser: unexpected token, '->' expected" << endl;
			}

			if(k=parseFunExpression()) return k;

			if(!getToken()) {
				cerr << "Parser: ')' expected, found EOF" << endl;
				return -1;
			}
			if(token != ')') {
				cerr << "Parser: unexpected token, ')' expected." << endl;
			}
			
			//if fType == 0, no constructor, 1 arg
			//else constructor, 2args
			(*count)++;
			program->addExpressionMatchElement(fType+1);

			if(getToken() && token == '|') {
				if(k=parseMatchCons(count)) return k;
				return 0;
			}
			giveBack();
			return 0;
		}
	}
	
	cerr << "Parser: constructor expected" << endl;
	return -1;
}

int Parser::parseUpdate(void) {
	int k;

	if(k=parseExpression()) return k;

	if(!getToken()) {
		cerr << "Parser: expected keyword 'with', found EOF" << endl;
		return -1;
	}

	if(token != WITH) {
		cerr << "Parser: unexpected token, expected keyword 'with'." << endl;
		return -1;
	}

	if(!getToken()) {
		cerr << "Parser: expected '[', found EOF" << endl;
		return -1;
	}

	if(token != '[') {
		cerr << "Parser: unexpected token, expected '['" << endl;
		return -1;
	}

	if(k=parseUpdateVals()) return k;
	//create update expression
	return 0;
}

int Parser::parseUpdateVals() {
	int k;

	int n=0;

	while(1) {
		if(!getToken()) {
			cerr << "Parser: expected '[', found EOF" << endl;
			return -1;
		}

		if(token == ']') {

		}

		//TODO Finish, but is not used so it can wait.
	}
}

int Parser::parseLet() {
	int k;

	//Create a new scope
	program->addScope();

	if(k=parseExpression()) return k;
	
	if(!getToken()) {
		cerr << "Parser: expected '->', found EOF" << endl;
		return -1;
	}

	if(token != FLECHE) {
		cerr << "Parser: expected '->'." << endl;
		return -1;
	}

	if(k=parseLocals()) return k;

	if(!getToken()) {
		cerr << "Parser: expected keyword 'in', found EOF" << endl;
		return -1;
	}

	if(token != IN) {
		cerr << "Parser: expected keyword 'in'." << endl;
		return -1;
	}

	if(k=parseExpression()) return k;

	program->addExpressionLet();

	//remove scope
	program->removeScope();

	return 0;
}

int Parser::parseLocals(void) {
	int k;

	if(!getToken()) {
		cerr << "Parser: term expected, found EOF" << endl;
		return -1;
	} else if(token == '['  ) {
		int n = 0;
		while(1) {
			if(!getToken()) {
				cerr << "Parser: expected ']', found EOF" << endl;
				return -1;
			}

			if(token == ']') {
				program->addExpressionTuple(n);
				return 0;
			}

			else if (token == '_') {
				//Why check this case here? 
				//when this is done in the outter one
				program->addExpressionUndef();
				n++;
			}

			else {
				giveBack();
				if(k=parseLocals()) return k;
				n++;
			}
		}
	} else if(token == '(') {
		//We should never get here
		//but the expression is defined in the gramar
		
		//List type
		while(1) {
			if(!getToken()) return parseLocals();

			if(token == '_') {
				if(getToken() && token == ')') {
					//add parenth expr?
					return 0;
				}
				giveBack();
			} else if(token == ID) {

			}
		}
	} else if(token == '_') {
		program->addExpressionUndef();	
		return 0;
	} else if(token == ID) {
		program->addLocalVariable(this->sVal);
		program->addExpressionChar(this->sVal);
		return 0;
	}	
}

int Parser::parseCall(void) {
	int k;

	if(k=parseExpression()) return k;

	if(getToken() && token == '[') {
		int nval=0;

		while(1) {
			if(!getToken()) {
				cerr << "Parser: expected ']', found eof" << endl;
				return -1;
			}

			if(token == ']') {
				program->addExpressionTuple(nval);
				program->addExpressionCall();
				return 0;
			}
			giveBack();
			if(k=parseExpression()) return k;
			nval++;
		}
	} else {
		giveBack();
		if(k=parseExpression()) return k;
		//TODO things?
		program->addExpressionCall();
		return 0;
	}


}

int Parser::parsePntFun(void) {
	int k;
	int val;
	int ref;

	DEBUG();

	if(!getToken()) {
		cerr << "Parser: function name expected, found EOF" << endl;
		return -1;
	}

	if(token == ID) {
		//TODO Check if ID exists and create a pointer
		program->addExpressionChar(this->sVal);
		
		//cerr << "Parser: unknown label" << endl;
		//return -1;
	}


	program->addExpressionPointer();

	return 0;
}

int Parser::parseSet(void) {
	int k;
	int nbExprs = 0;
	int val;
	int ref;
	int opstore=-1;

	if(!getToken()) {
		cerr << "Parser: reference expected, found eof" << endl;
		return -1;
	}

	if(token != VAR_ID) {
		cerr << "Parser: reference expected." << endl;
		return -1;
	}

	program->addExpressionChar(this->sVal);

	if(!(program->isVariable(this->sVal))) {
		cerr << this->sVal << " unknown reference" << endl;
		return -1;
	}

	if(k=parseSetPoint(&nbExprs)) return k;

	if(!getToken()) {
		cerr << "Parser: expected '=', found eof" << endl;
		return -1;
	}

	if(token != '=') {
		cerr << "Parser: expected '='" << endl;
		return -1;
	}

	if(k=parseExpression()) return k;
	program->addExpressionSet(nbExprs);

	return 0;
}

int Parser::parseSetPoint(int *nbExprs) {
	int k;

	if(!getToken()) return 0;

	if(token != '.') {
		giveBack();
		return 0;
	}

	while(1) {
		if(!getToken()) {
			cerr << "Parser: expression, or field name expected, found EOF" << endl;
			return -1;
		}

		if(token == ID) {
			program->addExpressionChar(this->sVal);
			if(!(program->isVariable(this->sVal))) {
				cerr << this->sVal << " unknown reference" << endl;
				return -1;
			}
			(*nbExprs)++;
		} else {
			giveBack();
			if(k=parseTerm()) return k;
			(*nbExprs)++;
		}

		if(!getToken()) return 0;

		if(token != '.') {
			giveBack();
			return 0;
		}
	}
}

int Parser::parseType(void) {
	string name;

	if(!getToken()) {
		cerr << "Parser:  expected type name, found  EOF" << endl;
		return -1;
	}

	if(token != TYPE_ID) {
		cerr << "Parser: expected type name" << endl;
		return -1;
	}

	name = this->sVal;
	
	int narg=0;

	/*if(!getToken()) {
		//we dont know what this actually is.
		//syntax: type typeID(ID ID...) = ... 
		if(token != '(') giveBack();
		else {
			do {
				if(!getToken()) {
					cerr << "Parser: param or ')' expected, found EOF" << endl;
					return -1;
				}
				if(token == ID) {
					program->addExpressionChar(this->sVal);
				} else if(token != ')') {
					cerr << "Parser expected ')'." << endl;
					return -1;
				}
			} while(token != ')');
		}
	} */

	if(!getToken()) {
		cerr << "Parser: '=' or ';;' expected, found EOF" << endl;
		return -1;
	}

	if(token == '=') {
		if(!getToken()) {
			cerr << "Parser: uncomplete type definition" << endl;
			return -1;
		}

		if(token == '[') return parseStruct(name);
		giveBack();
		return parseSum(name);

	} else if(token == DOUBLE_POINT_VIRGULE) {
		cout << "Parser: incomplete type definition: " << name;
		return 0;
	}
	
	return -1;
}

int Parser::parseStruct(string name) {
	int k;
	int nbExpr=0;
	DEBUG();
	int loop=1;
	do {
		if(!getToken()) {
			cerr << "Parser: field name or ']' expected, found EOF" << endl;
			return -1;
		}
		if(token == ID) {
			program->addGlobalVariable(this->sVal);
			program->addStructureField(this->sVal);
			program->addExpressionChar(this->sVal);
			nbExpr++;
		}
		else if(token == ']') loop =0;
		else { cerr << "Parser: field name or  ']' expected" << endl; return -1; }
	} while(loop);

	if(!getToken()) {
		cerr << "Parser: ';;' expected, found EOF" << endl;
		return -1;
	}

	if(token != DOUBLE_POINT_VIRGULE) {
		cerr << "Parser: ';;' expected" << endl;
		return -1;
	}

	program->addExpressionStruct(nbExpr);
	program->addDeclarationType(name);
	return 0;
}

int Parser::parseSum(string name) {
	int k;
	int nbExpr=0;
	int nbSumEl=0;

	int loop=1;

	do {
		if(!getToken()) {
			cerr << "Parser: constructor expected, found EOF" << endl;
			return -1;
		}
		if(token == ID) {
			program->addExpressionChar(this->sVal);
			//Add to global variable table
			//program->addGlobalVariable(this->sVal);
			nbSumEl++;
		} else {
			cerr << "Parser: constructor expected" << endl;
			return -1;
		}

		if(getToken()) {
			if(token == ID) {
				//This ID is supposed to be a type
				//We should check if its a basic type
				//or one defined else compilation error
				program->addExpressionChar(this->sVal);
				nbSumEl++;
			} else if(token == '_') {
				program->addExpressionUndef();
				nbSumEl++;
			} else {
				giveBack();
			}
			
			/*if(token != '_') {
				giveBack();
			} else {
				program->addExpressionUndef();
				nbSumEl++;
			}*/
		}
		

		if(!getToken()) {
			cerr << "Parser expected '|' or ';;', found EOF" << endl;
			return -1;
		}
		
		if(token == '|') {
			program->addExpressionSumElement(nbSumEl);
			nbSumEl=0;
			nbExpr++;
		} else if(token == DOUBLE_POINT_VIRGULE) {
			program->addExpressionSumElement(nbSumEl);
			nbExpr++;
			nbSumEl=0;
			loop = 0;
		} else {
			cerr << "Parser: expected ';;' or '|'" << endl;
			return -1;
		}
	} while(loop);

	program->addExpressionSum(nbExpr);
	program->addDeclarationType(name);
	return 0;
}

int Parser::parseRef(void) {
	int k;

	int val;
	int ref;

	if(program->isVariable(this->sVal)) {
		return parseGetPoint();
	}
	//sth about sum constructors.. has to go here
	//But what? 
	//Check if ID is a sum constructor with/without arg, and parse
	int tmp = program->isSumTypeField(this->sVal);	
	if(tmp == 0) {
		return 0;
	} else if(tmp == 1) {
		if(k=parseExpression()) return k;
		program->addExpressionCons();
		return 0;
	}
	
	//Handle function call
	int functionNbArgs = program->getFunctionNbArgs(this->sVal);

	if(functionNbArgs >= 0) {
		int i=0;
		for(i=0; i<functionNbArgs; i++) {
			if(k=parseExpression()) {
				cerr << "Parser: function " << this->sVal << " requires " << functionNbArgs << " arguments" << endl;
				return k;
			}
		}
		// at this point we have parsed all args
		program->addExpressionFunCall(functionNbArgs);
		return 0;
	} 

	cerr << "Unknown reference: " << this->sVal << endl;
	return -1;
}

int Parser::parseGetPoint(void) {
	int k;
	int nbExpr=0;

	while(1) {
		if(!getToken()) return 0;

		if(token != '.') {
			giveBack();
			if(nbExpr > 0) {
				program->addExpressionDot(nbExpr);
			}
			return 0;
		}

		//at this point the expression is: ID.

		if(!getToken()) {
			cerr << "Parser: expression or field name expected, found EOF" << endl;
			return -1;
		}

		if(token == ID && 
			program->isGlobalVariable(this->sVal) && 
			program->isStructureField(this->sVal)) {

			program->addExpressionChar(this->sVal);
			nbExpr++;
		} else {
			giveBack();
			if(k=parseTerm()) return k;
			nbExpr++;

		}
	}

}

int Parser::parseFields() {
	int k;
	int loop=1;
	int nbExpr=0;

	while(loop) {
		if(!getToken()) {
			cerr << "Parser: expected ':', found EOF" << endl;
			return -1;
		}
		if(token != ':') {
			cerr << "Parser: expected ':'" << endl;
			return -1;
		}

		if(k=parseExpression()) return k;

		nbExpr++;

		if(!getToken()) {
			cerr << "Parser: ']' expected, found EOF" << endl;
			return -1;
		}
		if(token == ']') {
			program->addExpressionStructCons(nbExpr);
			return 0;
		}
		loop = 0;
		if(token == ID) {
			if(program->isGlobalVariable(this->sVal) &&
				program->isStructureField(this->sVal)) {
				program->addExpressionChar(this->sVal);
				loop=1;
			}
		}

	}
	cerr << "Parser: ']' expected, found EOF" << endl;
	return -1;
}
