%{
    #include "vm.hpp"
%}

%defines %union { std::string *s; }

%token <s> NAME

%%
REPL : | REPL ex
ex : NAME