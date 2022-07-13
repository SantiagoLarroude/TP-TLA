#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "logger.h"
#include "mem_management.h" /* free_* functions */

#include "symbols.h"

#define BLOCKSIZE 64
#define THRESHOLD 0.75
#define SCOPE_DANGLING -1

typedef unsigned long long hash_t;

typedef struct node {
        struct node *next;
        hash_t prehash;
        variable *var;
        long scope;
} node;

static unsigned current_scope;
static unsigned num_blocks;
static unsigned num_dangling;
static unsigned table_size;

static node **table;

static hash_t hash(const char *name);
static variable *lookup_variable_in_scope(const char *name, long scope);
static node *lookup_node_in_scope(const char *variable_name, long scope);
static void insert_std_functions();
static node *free_table_node(node *table_node);

bool initialize_table()
{
        current_scope = 0;
        num_blocks = 1;
        table = calloc(BLOCKSIZE, sizeof(node *));
        if (table == NULL) {
                error_no_memory();
                return false;
        }

        insert_std_functions();

        return true;
}

static hash_t hash(const char *name)
{
        // djb2
        // http://www.cse.yorku.ca/~oz/hash.html
        hash_t hash = 5381;

        if (name == NULL)
                return hash;

        for (unsigned i = 0; name[i] != '\0'; i++)
                hash = ((hash << 5) + hash) + name[i]; /* hash * 33 + c */

        return hash;
}

void next_scope()
{
        LogDebug("%s()\nCurrent scope: %d.", __func__, current_scope);
        current_scope++;
}

void prev_scope()
{
        LogDebug("%s()\nCurrent scope: %d.", __func__, current_scope);
        node *current;

        for (int i = 0; i < table_size; i++) {
                if (table[i] == NULL)
                        continue;

                current = table[i];
                while (current != NULL && current->scope == current_scope)
                        current = current->next;

                table[i] = current;
        }

        if (current_scope > 0)
                current_scope--;
}

bool is_dangling(variable *var)
{
        LogDebug("%s(%p)", __func__, var);

        if (var == NULL)
                return false;

        if (lookup_variable_in_scope(var->name, SCOPE_DANGLING) == NULL)
                return false;

        return true;
}

bool assign_scope_to_dangling_variable(variable *var)
{
        LogDebug("%s(%p)", __func__, var);
        if (is_dangling(var) == false)
                return true;

        node *table_node = lookup_node_in_scope(var->name, SCOPE_DANGLING);
        if (table_node == NULL)
                return false;

        table_node->scope = current_scope;
        num_dangling--;

        return true;
}
unsigned count_dangling()
{
        LogDebug("%s()", __func__);
        return num_dangling;
}

int insert_variable(variable *var)
{
        LogDebug("%s(%p)\nCurrent scope: %d.", __func__, var, current_scope);

        hash_t prehash = hash(var->name);
        unsigned key_index = prehash % (num_blocks * BLOCKSIZE);
        node *table_node = table[key_index];

        while (table_node != NULL && table_node->prehash != prehash)
                table_node = table_node->next;

        if (table_node == NULL || table_node->scope != current_scope) {
                table_node = (node *)calloc(1, sizeof(node));
                table_node->next = table[key_index];
                table_node->prehash = prehash;
                table_node->var = var;
                table_node->scope = current_scope;
                table[key_index] = table_node;

                table_size++;
        } else {
                return MULTIPLE_DECLARATION;
        }

        if ((table_size * 1.0) / (num_blocks * BLOCKSIZE) >= THRESHOLD) {
                num_blocks++;
                table = realloc(table,
                                sizeof(node *) * num_blocks * BLOCKSIZE);
                memset(table + table_size, 0,
                       (num_blocks * BLOCKSIZE - table_size) * sizeof(node *));
        }

        return SUCCESS;
}

int insert_dangling_variable(variable *var)
{
        LogDebug("%s(%p)\nCurrent scope: %d.", __func__, var, current_scope);
        int ret_val = insert_variable(var);
        if (ret_val == SUCCESS) {
                node *table_node =
                        lookup_node_in_scope(var->name, current_scope);
                table_node->scope = SCOPE_DANGLING;
                num_dangling++;
        }

        return ret_val;
}

void insert_function(char *name) // type de lo que vaya a devolver
{
        variable *fun = (variable *)calloc(1, sizeof(variable));
        if (fun == NULL) {
                error_no_memory();
                exit(1);
        }

        fun->name = strdup(name);
        fun->type = FUNCTION_TYPE;

        insert_variable(fun);
}

static void insert_std_functions()
{
        insert_function("columns");
        insert_function("lines");
        insert_function("byIndex");
        insert_function("filter");
        insert_function("toString");
        insert_function("at");
}

variable *lookup_variable(const char *name)
{
        LogDebug("%s(%s)\nCurrent scope: %d.", __func__, name, current_scope);
        variable *aux = NULL;

        for (long scope = current_scope; scope >= 0 && aux == NULL; scope--)
                aux = lookup_variable_in_scope(name, scope);

        return aux;
}

variable *lookup_dangling_variable(const char *name)
{
        LogDebug("%s(%s)\nCurrent scope: %d.", __func__, name, current_scope);

        return lookup_variable_in_scope(name, SCOPE_DANGLING);
}

static variable *lookup_variable_in_scope(const char *name, long scope)
{
        LogDebug("%s(%s, %ld)\nCurrent scope: %d.", __func__, name, scope,
                 current_scope);

        node *node = lookup_node_in_scope(name, scope);
        if (node == NULL)
                return NULL;

        return node->var;
}

static node *lookup_node_in_scope(const char *variable_name, long scope)
{
        LogDebug("%s(%s, %ld)\nCurrent scope: %d.", __func__, variable_name,
                 scope, current_scope);
        unsigned long long pre = hash(variable_name);
        unsigned key_index = pre % (num_blocks * BLOCKSIZE);
        node *node = table[key_index];

        while (node != NULL && (node->prehash != pre || node->scope != scope))
                node = node->next;

        if (node == NULL)
                return NULL;

        return node;
}

void free_table()
{
        LogDebug("%s()", __func__);

        if (table == NULL)
                return;

        for (size_t i = 0; i < num_blocks * BLOCKSIZE; i++) {
                node *table_node = table[i];
                while (table_node != NULL) {
                        table_node = free_table_node(table_node);
                }
        }

        free_and_keep_address(table);
}

static node *free_table_node(node *table_node)
{
        if (table_node == NULL)
                return NULL;

        node *next_node = table_node->next;
        free_variable(table_node->var);
        free_and_keep_address(table_node);

        return next_node;
}
