#ifndef _VM_HPP
#define _VM_HPP

#define TITLE "P language: programming over `libpmem` persistent memory"
#define AUTHOR "Dmitry Ponyatov"
#define EMAIL "<dponyatov@gmail.com>"
#define COPYRIGHT "(c) " AUTHOR " " EMAIL " 2021 MIT"

#include <cstdio>

extern int yylex();
extern int yylineno;
extern char *yytext;
extern int yyparse();
#include "parser.hpp"

#endif // _VM_HPP
