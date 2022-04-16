%{

#include "bison-actions.h"

/* Top level root node in the AST */
node_block* programblock = NULL;

%}

%union {
    struct node_block* block;
    struct node_expression* expr;
    struct node_statement* stmt;
    struct node_identifier* id;

    char* string;

    token_t token;

    token_t void;

    FILE* file;
}
/* esta chequeado esto????????????????????? */
/* %define api.value.type {union YYSTYPE} */


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
%type <expr>    expression
                list id_list
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
            ;

expression  : expression ASSIGN identifier { 
                    $$ = grammar_expression_assignment($1, $3); 
                }
            |   expression ASSIGN OPEN_BRACKETS id_list CLOSE_BRACKETS {
                    $$ = grammar_expression_assignment_list($1, $4);
                }
            |   expression AND expression {
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
            |   OPEN_BRACKETS list CLOSE_BRACKETS {}
            |   OPEN_PARENTHESIS expression CLOSE_PARENTHESIS { 
                    $$ = $2;        /* TODO: Test this. (!) */
                }
            |   constant
            ;

 loop        :   EACH COLUMN CONTAINS expression {}
             |   EACH ROW CONTAINS expression {}
             ;

 if          :   IF expression THEN { /* do stuff when this rule is encountered */ }
             |   ELSE IF expression { /* tengo que hacer algo. Ej: expression */ }
             |   ELSE expression { expression | loop | comparison | num_arithm | str_arithm | if } 
             /* esta bien asi? xq lit que puedo tener cualquier cosa dentro del if */
             /* ese {} lo pondria en los 3 casos */
             ;


file_param          :   FILE_TYPE
                    |   STRING

comparison  :   EQUALS | NOT_EQUALS
            |   GREATER_THAN | GREATER_EQUAL | LESS_THAN | LESS_EQUAL
            ;

num_arithm  :   ADD | SUB
            |   MUL | DIV 
            |   MOD
            ;

str_arithm  :   STR_ADD | STR_SUB
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
