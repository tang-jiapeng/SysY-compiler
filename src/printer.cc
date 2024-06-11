#include "printer.h"

#include <string>

#include "ast.h"

// ref:
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template <typename... Args>
std::string strfmt(const std::string &format, Args... args) {
  int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) +
               1;  // Extra space for '\0'
  if (size_s <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  auto size = static_cast<size_t>(size_s);
  std::unique_ptr<char[]> buf(new char[size]);
  std::snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1);  // We don't want the '\0' inside
}

std::string Printer::visit(CompUnitAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "CompUnit:\n";
  depth += 2;
  auto *vec = ast.defs.get();
  for (auto &i : *vec) {
    ans += visit(*(dynamic_cast<DefAST *>(i.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(DefAST &ast) {
  std::string ans = "";
  if (ast.type == DefAST::DefType::FuncDef) {
    ans += visit(*(dynamic_cast<FuncDefAST *>(ast.exp.get())));
  } else if (ast.type == DefAST::DefType::ConstDecl) {
    ans += visit(*(dynamic_cast<ConstDeclAST *>(ast.exp.get())));
  } else {
    ans += visit(*(dynamic_cast<VarDeclAST *>(ast.exp.get())));
  }
  return ans;
}

std::string Printer::visit(ConstDeclAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "ConstDecl:\n";
  depth += 2;
  ans += visit(*dynamic_cast<BTypeAST *>(ast.btype.get()));
  auto *vec = ast.const_defs.get();
  for (auto &i : *vec) {
    ans += visit(*(dynamic_cast<ConstDefAST *>(i.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(BTypeAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  if (ast.type == "int") {
    ans += "BType:int\n";
  }
  return ans;
}

std::string Printer::visit(FuncTypeAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  if (ast.type == "int") {
    ans += "FuncType:int\n";
  } else if (ast.type == "void") {
    ans += "FuncType:void\n";
  }
  return ans;
}

std::string Printer::visit(ConstDefAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "ConstDef:\n";
  ans += strfmt("%*s", depth, "");
  ans += "Ident:" + ast.ident + "\n";
  auto *vec = ast.indexs.get();
  if (!vec->empty()) {
    ans += strfmt("%*s", depth, "");
    ans += "Arrays:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<ExpAST *>(i.get())));
    }
    depth -= 2;
  }
  if (ast.const_init_val != nullptr) {
    ans += "Const";
    ans += visit(*(dynamic_cast<InitValAST *>(ast.const_init_val.get())));
  }
  return ans;
}

std::string Printer::visit(VarDeclAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "Decl:\n";
  depth += 2;
  ans += visit(*dynamic_cast<BTypeAST *>(ast.btype.get()));
  auto *vec = ast.var_defs.get();
  for (auto &i : *vec) {
    ans += visit(*(dynamic_cast<VarDefAST *>(i.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(VarDefAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "Def:\n";
  ans += strfmt("%*s", depth, "");
  ans += "Ident:" + ast.ident + "\n";
  auto *vec = ast.indexs.get();
  if (!vec->empty()) {
    ans += strfmt("%*s", depth, "");
    ans += "Arrays:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<ExpAST *>(i.get())));
    }
    depth -= 2;
  }
  if (ast.init_val != nullptr) {
    ans += visit(*(dynamic_cast<InitValAST *>(ast.init_val.get())));
  }
  return ans;
}

std::string Printer::visit(InitValAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "InitVal:";
  depth += 2;
  auto *vec = ast.init_vals.get();
  if (ast.exp != nullptr) {
    ans += "\n";
    ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
  } else if (!vec->empty()) {
    ans += "\n";
    ans += strfmt("%*s", depth, "");
    ans += "InitValList:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<InitValAST *>(i.get())));
    }
    depth -= 2;
  } else {
    ans += "{}\n";
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(FuncDefAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "FuncDef:\n";
  depth += 2;
  ans += visit(*(dynamic_cast<FuncTypeAST *>(ast.func_type.get())));
  ans += strfmt("%*s", depth, "");
  ans += "Ident:" + ast.ident + "\n";
  auto *vec = ast.func_fparams.get();
  if (!vec->empty()) {
    ans += strfmt("%*s", depth, "");
    ans += "FuncFParamList:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<FuncFParamAST *>(i.get())));
    }
    depth -= 2;
  }
  ans += visit(*(dynamic_cast<BlockAST *>(ast.block.get())));
  depth -= 2;
  return ans;
}

std::string Printer::visit(FuncFParamAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "FuncFParam:\n";
  depth += 2;
  ans += strfmt("%*s", depth, "");
  ans += visit(*(dynamic_cast<FuncTypeAST *>(ast.btype.get())));
  ans += strfmt("%*s", depth, "");
  ans += "Ident:" + ast.ident + "\n";
  if (ast.type == FuncFParamAST::FuncFparamType::Array) {
    ans += strfmt("%*s", depth, "");
    ans += "Array:[]\n";
  }
  auto *vec = ast.indexs.get();
  if (!vec->empty()) {
    ans += strfmt("%*s", depth, "");
    ans += "Arrays:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<ExpAST *>(i.get())));
    }
    depth -= 2;
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(BlockAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "Block:\n";
  depth += 2;
  auto *vec = ast.block_items.get();
  if (!vec->empty()) {
    ans += strfmt("%*s", depth, "");
    ans += "BlockItemList:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<BlockItemAST *>(i.get())));
    }
    depth -= 2;
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(BlockItemAST &ast) {
  std::string ans = "";
  if (ast.type == BlockItemAST::ItemType::ConstDecl) {
    ans += visit(*(dynamic_cast<ConstDeclAST *>(ast.exp.get())));
  } else if (ast.type == BlockItemAST::ItemType::VarDecl) {
    ans += visit(*(dynamic_cast<VarDeclAST *>(ast.exp.get())));
  } else if (ast.type == BlockItemAST::ItemType::Stmt) {
    ans += visit(*(dynamic_cast<StmtAST *>(ast.exp.get())));
  }
  return ans;
}

std::string Printer::visit(StmtAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "Stmt:";
  switch (ast.type) {
    case StmtAST::StmtType::Empty:
      ans += "Semicolon\n";
      break;
    case StmtAST::StmtType::Assign:
      ans += "\n";
      depth += 2;
      ans += visit(*(dynamic_cast<LValAST *>(ast.exp.get())));
      ans += visit(*(dynamic_cast<ExpAST *>(ast.stmt.get())));
      depth -= 2;
      break;
    case StmtAST::StmtType::Return:
      ans += "Return\n";
      depth += 2;
      ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
      depth -= 2;
      break;
    case StmtAST::StmtType::If:
      ans += "\n";
      depth += 2;
      ans += visit(*dynamic_cast<IfAST *>(ast.exp.get()));
      if (ast.stmt != nullptr) {
        ans += "Else\n";
        ans += visit(*(dynamic_cast<StmtAST *>(ast.stmt.get())));
      }
      depth -= 2;
      break;
    case StmtAST::StmtType::Break:
      ans += "Break\n";
      break;
    case StmtAST::StmtType::Continue:
      ans += "Continue\n";
      break;
    case StmtAST::StmtType::Exp:
      ans += "\n";
      depth += 2;
      ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
      depth -= 2;
      break;
    case StmtAST::StmtType::While:
      ans += "While\n";
      depth += 2;
      ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
      ans += visit(*(dynamic_cast<StmtAST *>(ast.stmt.get())));
      depth -= 2;
      break;
    case StmtAST::StmtType::Block:
      ans += "\n";
      depth += 2;
      ans += visit(*(dynamic_cast<BlockAST *>(ast.exp.get())));
      depth -= 2;
      break;
    default:
      break;
  }
  return ans;
}

std::string Printer::visit(IfAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "If\n";
  depth += 2;
  ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
  depth -= 2;
  return ans;
}

std::string Printer::visit(ExpAST &ast) {
  std::string ans = "";
  ans += visit(*(dynamic_cast<LOrExpAST *>(ast.or_exp.get())));
  return ans;
}

std::string Printer::visit(LValAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "LVal:\n";
  depth += 2;
  ans += strfmt("%*s", depth, "");
  ans += "Ident:" + ast.ident + "\n";
  auto *vec = ast.indexs.get();
  if (!vec->empty()) {
    ans += "Arrays:\n";
    depth += 2;
    for (auto &i : *vec) {
      ans += visit(*(dynamic_cast<ExpAST *>(i.get())));
    }
    depth -= 2;
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(PrimaryExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "PrimaryExp:\n";
  depth += 2;
  if (ast.type == PrimaryExpAST::PrimaryExpType::Exp) {
    ans += visit(*(dynamic_cast<ExpAST *>(ast.exp.get())));
  } else if (ast.type == PrimaryExpAST::PrimaryExpType::Number) {
    ans += visit(*(dynamic_cast<NumberAST *>(ast.exp.get())));
  } else if (ast.type == PrimaryExpAST::PrimaryExpType::LVal) {
    ans += visit(*(dynamic_cast<LValAST *>(ast.exp.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(NumberAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "Number:";
  ans += " IntConst: " + std::to_string(ast.val) + "\n";
  return ans;
}

std::string Printer::visit(UnaryExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "UnaryExp:\n";
  depth += 2;
  if (ast.type == UnaryExpAST::Exp) {
    ans += visit(*(dynamic_cast<PrimaryExpAST *>(ast.exp.get())));
  } else if (ast.type == UnaryExpAST::Op) {
    ans += strfmt("%*s", depth, "");
    ans += "UnaryOp:";
    if (ast.op == "+") ans += "+\n";
    if (ast.op == "-") ans += "-\n";
    if (ast.op == "!") ans += "!\n";
    ans += visit(*dynamic_cast<UnaryExpAST *>(ast.exp.get()));
  } else if (ast.type == UnaryExpAST::Call) {
    ans += strfmt("%*s", depth, "");
    ans += "Call:\n";
    depth += 2;
    ans += strfmt("%*s", depth, "");
    ans += "Ident:" + ast.op + "\n";
    auto *vec = ast.func_rparams.get();
    if (!vec->empty()) {
      ans += strfmt("%*s", depth, "");
      ans += "FuncRParamList:" + std::to_string(vec->size()) + "\n";
      depth += 2;
      for (auto &i : *vec) {
        ans += visit(*(dynamic_cast<ExpAST *>(i.get())));
      }
      depth -= 2;
    }
    depth -= 2;
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(MulExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "MulExp:\n";
  depth += 2;
  if (ast.type == MulExpAST::Exp) {
    ans += visit(*(dynamic_cast<UnaryExpAST *>(ast.unary_exp.get())));
  } else if (ast.type == MulExpAST::Op) {
    ans += visit(*(dynamic_cast<MulExpAST *>(ast.mul_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "MulOP:";
    if (ast.op == "*") ans += "*\n";
    if (ast.op == "/") ans += "/\n";
    if (ast.op == "%") ans += "%\n";
    ans += visit(*dynamic_cast<UnaryExpAST *>(ast.unary_exp.get()));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(AddExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "AddExp:\n";
  depth += 2;
  if (ast.type == AddExpAST::Exp) {
    ans += visit(*(dynamic_cast<MulExpAST *>(ast.mul_exp.get())));
  } else if (ast.type == AddExpAST::Op) {
    ans += visit(*(dynamic_cast<AddExpAST *>(ast.add_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "AddOP:";
    if (ast.op == "+") ans += "+\n";
    if (ast.op == "-") ans += "-\n";
    ans += visit(*(dynamic_cast<MulExpAST *>(ast.mul_exp.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(RelExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "RelExp:\n";
  depth += 2;
  if (ast.type == RelExpAST::Exp) {
    ans += visit(*(dynamic_cast<AddExpAST *>(ast.add_exp.get())));
  } else if (ast.type == RelExpAST::Op) {
    ans += visit(*(dynamic_cast<RelExpAST *>(ast.rel_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "RelOP:";
    if (ast.op == ">=") ans += ">=\n";
    if (ast.op == "<=") ans += "<=\n";
    if (ast.op == ">") ans += ">\n";
    if (ast.op == "<") ans += "<\n";
    ans += visit(*(dynamic_cast<AddExpAST *>(ast.add_exp.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(EqExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "EqExp:\n";
  depth += 2;
  if (ast.type == EqExpAST::Exp) {
    ans += visit(*(dynamic_cast<RelExpAST *>(ast.rel_exp.get())));
  } else if (ast.type == EqExpAST::Op) {
    ans += visit(*(dynamic_cast<EqExpAST *>(ast.eq_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "EqOP:";
    if (ast.op == "==") ans += "==\n";
    if (ast.op == "!=") ans += "!=\n";
    ans += visit(*(dynamic_cast<RelExpAST *>(ast.rel_exp.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(LAndExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "LAndExp:\n";
  depth += 2;
  if (ast.type == LAndExpAST::Exp) {
    ans += visit(*(dynamic_cast<EqExpAST *>(ast.eq_exp.get())));
  } else if (ast.type == LAndExpAST::Op) {
    ans += visit(*(dynamic_cast<LAndExpAST *>(ast.and_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "AND_OP:&&";
    ans += visit(*(dynamic_cast<EqExpAST *>(ast.eq_exp.get())));
  }
  depth -= 2;
  return ans;
}

std::string Printer::visit(LOrExpAST &ast) {
  std::string ans = strfmt("%*s", depth, "");
  ans += "LOrExp:\n";
  depth += 2;
  if (ast.type == LOrExpAST::Exp) {
    ans += visit(*(dynamic_cast<LAndExpAST *>(ast.and_exp.get())));
  } else if (ast.type == LOrExpAST::Op) {
    ans += visit(*(dynamic_cast<LOrExpAST *>(ast.or_exp.get())));
    ans += strfmt("%*s", depth, "");
    ans += "OR_OP:||";
    ans += visit(*(dynamic_cast<LAndExpAST *>(ast.and_exp.get())));
  }
  depth -= 2;
  return ans;
}