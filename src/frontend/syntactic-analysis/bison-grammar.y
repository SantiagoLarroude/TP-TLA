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
%token <token> IF THEN ELSE

/* Loop */
%token <token> EACH CONTAINS

/* Comma (,) and Colon (:) */
%token <token> COMMA COLON EXCLAMATION

/* Brackets */
%token <token> OPEN_BRACKETS CLOSE_BRACKETS

/* Parentheses */
%token <token> OPEN_PARENTHESIS CLOSE_PARENTHESIS

/* Functions */
%token <token> START_FN END_FN

/* Files */
%token <token> FSTREAM_STDOUT FSTREAM_OVERWRITE

/* File internals */
%token <token> TROW TCOLUMN TSEPARATOR

/* Data types */
%token <string> NUMBER
%token <string> CHARACTER
%token <string> STRING
%token <token>  TFILE



// Types

%type <void>    program
%type <fun>     functions function
/* %type <cmd>     commands command */
%type <id>      identifier
%type <expr>    expressions expression
                bool_expr cmp_expr 
                num_arithm str_arithm
                list id_list
                cond_expr
                file_ops
                constant
%type <file>    file_expr

// %type <token>   comparison /* num_arithm str_arithm */

// Associativity and precedence rules (from lower precedence to higher)
%left   PIPE
%left   TROW TCOLUMN
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
%left   COLON EXCLAMATION


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


program     :   functions { grammar_program($1); }
            ;

functions   :   function
            |   functions function
            ;

function    :   START_FN identifier expressions END_FN {
                    $$ = grammar_function_new($2, $3);
                }
            ;

/* commands    :   command
            |   commands command
            ;

command     :   expression {
                    $$ = grammar_command_from_expression($1);
                }
            ; */

expressions : expression 
            | expressions expression
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
            |   num_arithm
            |   str_arithm
            |   file_expr {
                    $$ = grammar_expression_from_file($1);
                }
            |   file_ops
            |   identifier { $$ = grammar_expression_from_identifier($1); }
            |   constant
            ;

file_expr   :   TFILE STRING ASSIGN identifier {
                    $$ = grammar_file_new($2, $4);
                }
            |   FSTREAM_STDOUT ASSIGN identifier {
                    $$ = grammar_file_new("STDOUT", $3);
                }
            |   TFILE ASSIGN identifier {
                    $$ = grammar_file_new("ANONYMOUS", $3);
                }
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

file_ops    :   identifier FSTREAM_OVERWRITE identifier {
                    $$ = grammar_expression_file_overwrite($1, $3);
                }
            |   identifier COLON expressions EXCLAMATION {
                    $$ = grammar_expression_apply_to_file($1, $3);
                }
            |   expression TROW NUMBER {
                    $$ = grammar_expression_get_column_row_constant(TROW,
                                                                    $1, $3);
                }
            |   expression TROW OPEN_BRACKETS list CLOSE_BRACKETS {
                    $$ = grammar_expression_get_column_row_list(TROW,
                                                                $1, $4);
                }
            |   expression TROW identifier {
                    $$ = grammar_expression_get_column_row_id(TROW,
                                                              $1, $3);
                }
            |   expression TCOLUMN NUMBER {
                    $$ = grammar_expression_get_column_row_constant(TCOLUMN,
                                                                    $1, $3);
                }
            |   expression TCOLUMN OPEN_BRACKETS list CLOSE_BRACKETS {
                    $$ = grammar_expression_get_column_row_list(TCOLUMN,
                                                                $1, $4);
                }
            |   expression TCOLUMN identifier {
                    $$ = grammar_expression_get_column_row_id(TCOLUMN,
                                                              $1, $3);
                }
            |   identifier COLON TSEPARATOR CHARACTER EXCLAMATION {
                    $$ = grammar_expression_separator($1, $4);
                }
            |   identifier COLON TSEPARATOR OPEN_BRACKETS list CLOSE_BRACKETS
                EXCLAMATION {
                    $$ = grammar_expression_separator_list($1, $5);
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
            |   CHARACTER   { $$ = grammar_constant_char($1); }
            |   STRING      { $$ = grammar_constant_string($1); }
            ;

%%
