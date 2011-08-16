LDFLAGS=-ll -ly -lm
CFLAGS=
EXEC=metalp

all: parser comp 

comp: metalp.lex.yy.o \
      metalp.o  \
      Declaration.o \
      DeclarationConstant.o \
      DeclarationFunction.o \
      DeclarationProto.o \
      DeclarationVariable.o  \
      DeclarationVMFunction.o \
      DeclarationType.o \
      Expression.o \
      ExpressionArray.o \
      ExpressionBinaryOp.o \
      ExpressionCall.o \
      ExpressionChar.o \
      ExpressionCons.o \
      ExpressionDot.o \
      ExpressionFor.o \
      ExpressionFun.o \
      ExpressionFunCall.o \
      ExpressionIf.o \
      ExpressionInt.o \
      ExpressionLet.o \
      ExpressionList.o \
      ExpressionMatch.o \
      ExpressionMatchElement.o \
      ExpressionNil.o \
      ExpressionParenth.o \
      ExpressionPointer.o \
      ExpressionSet.o \
      ExpressionString.o \
      ExpressionTuple.o \
      ExpressionStruct.o \
      ExpressionStructCons.o \
      ExpressionSum.o \
      ExpressionSumElement.o \
      ExpressionUnaryOp.o \
      ExpressionUndef.o \
      ExpressionVal.o \
      ExpressionWhile.o \
      Parser.o \
      Program.o \
      Type.o \
      TypeArray.o \
      TypeInt.o \
      TypeString.o \
      TypeTable.o \
      VariableTable.o \
      VMFunctions.o
	g++ -o $(EXEC) $^ $(LDFLAGS)

parser:
	@echo "Generating Lexer.."
	@flex -B --yylineno -o metalp.lex.yy.c metalp.l
	@echo "Done."

metalp.lex.yy.o:
	@g++ $(CFLAGS) -c metalp.lex.yy.c -ll -ly -lm 


%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $< 

clean: cleanmetalp 
	@echo "Done!"

cleanmetalp:
	@rm *.o metalp.lex.yy.c metalp

ctags: 
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .

.PHONY: parser clean
