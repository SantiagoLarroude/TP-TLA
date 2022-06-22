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

%%

{comment}       { pattern_ignored(yytext); }

{string}        { return pattern_string(yytext); }
{number}        { return pattern_number(yytext); }

"function"      { return pattern(yytext, FUNCTION); }
"return"        { return pattern(yytext, RETURN); }
"end"           { return pattern(yytext, END);  }

","             { return pattern(yytext, COMMA); }
":"             { return pattern(yytext, COLON); }
"."             { return pattern(yytext,DOT); }

"["             { return pattern(yytext, OPEN_BRACKETS); }
"]"             { return pattern(yytext, CLOSE_BRACKETS); }

"("             { return pattern(yytext, OPEN_PARENTHESIS); }
")"             { return pattern(yytext, CLOSE_PARENTHESIS); }

"->"            { return pattern(yytext, ASSING); }
"->!"           { return pattern(yytext, FSTREAM_OVERWRITE); }

"not"           { return pattern(yytext, NOT); }
"and"           { return pattern(yytext, AND); }
"or"            { return pattern(yytext, OR); }

"=="            { return pattern(yytext, EQUALS); }
"!="            { return pattern(yytext, NOT_EQUALS); }

">"             { return pattern(yytext, GREATER_THAN); }
">="            { return pattern(yytext, GREATER_EQUAL); }
"<"             { return pattern(yytext, LESS_THAN); }
"<="            { return pattern(yytext, LESS_EQUAL); }

"+"             { return pattern(yytext, ADD); }
"-"             { return pattern(yytext, SUB); }
"*"             { return pattern(yytext, MUL); }
"/"             { return pattern(yytext, DIV); }
"%"             { return pattern(yytext, MOD); }

"++"            { return pattern(yytext, STR_ADD); }
"--"            { return pattern(yytext, STR_SUB); }

"let"           { return pattern(yytext, LET); }
"be"            { return pattern(yytext, BE); }

"if"            { return pattern(yytext, IF); }
"then"          { return pattern(yytext, THEN); }
"else"          { return pattern(yytext, ELSE); }

"for"           { return pattern(yytext, FOR); }
"in"            { return pattern(yytext, IN); }
"do"            { return pattern(yytext, DO); }

"as"            { return pattern(yytext, AS); }
"with"          { return pattern(yytext, WITH); }

"STDOUT"        { return pattern(yytext, FSTREAM_STDOUT); }

"True"          { return pattern_bool(true); }
"False"         { return pattern_bool(false); }

"is"            { return pattern(yytext, IS); }
"File"          { log_debug_token(yytext);  return TYPE_FILE; }
"Number"        { return pattern_number(yytext); }
"String"        { return pattern_string(yytext); }
"Boolean"       { return pattern_bool(yytext); }

{id}            { return pattern_id(yytext); }

{endline}       { return pattern(yytext, TEOL); }

{whitespace}    { pattern_ignored(yytext); }

.               { return pattern_unknown(yytext); }

<<EOF>>         { yyterminate(); return 0; }
%%
