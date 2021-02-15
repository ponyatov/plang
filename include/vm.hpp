/// @file

#ifndef _VM_HPP
#define _VM_HPP

/// @defgroup metainfo metainfo
/// @brief short system information
/// @{

#define TITLE "P language: programming over `libpmem` persistent memory"
#define AUTHOR "Dmitry Ponyatov"
#define EMAIL "<dponyatov@gmail.com>"
#define COPYRIGHT "(c) " AUTHOR " " EMAIL " 2021 MIT"
#define GITHUB "https://github.com/ponyatov/plang"

/// @}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#include "libpmem.h"

/// @defgroup vm vm
/// @brief Virtual Machine /FORTH inspired/
/// @{

/// @defgroup config config
/// @{

/// machine word size (reduced to 16 bit)
#define CELL uint16_t
/// single-byte addressing
#define BYTE uint8_t
/// main memory size, bytes
#define Msz 0x10
/// return (call) stack size, cells
#define Rsz 0x100
/// data stack size, cells
#define Dsz 0x10

/// @}

/// @defgroup asm asm
/// @brief simple FORTH-like portable bytecode assembler
/// @{

/// lexer /flex/
extern int yylex();
/// current lexer line number
extern int yylineno;
/// current lexer literal
extern char *yytext;
/// source code input from disk file
extern FILE *yyin;

/// syntax parser /bison/
extern int yyparse();
/// parser error callback
extern void yyerror(std::string msg);
#include "parser.hpp"

/// @name labels symbol table
/// @{

/// known backward references
extern std::map<std::string, CELL> label;
/// unknown *forward references*
extern std::map<std::string, std::vector<CELL>> forward;

/// define new label
extern void Ldefine(std::string *name);

/// compile known `label` by name or mark in `forward`
extern void Lcompile(std::string *name);

/// dump unresolved `forward`s and abort if exists
extern void Lunresolved();

/// @}
/// @}

/// @defgroup mem mem
/// @ingroup vm
/// @brief VM registers & memory spaces
/// @{

/// @name memory operations
/// @{

/// store cell in memory `M[addr]`
extern void Cstore(CELL addr, CELL c);
/// fetch cell from memory `M[addr]`
extern CELL Cfetch(CELL addr);

/// @}
/// @}
extern CELL Cp;
extern BYTE M[Msz];
extern CELL Ip;

/// @defgroup cmd commands
/// @ingroup vm
/// @brief byte-sized commands for stack machine
/// @{

#define op_NOP 0x00
#define op_BYE 0xFF

#define op_JMP 0x01
#define op_qJMP 0x02
#define op_CALL 0x03
#define op_RET 0x04
#define op_LIT 0x05

/// do nothing: code/data padding command
extern void nop();
/// stop system
extern void bye();

/// unconditional branch
extern void jmp();
/// conditional branch `( flag=F -- )`
extern void qjmp();
/// nested call `(R: -- addr )` /`addr` after command/
extern void call();
/// return from `call` `(R: addr -- )`
extern void ret();

/// @}

/// @defgroup compiler compiler
/// @brief *compilation in FORTH* means adding bytes to the end of used memory
/// @{

/// compile byte: `M[Cp++] = b`
extern void Bcompile(BYTE b);
/// compile cell: `C[Cp+=CELL] = c`
extern void Ccompile(CELL c);

/// @}

/// @defgroup bcx bcx
/// @brief bytecode interpreter (executor)
/// @ingroup vm

/// @brief bytecode pipeline loop: fetch/decode/execute
/// @ingroup vm
extern void bcx();

#endif // _VM_HPP

/// @}
