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

"function"      { return all_type_of_pattern(yytext, FUNCTION); }
"return"        { return all_type_of_pattern(yytext, RETURN); }
"end"           { return all_type_of_pattern(yytext, END);  }

","             { return all_type_of_pattern(yytext, COMMA); }
":"             { return all_type_of_pattern(yytext, COLON); }
"."             { return all_type_of_pattern(yytext,DOT); }

"["             { return all_type_of_pattern(yytext, OPEN_BRACKETS); }
"]"             { return all_type_of_pattern(yytext, CLOSE_BRACKETS); }

"("             { return all_type_of_pattern(yytext, OPEN_PARENTHESIS); }
")"             { return all_type_of_pattern(yytext, CLOSE_PARENTHESIS); }

"->"            { return all_type_of_pattern(yytext, ASSIGN); }
"->!"           { return all_type_of_pattern(yytext, FSTREAM_OVERWRITE); }

"not"           { return all_type_of_pattern(yytext, NOT); }
"and"           { return all_type_of_pattern(yytext, AND); }
"or"            { return all_type_of_pattern(yytext, OR); }

"=="            { return all_type_of_pattern(yytext, EQUALS); }
"!="            { return all_type_of_pattern(yytext, NOT_EQUALS); }

">"             { return all_type_of_pattern(yytext, GREATER_THAN); }
">="            { return all_type_of_pattern(yytext, GREATER_EQUAL); }
"<"             { return all_type_of_pattern(yytext, LESS_THAN); }
"<="            { return all_type_of_pattern(yytext, LESS_EQUAL); }

"+"             { return all_type_of_pattern(yytext, ADD); }
"-"             { return all_type_of_pattern(yytext, SUB); }
"*"             { return all_type_of_pattern(yytext, MUL); }
"/"             { return all_type_of_pattern(yytext, DIV); }
"%"             { return all_type_of_pattern(yytext, MOD); }

"++"            { return all_type_of_pattern(yytext, STR_ADD); }
"--"            { return all_type_of_pattern(yytext, STR_SUB); }

"let"           { return all_type_of_pattern(yytext, LET); }
"be"            { return all_type_of_pattern(yytext, BE); }

"if"            { return all_type_of_pattern(yytext, IF); }
"then"          { return all_type_of_pattern(yytext, THEN); }
"else"          { return all_type_of_pattern(yytext, ELSE); }

"for"           { return all_type_of_pattern(yytext, FOR); }
"in"            { return all_type_of_pattern(yytext, IN); }
"do"            { return all_type_of_pattern(yytext, DO); }

"as"            { return all_type_of_pattern(yytext, AS); }
"with"          { return all_type_of_pattern(yytext, WITH); }

"STDOUT"        { return all_type_of_pattern(yytext, FSTREAM_STDOUT); }

"True"          { return pattern_bool(true); }
"False"         { return pattern_bool(false); }

"is"            { return all_type_of_pattern(yytext, IS); }
"File"          { log_debug_token(yytext);  return TYPE_FILE; }
"Number"        { return pattern_number(yytext); }
"String"        { return pattern_string(yytext); }
"Boolean"       { return pattern_bool(yytext); }

{id}            { return pattern_id(yytext); }

{endline}       { return all_type_of_pattern(yytext, TEOL); }

{whitespace}    { pattern_ignored(yytext); }

.               { return pattern_unknown(yytext); }

<<EOF>>         { yyterminate(); return 0; }
%%
