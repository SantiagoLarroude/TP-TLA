%{

#include "flex-actions.h"


/* Macros and constants */
// Debug

// Log token
#define log_debug_token(yytxt)                                              \
        LogDebug("Token: '%s'.", yytxt);


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

comment         #.*
/* [ \t\v\f\n\r] */
whitespace      [[:space:]]

/* [a-zA-Z] */
alpha           [[:alpha:]]
/* [0-9] */
digit           [[:digit:]]
/* [a-zA-Z]+[a-zA-Z0-9_\-]* */
id              {alpha}+[{alpha}{digit}_\-]*
/* Both integer and decimal */
/* number          {digit}+[\.]?{digit}+ */
number          ({digit}+|{digit}+[\.]?{digit}+)

string          \".*\"
/* "dakjfkdlsjgaljd.gadsjbfdjba" --> esto deberia poder tomarlo el string */

row             [row|ROW]
column          [column|COLUMN]

/* https://regex101.com/r/UUUxB0/1 */
file            (^[\w\/]+\.csv)$ 
/* "test.csv" */

%%

"|"             { log_debug_token(yytext); return PIPE; }
"->"            { log_debug_token(yytext); return ASSIGN; }

"not"           { log_debug_token(yytext); return NOT; }
"and"           { log_debug_token(yytext); return AND; }
"or"            { log_debug_token(yytext); return OR; }

"eq"            { log_debug_token(yytext); return EQUALS; }
"neq"           { log_debug_token(yytext); return NOT_EQUALS; }

"gt"            { log_debug_token(yytext);  return GREATER_THAN; }
"ge"            { log_debug_token(yytext);  return GREATER_EQUAL; }
"lt"            { log_debug_token(yytext);  return LESS_EQUAL; }
"le"            { log_debug_token(yytext);  return LESS_THAN; }

"+"             { log_debug_token(yytext);  return ADD; }
"-"             { log_debug_token(yytext);  return SUB; }
"%"             { log_debug_token(yytext);  return MOD; }
"*"             { log_debug_token(yytext);  return MUL; }
"/"             { log_debug_token(yytext);  return DIV; }

"--"            { log_debug_token(yytext);  return STR_SUB; }
"++"            { log_debug_token(yytext);  return STR_ADD; }

"if"            { log_debug_token(yytext);  return IF; }
"else"          { log_debug_token(yytext);  return ELSE; }

"each"          { log_debug_token(yytext);  return EACH;}
"contains"      { log_debug_token(yytext);  return CONTAINS;}

","             { log_debug_token(yytext);  return COMMA; }
":"             { log_debug_token(yytext);  return COLON; }

"["             { log_debug_token(yytext);  return OPEN_BRACKETS; }
"]"             { log_debug_token(yytext);  return CLOSE_BRACKETS; }

"("             { log_debug_token(yytext);  return OPEN_PARENTHESIS; }
")"             { log_debug_token(yytext);  return CLOSE_PARENTHESIS; }

{row}           { log_debug_token(yytext);  return ROW;}
{column}        { log_debug_token(yytext);  return COLUMN;}

{id}            { return pattern_id(yytext); }

{string}        { return pattern_string(yytext); }

{number}        { return pattern_number(yytext); }

{whitespace}    { pattern_ignored(yytext); }
{comment}       { pattern_ignored(yytext); }

.               { return pattern_unknown(yytext); }

%%
