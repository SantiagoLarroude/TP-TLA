%{

#include "bison-actions.h"

/* Top level root node in the AST */
node_block * programblock = NULL;

%}

%define api.value.type {union YYSTYPE}


// Flex tokens:

/* Identifier */
%token <string> ID

/* Pipe operator */
%token <token> PIPE

/* Assignment */
%token <token> ASSIGN

/* Logic */
%token <token> NOT AND OR

/* Relational*/
%token <token>  EQUALS NOT_EQUALS
                GREATER_THAN GREATER_EQUAL LESS_THAN LESS_EQUAL

/* Arithmetic (numeric) */
%token <token> ADD SUB MOD MUL DIV

/* Arithmetic (string) */
%token <token> STR_SUB STR_ADD

/* Conditional */
%token <token> IF
%token <token> ELSE

/* Loop */
%token <token> EACH CONTAINS

/* Comma (,) and Colon (:) */
%token <token> COMMA COLON

/* Brackets */
%token <token> OPEN_BRACKETS CLOSE_BRACKETS

/* Parentheses */
%token <token> OPEN_PARENTHESIS CLOSE_PARENTHESIS

/* Data types */
%token <string> NUMBER
%token <string> STRING
    /* %token FILE_TYPE */

/* File internals */
%token <token> ROW
%token <token> COLUMN


// Types

%type <int> program /* stmts */ /* block */

/* %type <stmt>    stmt if_stmt loop_stmt; */
%type <expr>    expression
                /* list id_list */
                constant
%type <token>   comparison num_arithm str_arithm;

// Associativity and precedence rules (from lower precedence to higher)
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

/* Root node */
%start program


%%


program     :   expression  { grammar_program($1); }
                /* stmts {} */
            ;

// stmts       :   stmt {}
//             |   stmts stmt {}
//             ;
// 
// stmt        :   expression {}
//             ;

expression  : /*  expression ASSIGN ID { $$ = AssignmentGrammarAction($1, $3); }
            |   expression ASSIGN id_list {}
            |*/   expression AND expression {
                    $$ = grammar_expression_logic($2, $1, $3);
                }
            |   expression OR expression {
                    $$ = grammar_expression_logic($2, $1, $3);
                }
            |   NOT expression {
                    $$ = grammar_expression_logic($1, NULL, $2);
                }
            |   expression comparison expression {
                    $$ = grammar_expression_comparison($2, $1, $3);
                }
            |   expression num_arithm expression {
                    $$ = grammar_expression_arithmetic_numeric($2, $1, $3);
                }
            |   expression str_arithm expression {
                    $$ = grammar_expression_arithmetic_string($2, $1, $3); 
                }
//            |   OPEN_BRACKETS list CLOSE_BRACKETS {}
            |   OPEN_PARENTHESIS expression CLOSE_PARENTHESIS { 
                    $$ = $2;        /* TODO: Test this. (!) */
                }
            | constant
            ;

// loop_stmt   :   EACH COLUMN CONTAINS expression {}
//             |   EACH ROW CONTAINS expression {}
//             ;

// if_stmt     :   IF expression {}
//             |   ELSE IF expression {}
//            |   ELSE expression {}
//            ;

comparison  :   EQUALS | NOT_EQUALS
            |   GREATER_THAN | GREATER_EQUAL | LESS_THAN | LESS_EQUAL
            ;

num_arithm  :   ADD | SUB
            |   MUL | DIV 
            |   MOD
            ;

str_arithm  :   STR_ADD | STR_SUB
            ;

// list        :   /* blank */ {}
//             |   expression {}
//             |   list COMMA expression {}
//             ;
// 
// id_list     : ID { $$ = NewList($1); }
//             | id_list COMMA ID { $1 = }
//             ;

constant    :   NUMBER      { $$ = grammar_constant_number($1); }
            |   STRING      { $$ = grammar_constant_string($1); }
            ;

%%
