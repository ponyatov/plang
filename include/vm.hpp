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
#define Msz 0x20
/// return (call) stack size, cells
#define Rsz 0x100
/// data stack size, cells
#define Dsz 0x10

/// @}

/// @defgroup asm asm
/// @brief simple FORTH-like portable bytecode assembler
/// @{

/// @brief assembler
/// @param[in] pfile source code file path
void assembler(const char *pfile);

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
extern CELL Hp;
extern BYTE *M;
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
extern void Cbyte(BYTE b);
/// compile cell: `C[Cp+=CELL] = c`
extern void Ccell(CELL c);

/// compile word header
extern void Cheader(std::string *s);

/// @name word header structure /offsets/
/// @{

/// LFA: Link Field Area
#define LFA (0)
/// AFA: Attributes Field Area (IMMED,..)
#define AFA (LFA + sizeof(CELL))
/// NFA: Name Field Area (tiny byte-counted ASCII string)
#define NFA (AFA + sizeof(BYTE))
/// maximum word name length
#define NFA_MAX 0x11
/// CFA: Code Field Area (bytecode)
#define CFA (NFA + sizeof(BYTE) + M[NFA])

/// @}

/// @}

/// @defgroup bcx bcx
/// @brief bytecode interpreter (executor)
/// @ingroup vm

/// @brief bytecode pipeline loop: fetch/decode/execute
/// @ingroup vm
extern void bcx();

/// @defgroup persist persist
/// @brief `libpmem` persistent layer
/// @{

#include <libpmem.h>

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/// image file path
#define M_NVRAM "tmp/M.nvram"
#define M_NVRAM_UMASK (S_IRUSR | S_IWUSR)
#define MMAP_PROT (PROT_READ | PROT_WRITE)

/// program entry point
#define NVRAM_Ip (sizeof(CELL) * 0)
/// compiler pointer
#define NVRAM_Cp (sizeof(CELL) * 1)
/// latest word in vocabulary
#define NVRAM_Hp (sizeof(CELL) * 2)
/// memory image header size
#define NVRAM_HEADER (sizeof(CELL) * 3)

/// persistent layer init
/// @return true if image file exists
extern bool pinit(const char *path);

/// sync system state `->` image
extern void psync();

/// @}

#endif // _VM_HPP

/// @}
