
%option noyywrap
%option nounput
%option noinput

%{

// 这里写一些全局的代码
// 因为最后要生成 C/C++ 文件, 实现主要逻辑的部分都是用 C/C++ 写的
// 难免会用到头文件, 所以通常头文件和一些全局声明/定义写在这里

#include <cstdlib>
#include <string>

#include "sysy.tab.hpp"

using namespace std;

%}

/* 空白符和注释 */
WhiteSpace        [ \t\n\r]*
LineComment       "//".*
BlockComment      \/\*([^\*]*|[\*]+[^\*\/])*[\*]+\/

/* 标识符 */
Identifier        [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal           [1-9][0-9]*
Octal             0[0-7]*
Hexadecimal       0[xX][0-9a-fA-F]+

/* 运算符 */
RelOP             [<>]=?
EqOP              [!=]=

%%


{WhiteSpace}      {}
{LineComment}     {} 
{BlockComment}    {}
 
"int"             { return INT; }
"return"          { return RETURN; }
"const"           { return CONST; }
"void"            { return VOID; }
"if"              { return IF; }
"while"           { return WHILE; }
"break"           { return BREAK; }
"continue"        { return CONTINUE; }

  
"&&"              { yylval.str_val = new string(yytext); return AND; }
"||"              { yylval.str_val = new string(yytext); return OR; }

{Identifier}      { yylval.str_val = new string(yytext); return IDENT; }

{RelOP}           { yylval.str_val = new string(yytext); return RELOP; }
{EqOP}            { yylval.str_val = new string(yytext); return EQOP; }

{Decimal}         { yylval.int_val = strtol(yytext,nullptr,0); return INT_CONST; }
{Octal}           { yylval.int_val = strtol(yytext,nullptr,0); return INT_CONST; }
{Hexadecimal}     { yylval.int_val = strtol(yytext,nullptr,0); return INT_CONST; }

.                 { return yytext[0]; }



%%

