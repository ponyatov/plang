%{
    #include "vm.hpp"
%}

%option noyywrap yylineno

%%
\#[^\n]*    { }                                 /* line comment     */

nop         { yylval.op = op_NOP; return cmd0; }
bye         { yylval.op = op_BYE; return cmd0; }

jmp         { yylval.op = op_JMP; return cmd1; }
\?jmp       { yylval.op = op_qJMP; return cmd1; }
call        { yylval.op = op_CALL; return cmd1; }
ret         { yylval.op = op_RET; return cmd0; }

":"         { return tCOLON; }

[a-z]+      { yylval.s = new std::string(yytext); return tNAME; }

[ \t\r\n]+  { }                                 /* drop spaces      */
.           { yyerror("lexer"); }               /* unparsed text    */
