#ifndef NODE_H
#define NODE_H

typedef struct node_expression {
        size_t length;
        char* value;

        union {
                token_t token;
                token_t type;
        };
} node_expression;

typedef struct node_statement {
        // 
} node_statement;

typedef struct node_identifier {
        //
} node_identifier;

typedef struct node_block {
        // 
        struct node_expression* exp;
} node_block;

#endif  /* NODE_H */