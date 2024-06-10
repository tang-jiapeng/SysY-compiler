#pragma once

#include "ast.h"

class Printer {
 public:
  int depth = 0;
  std::string visit(CompUnitAST& ast);
  std::string visit(DefAST& ast);
  std::string visit(ConstDeclAST& ast);
  std::string visit(TypeAST& ast);
  std::string visit(ConstDefAST& ast);
  std::string visit(VarDeclAST& ast);
  std::string visit(VarDefAST& ast);
  std::string visit(InitValAST& ast);
  std::string visit(FuncDefAST& ast);
  std::string visit(FuncFParamAST& ast);
  std::string visit(BlockAST& ast);
  std::string visit(StmtAST& ast);
  std::string visit(IfAST& ast);
  std::string visit(ExpAST& ast);
  std::string visit(LValAST& ast);
  std::string visit(PrimaryExpAST& ast);
  std::string visit(NumberAST& ast);
  std::string visit(UnaryExpAST& ast);
  std::string visit(MulExpAST& ast);
  std::string visit(AddExpAST& ast);
  std::string visit(RelExpAST& ast);
  std::string visit(EqExpAST& ast);
  std::string visit(LAndExpAST& ast);
  std::string visit(LOrExpAST& ast);
};