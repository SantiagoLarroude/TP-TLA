%{

#include "flex-actions.h"

%}

/* Obliga a crear una regla explícita para manejar lexemas desconocidos. */
%option nodefault

/* Escanea un único archivo y se detiene. */
%option noyywrap

/* Dispone una variable global "yylineno" con el número de línea actual. */
%option yylineno

/* Patrones reutilizables. */

/* cr              \r
lf              \n
crlf            \r\n
endline         \n */

number          [0-9\.]
whitespace      [ \f\n\r\t\v]
comment         \#
string          \".\"
/* "dakjfkdlsjgaljd.gadsjbfdjba" --> esto deberia poder tomarlo el string */

%%

"|"                                         { return PIPE; }
"->"                                        { return ASSIGN; }

"or"                                        { return OR; }
"and"                                       { return AND; }

"eq"                                        { return EQUALS; }
"neq"                                       { return NOT_EQUALS; }

"gt"                                        { return GREATER_THAN; }
"ge"                                        { return GREATER_EQUAL; }
"lt"                                        { return LESS_EQUAL; }
"le"                                        { return LESS_THAN; }

"+"                                         { return ADD; }
"-"                                         { return SUB; }
"%"                                         { return MOD; }
"*"                                         { return MUL; }
"/"                                         { return DIV; }


"--"                                        { return STR_SUB; }
"++"                                        { return STR_ADD; }

"not"                                       { return NOT; }

"["                                         { return OPEN_BRACKETS; }
"]"                                         { return CLOSE_BRACKETS; }

"("                                         { return OPEN_PARENTHESIS; }
")"                                         { return CLOSE_PARENTHESIS; }

{string}                                    { return StringPatternAction(yytext); }

{number}+                                   { return NumberPatternAction(yytext); }

{whitespace}                                { IgnoredPatternAction(yytext); }
{comment}                                   { IgnoredPatternAction(yytext); }

.                                           { return UnknownPatternAction(yytext); }

%%
