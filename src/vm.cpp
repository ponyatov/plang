/// @file

#include <vm.hpp>

/// @ingroup asm
/// @{

/// current source code file name
char *yyname = NULL;

#define YYERR                                                                  \
  "\nerror " << yyname << ":" << yylineno << "\t" << msg << " [" << yytext     \
             << "]\n"

void yyerror(std::string msg) {
  std::cout << YYERR;
  std::cerr << YYERR;
  exit(-1);
}

/// @}

/// @ingroup vm
/// @{

/// program entry point
int main(int argc, char *argv[]) {

  // header
  printf("\n%s\n%s\ngithub: %s\n\n", TITLE, COPYRIGHT, GITHUB);
  for (auto i = 0; i < argc; i++)
    printf("argv[%i] = <%s>\n", i, argv[i]);

  // assembler
  printf("\n<<asm>>\n");
  yyname = argv[1];
  yyin = fopen(yyname, "r");
  if (!yyin)
    abort();
  yyparse();

  // run byte-code interpreter
  bcx();

  // fake return
  return 0;
}

/// @}

/// @ingroup mem
/// @{

/// @brief main data/program memory:
/// bytecode shared with data in a single address space
BYTE M[Msz];
/// instruction pointer @ M
CELL Ip = 0;
/// compiler pointer @ M
CELL Cp = 0;

/// return stack
CELL R[Rsz];
/// return stack pointer @ R
CELL Rp = 0;

/// data stack: uses full host machine word to avoid overflow
int D[Dsz];
/// data stack pointer @ D
BYTE Dp = 0;

/// @}

void Cstore(CELL addr, CELL c) {
  assert(addr < Msz);
  *(CELL *)(&M[addr]) = c;
}

CELL Cfetch(CELL addr) {
  assert(addr < Msz);
  return *(CELL *)(&M[addr]);
}

void Bcompile(BYTE b) {
  printf("\n%.4X: %.2X", Cp, b);
  M[Cp++] = b;
}

void Ccompile(CELL c) {
  printf(" %.4X", c);
  Cstore(Cp, c);
  Cp += sizeof(CELL);
}

std::map<std::string, CELL> label;
std::map<std::string, std::vector<CELL>> forward;

void Ldefine(std::string *name) {
  printf("\n\n%.4X:\t%s\n", Cp, name->c_str());
  // store to table
  label[*name] = Cp;
  // resolve forwards
  auto fw = forward.find(*name);
  if (fw != forward.end()) {
    for (auto it = fw->second.begin(); it != fw->second.end(); it++)
      Cstore(*it, Cp);
    forward.erase(fw); // done
  }
}

void Lcompile(std::string *name) {
  if (label.find(*name) != label.end()) {
    Ccompile(label[*name]);
  } else {
    if (forward.find(*name) == forward.end()) // new forward
      forward[*name] = std::vector<CELL>();
    forward[*name].push_back(Cp);
    Ccompile(-1);
  }
}

void Lunresolved() {
  for (auto fw = forward.begin(); fw != forward.end(); fw++) {
    std::cerr << "\nunresolved forward: " << fw->first << "\n";
    abort();
  }
}

/// @ingroup bcx
void bcx() {
  Bcompile(op_BYE);
  Lunresolved();
  printf("\n\n<<bcx>>\n");
  yylineno = -1;
  while (true) {
    printf("\n%.4X: ", Ip);
    assert(Ip < Msz);
    BYTE op = M[Ip++];
    printf("%.2X ", op);
    switch (op) {
    case op_NOP:
      nop();
      break;
    case op_BYE:
      bye();
      break;
    case op_JMP:
      jmp();
      break;
    // case op_qJMP:
    //   qjmp();
    //   break;
    case op_CALL:
      call();
      break;
    case op_RET:
      ret();
      break;
    default:
      yyerror("unknown command");
    }
  }
}

void nop() { printf("nop"); }

void bye() {
  printf("bye\n\n");
  exit(0);
}

void jmp() {
  CELL addr = Cfetch(Ip);
  assert(addr < Msz);
  Ip = addr;
  printf("jmp %.4X", Ip);
}

void call() {
  CELL addr = Cfetch(Ip);
  assert(addr < Msz);
  R[Rp++] = Ip + sizeof(CELL);
  assert(Rp < Rsz);
  Ip = addr;
  printf("call %.4X", Ip);
}

void ret() {
  printf("ret");
  assert(Rp > 0);
  CELL addr = R[--Rp];
  assert(addr < Msz);
  Ip = addr;
}
