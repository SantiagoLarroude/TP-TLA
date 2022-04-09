%{

#include "bison-actions.h"

%}

/* // The union tag defaults to ‘YYSTYPE’
%union {
    char * string;
    double number;
} */

// Flex tokens:

/* Pipe operator */
%token PIPE

/* Assignment */
%token ASSIGN

/* Logic */
%token OR
%token AND
%token NOT

/* Relational*/
%token EQUALS
%token NOT_EQUALS

%token GREATER_THAN
%token GREATER_EQUAL
%token LESS_THAN
%token LESS_EQUAL

/* Arithmetic (numeric) */
%token ADD
%token SUB
%token MOD
%token MUL
%token DIV

/* Arithmetic (string) */
%token STR_SUB
%token STR_ADD

/* Brackets */
%token OPEN_BRACKETS
%token CLOSE_BRACKETS

/* Parentheses */
%token OPEN_PARENTHESIS
%token CLOSE_PARENTHESIS

/* Data types */
%token NUMBER
/* %token <string> STRING */

/* %token FILE_TYPE */

// Reglas de asociatividad y precedencia (de menor a mayor):
%left   PIPE
%left   ASSIGN
%left   OR
%left   AND
%left   EQUALS NOT_EQUALS
%left   GREATER_THAN GREATER_EQUAL LESS_THAN LESS_EQUAL
%left   ADD SUB
%left   MUL DIV MOD
%left   STR_ADD STR_SUB
%right  NOT
%left   OPEN_BRACKETS CLOSE_BRACKETS          // TODO: Consultar
%left   OPEN_PARENTHESIS CLOSE_PARENTHESIS    // TODO: Consultar

%%

program: expression                                                { $$ = ProgramGrammarAction($1); }
    ;

expression: expression ADD expression                            { $$ = AdditionExpressionGrammarAction($1, $3); }
    | expression SUB expression                                    { $$ = SubtractionExpressionGrammarAction($1, $3); }
    | expression MUL expression                                    { $$ = MultiplicationExpressionGrammarAction($1, $3); }
    | expression DIV expression                                    { $$ = DivisionExpressionGrammarAction($1, $3); }
    | factor                                                    { $$ = FactorExpressionGrammarAction($1); }
    ;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS            { $$ = ExpressionFactorGrammarAction($2); }
    | constant                                                    { $$ = ConstantFactorGrammarAction($1); }
    ;

constant: NUMBER                                                { $$ = IntegerConstantGrammarAction($1); }
    ;

/* value:  NUMBER    { $$ = NumberConstantGrammarAction($1); }
    |   STRING    { $$ = StringConstantGrammarAction($1); }
    ; */

%%
