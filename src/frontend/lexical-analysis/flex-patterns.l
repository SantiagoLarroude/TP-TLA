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
id              {alpha}+({alpha}|{digit}|_|\-)*
/* Both integer and decimal */
/* number          {digit}+[\.]?{digit}+ */
number          ({digit}+|{digit}+[\.]?{digit}+)

/* Source: https://stackoverflow.com/a/2039820 */
string          \"(\\.|[^"\\])*\"
character       \'(\\.|[^'\\])?\'

/* https://regex101.com/r/UUUxB0/1 */
/* file            (^[{alpha}{digit}\/]+\.csv)$  */
/* "test.csv" */

%%

{comment}       { pattern_ignored(yytext); }

"|"             { log_debug_token(yytext); return PIPE; }
"->"            { log_debug_token(yytext); return ASSIGN; }

"and"           { log_debug_token(yytext); return AND; }
"or"            { log_debug_token(yytext); return OR; }

"eq"            { log_debug_token(yytext); return EQUALS; }
"neq"           { log_debug_token(yytext); return NOT_EQUALS; }

"gt"            { log_debug_token(yytext);  return GREATER_THAN; }
"ge"            { log_debug_token(yytext);  return GREATER_EQUAL; }
"lt"            { log_debug_token(yytext);  return LESS_THAN; }
"le"            { log_debug_token(yytext);  return LESS_EQUAL; }

"+"             { log_debug_token(yytext);  return ADD; }
"-"             { log_debug_token(yytext);  return SUB; }
"*"             { log_debug_token(yytext);  return MUL; }
"/"             { log_debug_token(yytext);  return DIV; }
"%"             { log_debug_token(yytext);  return MOD; }

"++"            { log_debug_token(yytext);  return STR_ADD; }
"--"            { log_debug_token(yytext);  return STR_SUB; }

"not"           { log_debug_token(yytext); return NOT; }

"if"            { log_debug_token(yytext);  return IF; }
"then"          { log_debug_token(yytext);  return THEN; }
"else"          { log_debug_token(yytext);  return ELSE; }

"each"          { log_debug_token(yytext);  return EACH; }
"contains"      { log_debug_token(yytext);  return CONTAINS; }

"file"          { log_debug_token(yytext);  return TFILE; }
"STDOUT"        { log_debug_token(yytext);  return FSTREAM_STDOUT; }
"->!"           { log_debug_token(yytext);  return FSTREAM_OVERWRITE; }

","             { log_debug_token(yytext);  return COMMA; }
":"             { log_debug_token(yytext);  return COLON; }
"!"             { log_debug_token(yytext);  return EXCLAMATION; }

"["             { log_debug_token(yytext);  return OPEN_BRACKETS; }
"]"             { log_debug_token(yytext);  return CLOSE_BRACKETS; }

"("             { log_debug_token(yytext);  return OPEN_PARENTHESIS; }
")"             { log_debug_token(yytext);  return CLOSE_PARENTHESIS; }

"fn"            { log_debug_token(yytext);  return START_FN; }
"end"           { log_debug_token(yytext);  return END_FN; }

"row"           { log_debug_token(yytext);  return TROW; }
"column"        { log_debug_token(yytext);  return TCOLUMN; }
"separator"     { log_debug_token(yytext);  return TSEPARATOR; }

{id}            { return pattern_id(yytext); }

{character}     { return pattern_char(yytext); }
{string}        { return pattern_string(yytext); }
{number}        { return pattern_number(yytext); }

{whitespace}    { pattern_ignored(yytext); }

.               { return pattern_unknown(yytext); }

<<EOF>>         { yyterminate(); return 0; }
%%
