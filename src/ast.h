#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utils.h"

class BaseAST;

class CompUnitAST;
class DeclDefAST;
class DeclAST;
class DefListAST;
class DefAST;
class ArraysAST;
class InitValListAST;
class InitValAST;
class FuncDefAST;
class FuncFParamListAST;
class FuncFParamAST;
class BlockAST;
class BlockItemListAST;
class BlockItemAST;
class StmtAST;
class ReturnStmtAST;
class SelectStmtAST;
class IterationStmtAST;
class LValAST;
class PrimaryExpAST;
class NumberAST;
class UnaryExpAST;
class CallAST;
class FuncCParamListAST;
class MulExpAST;
class AddExpAST;
class RelExpAST;
class EqExpAST;
class LAndExpAST;
class LOrExpAST;

class Visitor;

class BaseAST {
 public:
  virtual void accept(Visitor &visitor) = 0;
  BaseAST() = default;
  virtual ~BaseAST() = default;
};

class CompUnitAST : public BaseAST {
 public:
  std::vector<std::unique_ptr<DeclDefAST>> declDefList;
  void accept(Visitor &visitor) override;
};

class DeclDefAST : public BaseAST {
 public:
  std::unique_ptr<DeclAST> Decl = nullptr;
  std::unique_ptr<FuncDefAST> funcDef = nullptr;
  void accept(Visitor &visitor) override;
};

class DeclAST : public BaseAST {
 public:
  TYPE bType;
  bool isConst;
  std::vector<std::unique_ptr<DefAST>> defList;
  void accept(Visitor &visitor) override;
};

class DefListAST {
 public:
  std::vector<std::unique_ptr<DefAST>> list;
};

class DefAST : public BaseAST {
 public:
  std::unique_ptr<std::string> id;
  std::vector<std::unique_ptr<AddExpAST>> arrays;
  std::unique_ptr<InitValAST> initVal;
  void accept(Visitor &visitor) override;
};

class ArraysAST {
 public:
  std::vector<std::unique_ptr<AddExpAST>> list;
};

class InitValAST : public BaseAST {
 public:
  std::unique_ptr<AddExpAST> exp;
  std::vector<std::unique_ptr<InitValAST>> initValList;
  void accept(Visitor &visitor) override;
};

class InitValListAST {
 public:
  std::vector<std::unique_ptr<InitValAST>> list;
};

class FuncDefAST : public BaseAST {
 public:
  TYPE funcType;
  std::unique_ptr<std::string> id;
  std::vector<std::unique_ptr<FuncFParamAST>> funcFParamList;
  std::unique_ptr<BlockAST> block = nullptr;
  void accept(Visitor &visitor) override;
};

class FuncFParamListAST {
 public:
  std::vector<std::unique_ptr<FuncFParamAST>> list;
};

class FuncFParamAST : public BaseAST {
 public:
  TYPE bType;
  std::unique_ptr<std::string> id;
  bool isArray =
      false;  // 用于区分是否是数组参数，此时一维数组和多维数组expArrays都是empty
  std::vector<std::unique_ptr<AddExpAST>> arrays;
  void accept(Visitor &visitor) override;
};

class BlockAST : public BaseAST {
 public:
  std::vector<std::unique_ptr<BlockItemAST>> blockItemList;
  void accept(Visitor &visitor) override;
};

class BlockItemListAST {
 public:
  std::vector<std::unique_ptr<BlockItemAST>> list;
};

class BlockItemAST : public BaseAST {
 public:
  std::unique_ptr<DeclAST> decl = nullptr;
  std::unique_ptr<StmtAST> stmt = nullptr;
  void accept(Visitor &visitor) override;
};

class StmtAST : public BaseAST {
 public:
  STYPE sType;
  std::unique_ptr<LValAST> lVal = nullptr;
  std::unique_ptr<AddExpAST> exp = nullptr;
  std::unique_ptr<ReturnStmtAST> returnStmt = nullptr;
  std::unique_ptr<SelectStmtAST> selectStmt = nullptr;
  std::unique_ptr<IterationStmtAST> iterationStmt = nullptr;
  std::unique_ptr<BlockAST> block = nullptr;
  void accept(Visitor &visitor) override;
};

class ReturnStmtAST : public BaseAST {
 public:
  std::unique_ptr<AddExpAST> exp = nullptr;
  void accept(Visitor &visitor) override;
};

class SelectStmtAST : public BaseAST {
 public:
  std::unique_ptr<LOrExpAST> cond;
  std::unique_ptr<StmtAST> ifStmt, elseStmt;
  void accept(Visitor &visitor) override;
};

class IterationStmtAST : public BaseAST {
 public:
  std::unique_ptr<LOrExpAST> cond;
  std::unique_ptr<StmtAST> stmt;
  void accept(Visitor &visitor) override;
};

class AddExpAST : public BaseAST {
 public:
  std::unique_ptr<AddExpAST> addExp;
  std::unique_ptr<MulExpAST> mulExp;
  AOP op;
  void accept(Visitor &visitor) override;
};

class MulExpAST : public BaseAST {
 public:
  std::unique_ptr<UnaryExpAST> unaryExp;
  std::unique_ptr<MulExpAST> mulExp;
  MOP op;
  void accept(Visitor &visitor) override;
};

class UnaryExpAST : public BaseAST {
 public:
  std::unique_ptr<PrimaryExpAST> primaryExp;
  std::unique_ptr<CallAST> call;
  std::unique_ptr<UnaryExpAST> unaryExp;
  UOP op;
  void accept(Visitor &visitor) override;
};

class PrimaryExpAST : public BaseAST {
 public:
  std::unique_ptr<AddExpAST> exp;
  std::unique_ptr<LValAST> lval;
  std::unique_ptr<NumberAST> number;
  void accept(Visitor &visitor) override;
};

class NumberAST : public BaseAST {
 public:
  bool isInt;
  union {
    int intval;
    float floatval;
  };
  void accept(Visitor &visitor) override;
};

class LValAST : public BaseAST {
 public:
  std::unique_ptr<std::string> id;
  std::vector<std::unique_ptr<AddExpAST>> arrays;
  void accept(Visitor &visitor) override;
};

class CallAST : public BaseAST {
 public:
  std::unique_ptr<std::string> id;
  std::vector<std::unique_ptr<AddExpAST>> funcCParamList;
  void accept(Visitor &visitor) override;
};

class FuncCParamListAST {
 public:
  std::vector<std::unique_ptr<AddExpAST>> list;
};

class RelExpAST : public BaseAST {
 public:
  std::unique_ptr<AddExpAST> addExp;
  std::unique_ptr<RelExpAST> relExp;
  ROP op;
  void accept(Visitor &visitor) override;
};

class EqExpAST : public BaseAST {
 public:
  std::unique_ptr<RelExpAST> relExp;
  std::unique_ptr<EqExpAST> eqExp;
  EOP op;
  void accept(Visitor &visitor) override;
};

class LAndExpAST : public BaseAST {
 public:
  // lAndExp不为空则说明有and符号，or类似
  std::unique_ptr<EqExpAST> eqExp;
  std::unique_ptr<LAndExpAST> lAndExp;
  void accept(Visitor &visitor) override;
};

class LOrExpAST : public BaseAST {
 public:
  std::unique_ptr<LOrExpAST> lOrExp;
  std::unique_ptr<LAndExpAST> lAndExp;
  void accept(Visitor &visitor) override;
};

class Visitor {
 public:
  virtual void visit(CompUnitAST &ast) = 0;
  virtual void visit(DeclDefAST &ast) = 0;
  virtual void visit(DeclAST &ast) = 0;
  virtual void visit(DefAST &ast) = 0;
  virtual void visit(InitValAST &ast) = 0;
  virtual void visit(FuncDefAST &ast) = 0;
  virtual void visit(FuncFParamAST &ast) = 0;
  virtual void visit(BlockAST &ast) = 0;
  virtual void visit(BlockItemAST &ast) = 0;
  virtual void visit(StmtAST &ast) = 0;
  virtual void visit(ReturnStmtAST &ast) = 0;
  virtual void visit(SelectStmtAST &ast) = 0;
  virtual void visit(IterationStmtAST &ast) = 0;
  virtual void visit(AddExpAST &ast) = 0;
  virtual void visit(MulExpAST &ast) = 0;
  virtual void visit(UnaryExpAST &ast) = 0;
  virtual void visit(PrimaryExpAST &ast) = 0;
  virtual void visit(LValAST &ast) = 0;
  virtual void visit(NumberAST &ast) = 0;
  virtual void visit(CallAST &ast) = 0;
  virtual void visit(RelExpAST &ast) = 0;
  virtual void visit(EqExpAST &ast) = 0;
  virtual void visit(LAndExpAST &ast) = 0;
  virtual void visit(LOrExpAST &ast) = 0;
};



