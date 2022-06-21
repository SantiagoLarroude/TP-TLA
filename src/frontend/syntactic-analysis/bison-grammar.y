%{

#include "bison-actions.h"

/* Top level root node in the AST */
// node_block* programblock = NULL;

%}

/* descomentar la linea 11 */
/* %define api.value.type {union YYSTYPE} */

%union{
    // No terminales
    struct program* program;
    struct node_function* fun;
    struct node_function_call* fun_call;
    struct node_expression* expr;
    struct node_file_block* file_block;
    struct node_list* list;
    struct variable* var;

    // Terminales
    union variable_value value;
    char* string;
    token_t token;
}

// Flex tokens:

/* Identifier */
%token <var> ID

%token <token> FUNCTION RETURN END

/* Comma (,) and Colon (:) */
%token <token> COMMA COLON DOT

/* Brackets */
%token <token> OPEN_BRACKETS CLOSE_BRACKETS

/* Parentheses */
%token <token> OPEN_PARENTHESIS CLOSE_PARENTHESIS

/* Assignment */
%token <token> ASSIGN FSTREAM_OVERWRITE

/* Logic */
%token <token> NOT AND OR

/* Relational*/
%token <token>  EQUALS NOT_EQUALS
                GREATER_THAN GREATER_EQUAL LESS_THAN LESS_EQUAL

/* Arithmetic (numeric) */
%token <token> ADD SUB MOD MUL DIV

/* Arithmetic (string) */
%token <token> STR_SUB STR_ADD

%token <token> LET BE

/* Conditional */
%token <token> IF THEN ELSE

/* Loop */
%token <token> FOR IN DO


%token <token> AS WITH

/* Files */
%token <token> FSTREAM_STDOUT

%token <var> NUMBER STRING
%token <var> TTRUE TFALSE

/* Data types */
%token <token> IS
%token <token> TYPE_FILE TYPE_NUMBER TYPE_STRING TYPE_BOOLEAN

/* End Of Line */
%token <token> TEOL

// TYPES
%type <program>         program
%type <fun>         function
%type <fun_call>    fn_calls fn_call
%type <expr>        expressions expression
                    file_decl
                    var_decl
                    conditional
                    loop
                    return
                    num_arithm num_type str_arithm str_type
                    bool_expr bool_type cmp_expr cmp_type
                    assign
                    identifier
                    constant bool_constant
%type <list>        list range
                    args_list
%type <file_block>  file_handle
%type <token>       data_type


// Associativity and precedence rules (from lower precedence to higher)
%left   TLINES TCOLS
%left   ASSIGN FSTREAM_OVERWRITE
%left   THEN
%left   ELSE
%left   OR
%left   AND
%left   IS
%left   EQUALS NOT_EQUALS
%left   GREATER_THAN GREATER_EQUAL LESS_THAN LESS_EQUAL
%left   ADD SUB
%left   MUL DIV MOD
%left   STR_ADD STR_SUB
%right  NOT
%left   OPEN_BRACKETS CLOSE_BRACKETS
%left   OPEN_PARENTHESIS CLOSE_PARENTHESIS
%left   COLON


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


program     :   function { grammar_program($1); }
            ;

function    :   FUNCTION identifier 
                OPEN_PARENTHESIS args_list CLOSE_PARENTHESIS
                    expressions
                return
                END {
                    $$ = grammar_new_function($6);
                }
            ;

expressions :   expression
            |   expressions expression
            ;

expression  :   file_decl
            |   identifier
            |   var_decl
            |   bool_constant
            |   constant
            |   conditional
            |   loop
            |   OPEN_BRACKETS list CLOSE_BRACKETS {
                    $$ = grammar_expression_from_list($2);
                } 
            |   num_arithm
            |   str_arithm
            |   fn_calls { $$ = grammar_expression_from_funcall($1); }
            |   file_handle {
                    $$ = grammar_expression_from_filehandler($1);
                }
            |   assign {
                    $$ = grammar_expression_from_assignment($1);
                }
            ;

fn_calls    :   fn_call
            |   fn_calls DOT fn_call {
                    $$ = grammar_concat_function_call($1, $3);
                }
            ;

fn_call    :   identifier OPEN_PARENTHESIS args_list CLOSE_PARENTHESIS {
                    $$ = grammar_function_call($1, $3);
                }
            |   identifier DOT identifier
                OPEN_PARENTHESIS args_list CLOSE_PARENTHESIS {
                    $$ = grammar_function_call_from_id($1,
                                                       grammar_function_call(
                                                                      $3, $5));
                }
            ;
/*
 Ejemplo:
    File "path" as input.
*/
file_decl   :   TYPE_FILE STRING AS identifier TEOL {
                    $$ = grammar_new_declaration_file_node($2, $4, NULL);
                }
            |   TYPE_FILE FSTREAM_STDOUT AS identifier TEOL {
                    $$ = grammar_new_declaration_stdout_node($4, NULL);
                }
            |   TYPE_FILE STRING WITH list AS identifier TEOL {
                    $$ = grammar_new_declaration_file_node($2, $6, $4);
                }
            |   TYPE_FILE FSTREAM_STDOUT WITH list AS identifier TEOL {
                    $$ = grammar_new_declaration_stdout_node($6, $4);
                }
            ;

file_handle :   WITH identifier COLON expressions TEOL {
                    $$ = grammar_using_file($2, $4);
                }
            ;

var_decl    :   LET identifier BE expression TEOL {
                    $$ = grammar_new_variable($2, $4);
                }
            ;

conditional :   IF bool_expr THEN expression ELSE expression TEOL {
                    $$ = grammar_new_conditional($2, $4, $6);
                }
            ;

loop        :    FOR identifier IN expression DO expression TEOL {
                      $$ = grammar_new_loop($2, $4, $6);
                  }

list        :   /* blank */ { $$ = grammar_new_list(NULL); }
            |   expression  { $$ = grammar_new_list($1); }
            |   range       { $$ = $1; }
            |   list COMMA expression {
                    $$ = grammar_concat_list_expr($1, $3);
                }
            |   list COMMA range {
                    $$ = grammar_concat_list_list($1, $3);
                }
            ;

range       :   NUMBER DOT DOT NUMBER {
                    $$ = grammar_new_list_from_range($1, $4);
                }
            ;

args_list   :   /* blank */ { $$ = grammar_new_list_args(NULL); }
            |   expression  { $$ = grammar_new_list_args($1); }
            |   args_list COMMA expression { 
                    $$ = grammar_concat_list_args($1, $3);
                }
            ;

num_arithm  :   num_type ADD num_type {
                     $$ = grammar_expression_arithmetic_num_add($1, $3);
                }
            |   num_type SUB num_type {
                     $$ = grammar_expression_arithmetic_num_sub($1, $3);
                } 
            |   num_type MUL num_type {
                     $$ = grammar_expression_arithmetic_num_mul($1, $3);
                }
            |   num_type DIV num_type {
                     $$ = grammar_expression_arithmetic_num_div($1, $3);
                }
            |   num_type MOD num_type {
                     $$ = grammar_expression_arithmetic_num_mod($1, $3);
                }
            ;

num_type    :   identifier
            |   fn_calls { $$ = grammar_expression_from_funcall($1); }
            |   constant
            ;

str_arithm  :   str_type STR_ADD str_type {
                     $$ = grammar_expression_arithmetic_str_add($1, $3);
                }
            |   str_type STR_SUB str_type {
                     $$ = grammar_expression_arithmetic_str_sub($1, $3);
                }
            ;
    
str_type    :   identifier
            |   fn_calls { $$ = grammar_expression_from_funcall($1); }
            |   constant
            ;


bool_expr   :   bool_type AND bool_type {
                    $$ = grammar_expression_bool_and($1, $3);
                }
            |   bool_type OR bool_type {
                    $$ = grammar_expression_bool_or($1, $3);
                }
            |   NOT bool_type {
                    $$ = grammar_expression_bool_not($2);
                }
            |   bool_type
            |   OPEN_PARENTHESIS bool_expr CLOSE_PARENTHESIS { $$ = $2; }
            ;

bool_type   :   identifier
            |   fn_calls { $$ = grammar_expression_from_funcall($1); }
            |   cmp_expr
            ;

cmp_expr    :   cmp_type EQUALS cmp_type {
                    $$ = grammar_expression_cmp_equals($1, $3);
                }
            |   cmp_type NOT_EQUALS cmp_type {
                    $$ = grammar_expression_cmp_not_equals($1, $3);
                }
            |   cmp_type GREATER_THAN cmp_type {
                    $$ = grammar_expression_cmp_greater_than($1, $3);
                }
            |   cmp_type GREATER_EQUAL cmp_type {
                    $$ = grammar_expression_cmp_greater_equal($1, $3);
                }
            |   cmp_type LESS_THAN cmp_type {
                    $$ = grammar_expression_cmp_less_than($1, $3);
                }
            |   cmp_type LESS_EQUAL cmp_type {
                    $$ = grammar_expression_cmp_less_equal($1, $3);
                }
            |   identifier IS data_type {
                    $$ = grammar_expression_cmp_by_type($1, $3);
                }
            ;


cmp_type    :   identifier
            |   fn_calls { $$ = grammar_expression_from_funcall($1); }
            |   bool_constant
            |   constant
            |   num_arithm
            |   str_arithm
            ;

return      :   RETURN identifier TEOL { $$ = grammar_new_return_node($2); }
            |   RETURN TEOL { $$ = grammar_new_return_node(NULL); }

assign      :   expression ASSIGN identifier TEOL {
                    $$ = grammar_new_assignment_expression($1, $3);
                }
            ;

identifier  :   ID { $$ = grammar_identifier($1); }
            ;

data_type   :   TYPE_FILE
            |   TYPE_NUMBER
            |   TYPE_STRING
            |   TYPE_BOOLEAN
            ;

bool_constant 
            :   TTRUE       { $$ = grammar_constant_bool($1); }
            |   TFALSE      { $$ = grammar_constant_bool($1); }
            ;

constant    :   NUMBER      { $$ = grammar_constant_number($1); }
            |   STRING      { $$ = grammar_constant_string($1); }
            ;

%%

