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
comment         #.*

/* [ \t\v\f\n\r] */
whitespace      ([[:space:]])

cr              \r
lf              \n
endline         \.({whitespace})*({comment})?({cr}|{lf}|{cr}{lf})

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

%%

{comment}       { pattern_ignored(yytext); }

{character}     { return pattern_char(yytext); }
{string}        { return pattern_string(yytext); }
{number}        { return pattern_number(yytext); }

"function"      { log_debug_token(yytext);  return FUNCTION; }
"return"        { log_debug_token(yytext);  return RETURN; }
"end"           { log_debug_token(yytext);  return END; }

","             { log_debug_token(yytext);  return COMMA; }
":"             { log_debug_token(yytext);  return COLON; }
"."             { log_debug_token(yytext);  return DOT; }

"["             { log_debug_token(yytext);  return OPEN_BRACKETS; }
"]"             { log_debug_token(yytext);  return CLOSE_BRACKETS; }

"("             { log_debug_token(yytext);  return OPEN_PARENTHESIS; }
")"             { log_debug_token(yytext);  return CLOSE_PARENTHESIS; }

"->"            { log_debug_token(yytext); return ASSIGN; }
"->!"           { log_debug_token(yytext); return FSTREAM_OVERWRITE; }

"not"           { log_debug_token(yytext); return NOT; }
"and"           { log_debug_token(yytext); return AND; }
"or"            { log_debug_token(yytext); return OR; }

"=="            { log_debug_token(yytext); return EQUALS; }
"!="            { log_debug_token(yytext); return NOT_EQUALS; }

">"             { log_debug_token(yytext);  return GREATER_THAN; }
">="            { log_debug_token(yytext);  return GREATER_EQUAL; }
"<"             { log_debug_token(yytext);  return LESS_THAN; }
"<="            { log_debug_token(yytext);  return LESS_EQUAL; }

"+"             { log_debug_token(yytext);  return ADD; }
"-"             { log_debug_token(yytext);  return SUB; }
"*"             { log_debug_token(yytext);  return MUL; }
"/"             { log_debug_token(yytext);  return DIV; }
"%"             { log_debug_token(yytext);  return MOD; }

"++"            { log_debug_token(yytext);  return STR_ADD; }
"--"            { log_debug_token(yytext);  return STR_SUB; }

"let"           { log_debug_token(yytext);  return LET; }
"be"            { log_debug_token(yytext);  return BE; }

"if"            { log_debug_token(yytext);  return IF; }
"then"          { log_debug_token(yytext);  return THEN; }
"else"          { log_debug_token(yytext);  return ELSE; }

"for"           { log_debug_token(yytext);  return FOR; }
"in"            { log_debug_token(yytext);  return IN; }
"do"            { log_debug_token(yytext);  return DO; }

"as"            { log_debug_token(yytext);  return AS; }
"with"          { log_debug_token(yytext);  return WITH; }

"STDOUT"        { log_debug_token(yytext);  return FSTREAM_STDOUT; }

"True"          { log_debug_token(yytext);  return TTRUE; }
"False"         { log_debug_token(yytext);  return TFALSE; }

"is"            { log_debug_token(yytext);  return IS; }
"File"          { log_debug_token(yytext);  return TYPE_FILE; }
"Number"        { log_debug_token(yytext);  return TYPE_NUMBER; }
"String"        { log_debug_token(yytext);  return TYPE_STRING; }
"Boolean"       { log_debug_token(yytext);  return TYPE_BOOLEAN; }

{id}            { return pattern_id(yytext); }

{endline}       { log_debug_token(yytext); return TEOL; }

{whitespace}    { pattern_ignored(yytext); }

.               { return pattern_unknown(yytext); }

<<EOF>>         { yyterminate(); return 0; }
%%
