%{
    #include "vm.hpp"
%}

%defines %union { BYTE op; std::string *s; }

%token <s> tNAME

%token tCOLON

%token <op> cmd0 cmd1

%%
REPL : 
                                                    /* opcode-only command  */
REPL : REPL cmd0            { Cbyte($2); }
                                                    /* command with label   */
REPL : REPL cmd1 tNAME      { Cbyte($2); Lcompile($3); }
                                                    /* new label            */
REPL : REPL tNAME tCOLON    { Ldefine($2); }
REPL : REPL tCOLON tNAME    { Cheader($3); /*CFA*/ Ldefine($3); }
