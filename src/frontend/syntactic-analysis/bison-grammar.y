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
%type <expr>    expression
                bool_expr cmp_expr 
                num_arithm str_arithm
                list id_list
                cond_expr
                constant
// %type <token>   comparison /* num_arithm str_arithm */

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
            |   OPEN_BRACKETS list CLOSE_BRACKETS {}
            |   OPEN_PARENTHESIS expression CLOSE_PARENTHESIS { 
                    $$ = $2;        /* TODO: Test this. (!) */
                }
            |   bool_expr
            |   cond_expr
            |   constant
            |   num_arithm
            |   str_arithm
            ;

// loop       :   EACH COLUMN CONTAINS expression {}
//            |   EACH ROW CONTAINS expression {}
//            ;

cond_expr   :   IF expression THEN expression ELSE expression {
                    $$ = grammar_expression_if($2, $4, $6);
                }
            ;

bool_expr   :   expression AND expression {
                    $$ = grammar_expression_logic(AND, $1, $3);
                }
            |   expression OR expression {
                    $$ = grammar_expression_logic(OR, $1, $3);
                }
            |   NOT expression {
                    $$ = grammar_expression_logic(NOT, NULL, $2);
                }
            |   cmp_expr
            ;


// file_param  :   FILE_TYPE
//             |   STRING
//             ;

cmp_expr    :   expression EQUALS expression {
                        $$ = grammar_expression_comparison(EQUALS, $1, $3);
                }
            |   expression NOT_EQUALS expression {
                    $$ = grammar_expression_comparison(NOT_EQUALS, $1, $3);
                }
            |   expression GREATER_THAN expression {
                    $$ = grammar_expression_comparison(GREATER_THAN, $1, $3);
                }
            |   expression GREATER_EQUAL expression {
                    $$ = grammar_expression_comparison(GREATER_EQUAL, $1, $3);
                }
            |   expression LESS_THAN expression {
                    $$ = grammar_expression_comparison(LESS_THAN, $1, $3);
                }
            |   expression LESS_EQUAL expression {
                    $$ = grammar_expression_comparison(LESS_EQUAL, $1, $3);
                }
            ;

num_arithm  :   expression ADD expression {
                     $$ = grammar_expression_arithmetic_numeric(ADD, $1, $3);
                }
            |   expression SUB expression {
                     $$ = grammar_expression_arithmetic_numeric(SUB, $1, $3);
                }
            |   expression MUL expression {
                     $$ = grammar_expression_arithmetic_numeric(MUL, $1, $3);
                }
            |   expression DIV expression {
                     $$ = grammar_expression_arithmetic_numeric(DIV, $1, $3);
                }
            |   expression MOD expression {
                     $$ = grammar_expression_arithmetic_numeric(MOD, $1, $3);
                }
            ;
 
str_arithm  :   expression STR_ADD expression {
                     $$ = grammar_expression_arithmetic_string(STR_ADD, $1, $3);
                }
            |   expression STR_SUB expression {
                     $$ = grammar_expression_arithmetic_string(STR_SUB, $1, $3);
                }
            ;

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
