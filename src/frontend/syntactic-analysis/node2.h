#ifndef NODE2_H
#define NODE2_H

#include <stdbool.h>

typedef struct constant_node constant_node;

typedef enum CONSTANT_NODE_TYPE {
        STRING_CONSTANT_NODE_TYPE,
        NUMBER_CONSTANT_NODE_TYPE,
        BOOLEAN_CONSTANT_NODE_TYPE,
        ID_CONSTANT_NODE_TYPE,
        RETURN_CONSTANT_NODE_TYPE,
        RETURN_NULL_CONSTANT_NODE_TYPE
} CONSTANT_NODE_TYPE;

typedef union variable_value {
        bool boolean;
        long long integer;
        double dFloat;
        char * string;
} variable_value;

struct constant_node {
        variable_value * value;
        char * my_id;
        CONSTANT_NODE_TYPE constant_node_type;
} constant_node;



#endif  /* NODE_H */