#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Visitor;

class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void accept(Visitor& visitor) = 0;
};

class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> defs;

  CompUnitAST(std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& defs)
      : defs(std::move(defs)) {}

  void accept(Visitor& visitor) override;
};

class DefAST : public BaseAST {
 public:
  enum DefType { FuncDef, ConstDecl, VarDecl };

  std::unique_ptr<BaseAST> exp;
  DefType type;

  DefAST(std::unique_ptr<BaseAST>& exp, DefType type) : exp(std::move(exp)) {
    this->type = type;
  }

  void accept(Visitor& visitor) override;
};

class ConstDeclAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> btype;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> const_defs;

  ConstDeclAST(
      std::unique_ptr<BaseAST>& btype,
      std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& const_defs)
      : btype(std::move(btype)), const_defs(std::move(const_defs)) {}

  void accept(Visitor& visitor) override;
};

class TypeAST : public BaseAST {
 public:
  std::string type;

  TypeAST(const char* type) : type(type) {}

  void accept(Visitor& visitor) override;
};

class ConstDefAST : public BaseAST {
 public:
  std::string ident;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> indexs;
  std::unique_ptr<BaseAST> const_init_val;

  ConstDefAST(const char* ident, std::unique_ptr<BaseAST>& const_init_val)
      : ident(ident), const_init_val(std::move(const_init_val)) {}

  ConstDefAST(const char* ident,
              std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& indexs,
              std::unique_ptr<BaseAST>& const_init_val)
      : ident(ident),
        indexs(std::move(indexs)),
        const_init_val(std::move(const_init_val)) {}

  void accept(Visitor& visitor) override;
};

class VarDeclAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> btype;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> var_defs;

  VarDeclAST(std::unique_ptr<BaseAST>& btype,
             std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& var_defs)
      : btype(std::move(btype)), var_defs(std::move(var_defs)) {}

  void accept(Visitor& visitor) override;
};

class VarDefAST : public BaseAST {
 public:
  enum VarDefType { Exp, Array };

  std::string ident;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> indexs;
  std::unique_ptr<BaseAST> init_val;
  VarDefType type;

  VarDefAST(const char* ident, VarDefType type) : ident(ident) {
    this->type = type;
  }
  VarDefAST(const char* ident, std::unique_ptr<BaseAST>& init_val,
            VarDefType type)
      : ident(ident), init_val(std::move(init_val)) {
    this->type = type;
  }

  VarDefAST(const char* ident,
            std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& indexs,
            VarDefType type)
      : ident(ident), indexs(std::move(indexs)) {
    this->type = type;
  }

  VarDefAST(const char* ident,
            std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& indexs,
            std::unique_ptr<BaseAST>& init_val, VarDefType type)
      : ident(ident), indexs(std::move(indexs)), init_val(std::move(init_val)) {
    this->type = type;
  }

  void accept(Visitor& visitor) override;
};

class InitValAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> init_vals;

  InitValAST() = default;
  InitValAST(std::unique_ptr<BaseAST>& exp) : exp(std::move(exp)) {}
  InitValAST(std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& init_vals)
      : init_vals(std::move(init_vals)) {}

  void accept(Visitor& visitor) override;
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> func_fparams;
  std::unique_ptr<BaseAST> block;

  FuncDefAST(
      std::unique_ptr<BaseAST>& func_type, const char* ident,
      std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& func_fparams,
      std::unique_ptr<BaseAST>& block)
      : func_type(std::move(func_type)),
        ident(ident),
        func_fparams(std::move(func_fparams)),
        block(std::move(block)) {}

  void accept(Visitor& visitor) override;
};

class FuncFParamAST : public BaseAST {
 public:
  enum FuncFparamType { Var, Array };

  std::unique_ptr<BaseAST> btype;
  std::string ident;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> indexs;
  FuncFparamType type;

  FuncFParamAST(std::unique_ptr<BaseAST>& btype, const char* ident,
                FuncFparamType type)
      : btype(std::move(btype)), ident(ident) {
    this->type = type;
  }

  FuncFParamAST(std::unique_ptr<BaseAST>& btype, const char* ident,
                std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& indexs,
                FuncFparamType type)
      : btype(std::move(btype)), ident(ident), indexs(std::move(indexs)) {
    this->type = type;
  }

  void accept(Visitor& visitor) override;
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> block_items;

  BlockAST(std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& block_items)
      : block_items(std::move(block_items)) {}

  void accept(Visitor& visitor) override;
};

class StmtAST : public BaseAST {
 public:
  enum StmtType {
    Exp,
    Assign,
    Return,
    Empty,
    If,
    Block,
    While,
    Break,
    Continue
  };

  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> stmt;
  StmtType type;

  StmtAST(StmtType type) { this->type = type; };
  StmtAST(std::unique_ptr<BaseAST>& exp, StmtType type) : exp(std::move(exp)) {
    this->type = type;
  }
  StmtAST(std::unique_ptr<BaseAST>& exp, std::unique_ptr<BaseAST>& stmt,
          StmtType type)
      : exp(std::move(exp)), stmt(std::move(stmt)) {
    this->type = type;
  }

  void accept(Visitor& visitor) override;
};

class IfAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;

  IfAST(std::unique_ptr<BaseAST>& exp) : exp(std::move(exp)) {}

  void accept(Visitor& visitor) override;
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> or_exp;

  ExpAST(std::unique_ptr<BaseAST>& or_exp) : or_exp(std::move(or_exp)) {}

  void accept(Visitor& visitor) override;
};

class LValAST : public BaseAST {
 public:
  std::string ident;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> indexs;

  LValAST(const char* ident) : ident(ident) {}
  LValAST(const char* ident,
          std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& indexs)
      : ident(ident), indexs(std::move(indexs)) {}

  void accept(Visitor& visitor) override;
};

class PrimaryExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;

  PrimaryExpAST(std::unique_ptr<BaseAST>& exp) : exp(std::move(exp)) {}

  void accept(Visitor& visitor) override;
};

class NumberAST : public BaseAST {
 public:
  int val;

  NumberAST(int val) : val(val) {}

  void accept(Visitor& visitor) override;
};

class UnaryExpAST : public BaseAST {
 public:
  enum { Exp, Op, Call } type;

  std::string op;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> func_rparams;

  UnaryExpAST(std::unique_ptr<BaseAST>& exp) : exp(std::move(exp)) {
    type = Exp;
  }
  UnaryExpAST(const char* op, std::unique_ptr<BaseAST>& exp)
      : op(op), exp(std::move(exp)) {
    type = Op;
  }
  UnaryExpAST(
      const char* op,
      std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>& func_rparams)
      : op(op), func_rparams(std::move(func_rparams)) {
    type = Call;
  }

  void accept(Visitor& visitor) override;
};

class MulExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;
  std::unique_ptr<BaseAST> mul_exp;
  std::string op;
  std::unique_ptr<BaseAST> unary_exp;

  MulExpAST(std::unique_ptr<BaseAST>& unary_exp)
      : unary_exp(std::move(unary_exp)) {
    type = Exp;
  }
  MulExpAST(std::unique_ptr<BaseAST>& mul_exp, const char* op,
            std::unique_ptr<BaseAST>& unary_exp)
      : mul_exp(std::move(mul_exp)), op(op), unary_exp(std::move(unary_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class AddExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;
  std::unique_ptr<BaseAST> add_exp;
  std::string op;
  std::unique_ptr<BaseAST> mul_exp;

  AddExpAST(std::unique_ptr<BaseAST>& mul_exp) : mul_exp(std::move(mul_exp)) {
    type = Exp;
  }
  AddExpAST(std::unique_ptr<BaseAST>& add_exp, const char* op,
            std::unique_ptr<BaseAST>& mul_exp)
      : add_exp(std::move(add_exp)), op(op), mul_exp(std::move(mul_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class RelExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;

  std::unique_ptr<BaseAST> rel_exp;
  std::string op;
  std::unique_ptr<BaseAST> add_exp;

  RelExpAST(std::unique_ptr<BaseAST>& add_exp) : add_exp(std::move(add_exp)) {
    type = Exp;
  }
  RelExpAST(std::unique_ptr<BaseAST>& rel_exp, const char* op,
            std::unique_ptr<BaseAST>& add_exp)
      : rel_exp(std::move(rel_exp)), op(op), add_exp(std::move(add_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class EqExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;
  std::unique_ptr<BaseAST> eq_exp;
  std::string op;
  std::unique_ptr<BaseAST> rel_exp;

  EqExpAST(std::unique_ptr<BaseAST>& rel_exp) : rel_exp(std::move(rel_exp)) {
    type = Exp;
  }
  EqExpAST(std::unique_ptr<BaseAST>& eq_exp, const char* op,
           std::unique_ptr<BaseAST>& rel_exp)
      : eq_exp(std::move(eq_exp)), op(op), rel_exp(std::move(rel_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class LAndExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;
  std::unique_ptr<BaseAST> and_exp;
  std::string op;
  std::unique_ptr<BaseAST> eq_exp;

  LAndExpAST(std::unique_ptr<BaseAST>& eq_exp) : eq_exp(std::move(eq_exp)) {
    type = Exp;
  }
  LAndExpAST(std::unique_ptr<BaseAST>& and_exp, const char* op,
             std::unique_ptr<BaseAST>& eq_exp)
      : and_exp(std::move(and_exp)), op(op), eq_exp(std::move(eq_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class LOrExpAST : public BaseAST {
 public:
  enum { Exp, Op } type;
  std::unique_ptr<BaseAST> or_exp;
  std::string op;
  std::unique_ptr<BaseAST> and_exp;

  LOrExpAST(std::unique_ptr<BaseAST>& and_exp) : and_exp(std::move(and_exp)) {
    type = Exp;
  }
  LOrExpAST(std::unique_ptr<BaseAST>& or_exp, const char* op,
            std::unique_ptr<BaseAST>& and_exp)
      : or_exp(std::move(or_exp)), op(op), and_exp(std::move(and_exp)) {
    type = Op;
  }

  void accept(Visitor& visitor) override;
};

class Visitor {
 public:
  virtual void visit(CompUnitAST& ast) = 0;
  virtual void visit(DefAST& ast) = 0;
  virtual void visit(ConstDeclAST& ast) = 0;
  virtual void visit(TypeAST& ast) = 0;
  virtual void visit(ConstDefAST& ast) = 0;
  virtual void visit(VarDeclAST& ast) = 0;
  virtual void visit(VarDefAST& ast) = 0;
  virtual void visit(InitValAST& ast) = 0;
  virtual void visit(FuncDefAST& ast) = 0;
  virtual void visit(FuncFParamAST& ast) = 0;
  virtual void visit(BlockAST& ast) = 0;
  virtual void visit(StmtAST& ast) = 0;
  virtual void visit(IfAST& ast) = 0;
  virtual void visit(ExpAST& ast) = 0;
  virtual void visit(LValAST& ast) = 0;
  virtual void visit(PrimaryExpAST& ast) = 0;
  virtual void visit(NumberAST& ast) = 0;
  virtual void visit(UnaryExpAST& ast) = 0;
  virtual void visit(MulExpAST& ast) = 0;
  virtual void visit(AddExpAST& ast) = 0;
  virtual void visit(RelExpAST& ast) = 0;
  virtual void visit(EqExpAST& ast) = 0;
  virtual void visit(LAndExpAST& ast) = 0;
  virtual void visit(LOrExpAST& ast) = 0;
};