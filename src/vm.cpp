/// @file

#include <vm.hpp>

/// @ingroup asm
/// @{

/// current source code file name
char *yyname = NULL;

#define YYERR                                                                  \
  "\n\nerror " << yyname << ":" << yylineno << "\t" << msg << " [" << yytext   \
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

  // persistent layer init
  if (!pinit(M_NVRAM)) {
    assembler(argv[1]);
    Cbyte(op_BYE);
    Lunresolved();
    psync();
  }

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
BYTE *M = nullptr;
/// instruction pointer @ M
CELL Ip = NVRAM_HEADER;
/// compiler pointer @ M
CELL Cp = NVRAM_HEADER;
/// latest defined word header @ M
CELL Hp = NVRAM_HEADER;

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

void Cbyte(BYTE b) {
  printf("\n%.4X: %.2X", Cp, b);
  M[Cp++] = b;
}

void Ccell(CELL c) {
  printf(" %.4X", c);
  Cstore(Cp, c);
  Cp += sizeof(CELL);
}

void Cheader(std::string *s) {
  // LFA
  printf("\n%.4X:", Cp);
  Ccell(Hp);
  printf("\tLFA");
  Hp = Cp - sizeof(CELL);
  // AFA
  Cbyte(0x00);
  printf("\tAFA");
  // NFA
  BYTE len = strlen(s->c_str());
  assert(len < NFA_MAX);
  Cbyte(len);
  printf("\tNFA");
  const char *str = s->c_str();
  for (BYTE i = 0; i < len; i++) {
    Cbyte(str[i]);
    printf("\t'%c'", str[i]);
  }
}

std::map<std::string, CELL> label;
std::map<std::string, std::vector<CELL>> forward;

void Ldefine(std::string *name) {
  printf("\n\n%.4X:\t%s\n", Cp, name->c_str());
  // store to table
  label[*name] = Cp;
  // reassign entry point
  if (strcmp(name->c_str(), "init") == 0)
    Ip = Cp;
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
    Ccell(label[*name]);
  } else {
    if (forward.find(*name) == forward.end()) // new forward
      forward[*name] = std::vector<CELL>();
    forward[*name].push_back(Cp);
    Ccell(-1);
  }
}

void Lunresolved() {
  for (auto fw = forward.begin(); fw != forward.end(); fw++) {
    std::cerr << "\nunresolved forward: " << fw->first << "\n";
    abort();
  }
}

void assembler(const char *pfile) {
  printf("\n<< asm %s >>\n", pfile);
  yyin = fopen(pfile, "r");
  if (!yyin)
    abort();
  yyparse();
  fclose(yyin);
  // Lunresolved();
}

/// @ingroup bcx
void bcx() {
  printf("\n\n<< bcx >>\n");
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

// https://github.com/pmem/pmdk-examples/blob/master/hello_world/libpmem/hello_libpmem.c

bool pinit(const char *path) {
  printf("\n<< nvram %s >>\n", path);
  bool exists = true;
  // open image file
  int img = open(M_NVRAM, O_RDWR, M_NVRAM_UMASK);
  if (img < 0) {
    exists = false;
    img = open(M_NVRAM, O_RDWR | O_CREAT, M_NVRAM_UMASK);
    assert(img >= 0);
    assert(lseek(img, Msz - 1, SEEK_SET) >= 0);
    write(img, &img, 1);
    // close(img);
    // img = open(M_NVRAM, O_RDWR, M_NVRAM_UMASK);
  }
  // get stbuf.size
  struct stat stbuf;
  assert(fstat(img, &stbuf) >= 0);
  assert(stbuf.st_size == Msz);
  // mmap
  M = (BYTE *)mmap(NULL, stbuf.st_size, MMAP_PROT, MAP_SHARED, img, 0);
  assert(M != MAP_FAILED);
  close(img);
  //
  if (exists) {
    Ip = Cfetch(NVRAM_Ip);
    assert(Ip >= NVRAM_HEADER && Ip < Msz);
    Cp = Cfetch(NVRAM_Cp);
    assert(Cp >= NVRAM_HEADER && Cp < Msz);
    Hp = Cfetch(NVRAM_Hp);
    assert(Hp >= NVRAM_HEADER && Hp < Msz);
  } else {
    Ip = Cp = NVRAM_HEADER;
    Hp = 0; // end of word list
    memset(M, op_NOP, Msz);
  }
  //
  return exists;
}

void psync() {
  Cstore(NVRAM_Ip, Ip);
  Cstore(NVRAM_Cp, Cp);
  Cstore(NVRAM_Hp, Hp);
}
