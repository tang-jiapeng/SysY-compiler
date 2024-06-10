#include <iostream>
#include "ast.h"
#include <assert.h>


using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  assert(argc == 4);

  auto input = argv[1];
  // auto output = argv[3];

  yyin = fopen(input, "r");

  assert(yyin);

  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  // ast->print(1);
  cout << endl;

  return 0;
}
