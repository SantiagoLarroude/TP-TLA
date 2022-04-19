#ifndef NODE_H
#define NODE_H

typedef struct node {
        //
} node;

typedef struct node_command {
        // 
} node_command;

typedef struct node_variable_declaration {
        token_t type;
        struct node_identifier* id;
        struct node_expression* assignment_expr;
} node_variable_declaration;


typedef struct node_binary_operator {
        //
} node_binary_operator;

typedef struct node_assignment {
        struct node_identifier* rhs;
        struct node_expression* lhs;
} node_assignment;

typedef struct node_identifier {
        size_t length;
        char* name;
} node_identifier;

typedef struct node_block {
        // 
        struct node_expression* exp;
} node_block;

typedef struct node_expression {
        size_t length;
        char* value;

        union {
                token_t token;
                token_t type;
        };
} node_expression;



#endif  /* NODE_H */