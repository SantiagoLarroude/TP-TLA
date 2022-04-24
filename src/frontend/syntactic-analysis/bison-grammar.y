%{

#include "bison-actions.h"

/* Top level root node in the AST */
node_block* programblock = NULL;

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
%token <token> OR AND NOT

/* Relational*/
%token <token>  EQUALS NOT_EQUALS
                GREATER_THAN GREATER_EQUAL LESS_THAN LESS_EQUAL

/* Arithmetic (numeric) */
%token <token> ADD SUB MOD MUL DIV

/* Arithmetic (string) */
%token <token> STR_SUB STR_ADD

/* Conditional */
%token <token> IF
%token <token> THEN
%token <token> ELSE

/* Loop */
%token <token> EACH CONTAINS

/* Comma (,) and Colon (:) */
%token <token> COMMA COLON

/* Brackets */
%token <token> OPEN_BRACKETS CLOSE_BRACKETS

/* Parentheses */
%token <token> OPEN_PARENTHESIS CLOSE_PARENTHESIS

%token <token> START_FN END_FN

/* Data types */
%token <string> NUMBER
%token <string> STRING

%token <file> FILE_TYPE

/* File internals */
%token <token> ROW
%token <token> COLUMN


// Types

%type <void>    program
%type <id>      identifier
%type <cmd>     commands command
%type <expr>    expression bool_expr
                list id_list
                constant
                if
// %type <token>   comparison num_arithm str_arithm

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

/* Avoids Shift/Reduce conflict with if/then/else.
 *
 * See sections 
 * 5.2 Shift/Reduce Conflicts
 * 5.3.3 Specifying Precedence Only
 * from the Bison manual for a detailed explanation.
 */
%precedence THEN
%precedence ELSE

/* Root node */
%start program


%%


program     :   commands { grammar_program($1); }
            ;

commands    :   command
            |   commands command
            ;

command     :   expression {
                    $$ = grammar_command_from_expression($1);
                }
            ;

expression  :   expression ASSIGN identifier { 
                    $$ = grammar_expression_assignment($1, $3); 
                }
            |   expression ASSIGN OPEN_BRACKETS id_list CLOSE_BRACKETS {
                    $$ = grammar_expression_assignment_list($1, $4);
                }
//             |   expression comparison expression {
//                     $$ = grammar_expression_comparison($2, $1, $3);
//                 }
//             |   expression num_arithm expression {
//                     $$ = grammar_expression_arithmetic_numeric($2, $1, $3);
//                 }
//             |   expression str_arithm expression {
//                     $$ = grammar_expression_arithmetic_string($2, $1, $3);
//                 }
            |   OPEN_BRACKETS list CLOSE_BRACKETS {}
            |   OPEN_PARENTHESIS expression CLOSE_PARENTHESIS { 
                    $$ = $2;        /* TODO: Test this. (!) */
                }
            |   bool_expr
            |   if
            |   constant
            ;

// loop       :   EACH COLUMN CONTAINS expression {}
//            |   EACH ROW CONTAINS expression {}
//            ;

if          :   IF expression THEN expression ELSE expression {
                    $$ = grammar_expression_if($2, $4, $6);
                }
            ;

bool_expr   :   expression AND expression {
                    LogDebug("Reached AND\n");
                    $$ = grammar_expression_logic(AND, $1, $3);
                }
            |   expression OR expression {
                    LogDebug("Reached OR\n");
                    $$ = grammar_expression_logic(OR, $1, $3);
                }
            |   NOT expression {
                    LogDebug("Reached NOT\n");
                    $$ = grammar_expression_logic(NOT, NULL, $2);
                }
            /* |   expression comparison expression {
                    $$ = grammar_expression_comparison($2, $1, $3);
                } */
            ;


// file_param  :   FILE_TYPE
//             |   STRING
//             ;

// comparison  :   EQUALS | NOT_EQUALS
//             |   GREATER_THAN | GREATER_EQUAL | LESS_THAN | LESS_EQUAL
//             ;
// 
// num_arithm  :   ADD | SUB
//             |   MUL | DIV 
//             |   MOD
//             ;
// 
// str_arithm  :   STR_ADD | STR_SUB
//             ;

list        :   /* blank */ {
                    $$ = grammar_expression_list_new(NULL);
                }
            |   expression {
                    $$ = grammar_expression_list_new($1);
                }
            |   list COMMA expression {
                    $1 = grammar_expression_list_append($1, $3); 
                }
            ;

id_list     :   ID               { $$ = grammar_identifier_list_new($1); }
            |   id_list COMMA ID {
                    $1 = grammar_identifier_list_append($1, $3);
                }
            ;

identifier  :   ID { $$ = grammar_identifier($1); }
            ;

constant    :   NUMBER      { $$ = grammar_constant_number($1); }
            |   STRING      { $$ = grammar_constant_string($1); }
            ;

%%
