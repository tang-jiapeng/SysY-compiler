// 这里写一些选项, 可以控制 Flex/Bison 的某些行为

%code requires {
  #include <memory>
  #include <cstring>
  #include "ast.h"
}

%{

// 这里写一些全局的代码
// 因为最后要生成 C/C++ 文件, 实现主要逻辑的部分都是用 C/C++ 写的
// 难免会用到头文件, 所以通常头文件和一些全局声明/定义写在这里

#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include "ast.h"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast,const char *s);

using namespace std;

%}

%parse-param {std::unique_ptr<BaseAST> &ast}


%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
  std::vector<std::unique_ptr<BaseAST>> *ast_vec;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val

%token INT VOID RETURN CONST IF ELSE WHILE BREAK CONTINUE
%token <str_val> IDENT EQOP RELOP AND OR
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> CompUnit FuncDef Block BlockItem Stmt Decl Type If Def
%type <ast_val> Exp PrimaryExp UnaryExp AddExp MulExp RelExp EqExp LAndExp LOrExp Number LVal
%type <ast_val> ConstDecl ConstDef
%type <ast_val> VarDecl VarDef InitVal FuncFParam FuncRParam
%type <ast_vec> BlockArray ConstDefArray VarDefArray DefArray InitValArray
%type <ast_vec> FuncFParamArray FuncRParamArray IndexArray 
%type <str_val> UNARYOP MULOP ADDOP

%%


CompUnit 
  : DefArray {
    auto defs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($1);
    ast = std::unique_ptr<BaseAST>(new CompUnitAST(defs));
  }
  ;

DefArray 
  : Def DefArray {
    auto def = std::unique_ptr<BaseAST>($1);
    auto defs =(std::vector<std::unique_ptr<BaseAST>>*)($2);
    defs->push_back(std::move(def));
    $$ = defs;
  }
  | Def {
    auto def = std::unique_ptr<BaseAST>($1);
    auto defs =new std::vector<std::unique_ptr<BaseAST>>();
    defs->push_back(std::move(def));
    $$ = defs;
  }
  ;

Def
  : FuncDef {
    auto func_def = std::unique_ptr<BaseAST>($1);
    $$ = new DefAST(func_def,DefAST::DefType::FuncDef);
  } 
  | ConstDecl {
    auto const_decl = std::unique_ptr<BaseAST>($1);
    $$ = new DefAST(const_decl,DefAST::DefType::ConstDecl);
  }
  | VarDecl {
    auto var_decl = std::unique_ptr<BaseAST>($1);
    $$ = new DefAST(var_decl, DefAST::DefType::VarDecl);
  }
  ;

Decl : ConstDecl | VarDecl;

ConstDecl 
  : CONST Type ConstDefArray ';' {
    auto btype = std::unique_ptr<BaseAST>($2);
    auto const_defs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($3);
    $$ = new ConstDeclAST(btype,const_defs);
  }
  ;

ConstDefArray 
  : ConstDef ',' ConstDefArray {
    auto const_def = std::unique_ptr<BaseAST>($1);
    auto const_defs = (std::vector<std::unique_ptr<BaseAST>>*)($3);
    const_defs->push_back(std::move(const_def));
    $$ = const_defs;
  }
  | ConstDef {
    auto const_def = std::unique_ptr<BaseAST>($1);
    auto const_defs = new std::vector<std::unique_ptr<BaseAST>>();
    const_defs->push_back(std::move(const_def));
    $$ = const_defs;
  }
  ;

ConstDef
  : IDENT '=' InitVal {
    auto ident = std::unique_ptr<std::string>($1);
    auto const_init_val = std::unique_ptr<BaseAST>($3);
    $$ = new ConstDefAST(ident->c_str() , const_init_val);
  }
  | IDENT IndexArray '=' InitVal {
    auto ident = std::unique_ptr<std::string>($1);
    auto indexs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    auto const_init_val = std::unique_ptr<BaseAST>($4);
    $$ = new ConstDefAST(ident->c_str(),indexs,const_init_val);
  }
  ;

IndexArray 
  : '[' Exp ']' {
    auto exp = std::unique_ptr<BaseAST>($2);
    auto indexs = new std::vector<std::unique_ptr<BaseAST>>();
    indexs->push_back(std::move(exp));
    $$ = indexs;
  }
  | IndexArray '[' Exp ']' {
    auto exp = std::unique_ptr<BaseAST>($3);
    auto indexs = (std::vector<std::unique_ptr<BaseAST>>*)($1);
    indexs->push_back(std::move(exp));
    $$ = indexs;
  } 
  ;

VarDecl 
  : Type VarDefArray {
    auto btype = std::unique_ptr<BaseAST>($1);
    auto var_defs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    $$ = new VarDeclAST(btype,var_defs); 
  }
  ;

VarDefArray 
  : VarDef ',' VarDefArray {
    auto var_def = std::unique_ptr<BaseAST>($1);
    auto var_defs = (std::vector<std::unique_ptr<BaseAST>>*)($3);
    var_defs->push_back(std::move(var_def));
    $$ = var_defs;
  }
  | VarDef {
    auto var_def = std::unique_ptr<BaseAST>($1);
    auto var_defs = new std::vector<std::unique_ptr<BaseAST>>();
    var_defs->push_back(std::move(var_def));
    $$ = var_defs;
  }
  ;

VarDef 
  : IDENT {
    auto ident = std::unique_ptr<std::string>($1);
    $$ = new VarDefAST(ident->c_str(),VarDefAST::VarDefType::Exp);
  }
  | IDENT '=' InitVal {
    auto ident = std::unique_ptr<std::string>($1);
    auto init_val = std::unique_ptr<BaseAST>($3);
    $$ = new VarDefAST(ident->c_str(),init_val,VarDefAST::VarDefType::Exp);
  }
  | IDENT IndexArray {
    auto ident = std::unique_ptr<std::string>($1);
    auto indexs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    $$ = new VarDefAST(ident->c_str(),indexs,VarDefAST::VarDefType::Array);
  }
  | IDENT IndexArray '=' InitVal {
    auto ident = std::unique_ptr<std::string>($1);
    auto indexs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    auto init_val = std::unique_ptr<BaseAST>($4);
    $$ = new VarDefAST(ident->c_str(),indexs,init_val,VarDefAST::VarDefType::Array);
  }
  ;

InitVal 
: Exp {
  auto exp = std::unique_ptr<BaseAST>($1);
  $$ = new InitValAST(exp);
}
| '{' InitValArray '}' {
  auto init_vals = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
  $$ = new InitValAST(init_vals);
}
| '{' '}' {
  $$ = new InitValAST();
}
;

InitValArray
  : InitVal {
    auto init_val = std::unique_ptr<BaseAST>($1);
    auto init_vals = new std::vector<std::unique_ptr<BaseAST>>();
    init_vals->push_back(std::move(init_val));
    $$ = init_vals;
  }
  | InitVal ',' InitValArray {
    auto init_val = std::unique_ptr<BaseAST>($1);
    auto init_vals = (std::vector<std::unique_ptr<BaseAST>>*)($3);
    init_vals->push_back(std::move(init_val));
    $$ = init_vals;
  }
  ;


FuncDef 
  : Type IDENT '(' FuncFParamArray ')' Block {
    auto func_type = std::unique_ptr<BaseAST>($1);
    auto ident = std::unique_ptr<std::string>($2);
    auto func_fparams = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($4);
    auto block = std::unique_ptr<BaseAST>($6);
    $$ = new FuncDefAST(func_type, ident->c_str(), func_fparams, block);
  }
  ;

FuncFParamArray 
  : FuncFParam {
    auto func_fparam = std::unique_ptr<BaseAST>($1);
    auto func_fparams = new std::vector<std::unique_ptr<BaseAST>>();
    func_fparams->push_back(std::move(func_fparam));
    $$ = func_fparams;
  }
  | FuncFParam ',' FuncFParamArray {
    auto func_fparam = std::unique_ptr<BaseAST>($1);
    auto func_fparams = (std::vector<std::unique_ptr<BaseAST>>*)($3);
    func_fparams->push_back(std::move(func_fparam));
    $$ = func_fparams;
  }
  | {
    $$ = new std::vector<std::unique_ptr<BaseAST>> ();
  }
  ;

FuncFParam 
  : Type IDENT {
    auto btype = std::unique_ptr<BaseAST>($1);
    auto ident = std::unique_ptr<std::string>($2);
    $$ = new FuncFParamAST(btype,ident->c_str(),FuncFParamAST::FuncFparamType::Var);
  }
  | Type IDENT '[' ']' {
    auto btype = std::unique_ptr<BaseAST>($1);
    auto ident = std::unique_ptr<std::string>($2);
    $$ = new FuncFParamAST(btype,ident->c_str(),FuncFParamAST::FuncFparamType::Array);
  }
  | Type IDENT '[' ']' IndexArray {
    auto btype = std::unique_ptr<BaseAST>($1);
    auto ident = std::unique_ptr<std::string>($2);
    auto indexs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($5);
     $$ = new FuncFParamAST(btype,ident->c_str(),indexs,FuncFParamAST::FuncFparamType::Array);
  }
  ;

Type 
  : INT {
    $$ = new TypeAST("int");
  }
  | VOID {
    $$ = new TypeAST("void");
  }
  ;

Block
  : '{' BlockArray '}' {
    auto block_items = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    $$ = new BlockAST(block_items);
  }
  ;

BlockArray 
  : BlockItem BlockArray {
    auto block_item = std::unique_ptr<BaseAST>($1);
    auto block_items = (std::vector<std::unique_ptr<BaseAST>> *)($2);
    block_items->push_back(std::move(block_item));
    $$ = block_items;
  }
  | BlockItem {
    auto block_item = std::unique_ptr<BaseAST>($1);
    auto block_items = new std::vector<std::unique_ptr<BaseAST>>();
    block_items->push_back(std::move(block_item));
    $$ = block_items;
  }
  ;  

BlockItem : Decl | Stmt;

Stmt 
  : RETURN Exp ';' {
    auto exp = std::unique_ptr<BaseAST>($2);
    $$ = new StmtAST(exp,StmtAST::StmtType::Return);
  }
  | LVal '=' Exp ';' {
    auto lval = std::unique_ptr<BaseAST>($1);
    auto exp = std::unique_ptr<BaseAST>($3);
    $$ = new StmtAST(lval,exp,StmtAST::StmtType::Assign);
  }
  | Exp ';' {
    auto exp = std::unique_ptr<BaseAST>($1);
    $$ = new StmtAST(exp,StmtAST::StmtType::Exp);
  }
  | ';' {
    $$ = new StmtAST(StmtAST::StmtType::Empty);
  }
  | Block {
    auto block = std::unique_ptr<BaseAST>($1);
    $$ = new StmtAST(block,StmtAST::StmtType::Block);
  }
  | If  {
    auto exp = std::unique_ptr<BaseAST>($1);
    $$ = new StmtAST(exp,StmtAST::StmtType::If);
  }
  | If ELSE Stmt {
    auto exp = std::unique_ptr<BaseAST>($1);
    auto stmt = std::unique_ptr<BaseAST>($3);
    $$ = new StmtAST(exp,stmt,StmtAST::StmtType::If);
  } 
  | WHILE '(' Exp ')' Stmt {
    auto exp = std::unique_ptr<BaseAST>($3);
    auto stmt = std::unique_ptr<BaseAST>($5);
    $$ = new StmtAST(exp,stmt,StmtAST::StmtType::While);
  }
  | BREAK ';' {
    $$ = new StmtAST(StmtAST::StmtType::Break);
  }
  | CONTINUE ';' {
    $$ = new StmtAST(StmtAST::StmtType::Continue);    
  }
  ;

If 
  : IF '(' Exp ')' {
    auto exp = std::unique_ptr<BaseAST>($3);
    $$ = new IfAST(exp);
  }

Exp
  : LOrExp {
    auto lorexp = std::unique_ptr<BaseAST>($1);
    $$ = new ExpAST(lorexp);
  }
  ;

LVal 
  : IDENT {
    auto ident = std::unique_ptr<std::string>($1);
    $$ = new LValAST(ident->c_str());
  }
  | IDENT IndexArray {
    auto ident = std::unique_ptr<std::string>($1);
    auto indexs = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($2);
    $$ = new LValAST(ident->c_str(),indexs);
  }
  ;

PrimaryExp 
  : '(' Exp ')' {
    auto exp = std::unique_ptr<BaseAST>($2);
    $$ = new PrimaryExpAST(exp);
  }
  | Number {
    auto number = std::unique_ptr<BaseAST>($1);
    $$ = new PrimaryExpAST(number);
  }
  | LVal {
    auto lval = std::unique_ptr<BaseAST>($1);
    $$ = new PrimaryExpAST(lval);
  }
  ;


Number  
  : INT_CONST {
    $$ = new NumberAST($1);
  }
  ;

UnaryExp 
  : PrimaryExp {
    auto primaryexp = std::unique_ptr<BaseAST>($1);
    $$ = new UnaryExpAST(primaryexp);
  } 
  | UNARYOP UnaryExp {
    auto unaryop = std::unique_ptr<std::string>($1);
    auto unaryexp = std::unique_ptr<BaseAST>($2);
    $$ = new UnaryExpAST(unaryop->c_str(),unaryexp);
  }
  | IDENT '(' FuncRParamArray ')' {
    auto ident = std::unique_ptr<std::string>($1);
    auto func_rparams = std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>>($3);
    $$ = new UnaryExpAST(ident->c_str(),func_rparams);
  } 
  ;

FuncRParam : Exp;

FuncRParamArray 
  : FuncRParam {
    auto func_rparam = std::unique_ptr<BaseAST>($1);
    auto func_rparams = new std::vector<std::unique_ptr<BaseAST>>();
    func_rparams->push_back(std::move(func_rparam));
    $$ = func_rparams;
  }
  | FuncRParam ',' FuncRParamArray {
    auto func_rparam = std::unique_ptr<BaseAST>($1);
    auto func_rparams = (std::vector<std::unique_ptr<BaseAST>>*)($3);
    func_rparams->push_back(std::move(func_rparam));
    $$ = func_rparams;
  }
  | {
    $$ = new std::vector<std::unique_ptr<BaseAST>>();
  }
  ;

UNARYOP 
  : '+' {
    $$ = new string("+");
  }
  | '-' {
    $$ = new string("-");
  }
  | '!' {
    $$ = new string("!");
  }
  ;


MulExp 
  : UnaryExp {
    auto unaryexp = std::unique_ptr<BaseAST>($1);
    $$ = new MulExpAST(unaryexp);
  }
  | MulExp MULOP UnaryExp {
    auto mulexp = std::unique_ptr<BaseAST>($1);
    auto mulop = std::unique_ptr<std::string>($2);
    auto unaryexp = std::unique_ptr<BaseAST>($3);
    $$ = new MulExpAST(mulexp,mulop->c_str(),unaryexp);
  }
  ;

MULOP
  : '*' {
    $$ = new string("*");
  }
  | '/' {
    $$ = new string("/");
  }
  | '%' {
    $$ = new string("%");
  }
  ;

AddExp
  : MulExp {
    auto mulexp = std::unique_ptr<BaseAST>($1);
    $$ = new AddExpAST(mulexp);
  }
  | AddExp ADDOP MulExp {
    auto addexp = std::unique_ptr<BaseAST>($1);
    auto addop = std::unique_ptr<std::string>($2);
    auto mulexp = std::unique_ptr<BaseAST>($3);
    $$ = new AddExpAST(addexp,addop->c_str(),mulexp);
  }
  ;

ADDOP 
  : '+' {
    $$ = new string("+");
  }
  | '-' {
    $$ = new string("-");
  }
  ;

RelExp 
  : AddExp {
    auto addexp = std::unique_ptr<BaseAST>($1);
    $$ = new RelExpAST(addexp);
  }
  | RelExp RELOP AddExp {
    auto relexp = std::unique_ptr<BaseAST>($1);
    auto relop = std::unique_ptr<std::string>($2);
    auto addexp = std::unique_ptr<BaseAST>($3);
    $$ = new RelExpAST(relexp,relop->c_str(),addexp);
  }
  ;

EqExp 
  : RelExp {
    auto relexp = std::unique_ptr<BaseAST>($1);
    $$ = new EqExpAST(relexp);
  }
  | EqExp EQOP RelExp {
    auto eqexp = std::unique_ptr<BaseAST>($1);
    auto eqop = std::unique_ptr<std::string>($2);
    auto relexp = std::unique_ptr<BaseAST>($3);
    $$ = new EqExpAST(eqexp,eqop->c_str(),relexp);
  }
  ;

LAndExp 
  : EqExp {
    auto eqexp = std::unique_ptr<BaseAST>($1);
    $$ = new LAndExpAST(eqexp);
  }
  | LAndExp AND EqExp {
    auto landexp = std::unique_ptr<BaseAST>($1);
    auto andop = std::unique_ptr<std::string>($2);
    auto eqexp = std::unique_ptr<BaseAST>($3);
    $$ = new LAndExpAST(landexp,andop->c_str(),eqexp);
  }
  ;

LOrExp 
  : LAndExp {
    auto landexp = std::unique_ptr<BaseAST>($1);
    $$ = new LOrExpAST(landexp);
  }
  | LOrExp OR LAndExp {
    auto lorexp = std::unique_ptr<BaseAST>($1);
    auto orop = std::unique_ptr<std::string>($2);
    auto landexp = std::unique_ptr<BaseAST>($3);
    $$ = new LOrExpAST(lorexp,orop->c_str(),landexp);
  }
  ;

ConstExp : Exp;

%%

void yyerror(unique_ptr<BaseAST> &ast,const char *s) {
  extern int yylineno;
  extern char *yytext;
  int len = strlen(yytext);
  int i;
  char buf[512] = {0};
  for (i=0; i<len; ++i)
    sprintf(buf, "%s%d ", buf, yytext[i]);
  fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, buf, yylineno);

  /* cerr << "error: " << s << endl; */
}