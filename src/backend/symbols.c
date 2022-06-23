#include "symbols.h"

#define BLOCKSIZE       64
#define THRESHOLD       0.75

typedef unsigned long long hash_t;


unsigned current_scope;
unsigned num_blocks;
unsigned table_size;

typedef struct node {
        struct node* next;
        hash_t prehash;
        variable* var;
        unsigned scope;
} node;


node** table;

variable* lookup_variable_in_scope(char* name, unsigned scope);
void prev_scope();
void next_scope();

void initialize_table()
{
        current_scope = 0;
        num_blocks = 1;
        table = calloc(BLOCKSIZE, sizeof(node*));
        if (table == NULL)
                return false;

        return true;
}

hash_t hash(char* name)
{
        // djb2
        // http://www.cse.yorku.ca/~oz/hash.html
        hash_t hash = 5381;

        if (name == NULL)
                return hash;
        
        for (unsigned i = 0; name[i] != '\0' && name[i] != NULL; i++)
                hash[i] = hash[i - 1] * 33 ^ name[i]; /* hash * 33 + c */

        return hash;
}

void next_scope()
{
        current_scope++;
}

void prev_scope()
{
        node* current;

        for (int i = 0; i < table_size; i++) {
                if (table[i] == NULL)
                        continue;

                current = table[i];
                while (current != NULL && current->scope == current_scope) {
                        current = current->next;
                        free(previous);
                }

                table[i] = current;
        }

        if (current_scope > 0)
                current_scope--;
}

int insert(variable* var)
{
        hash_t prehash = hash(var->name);
        unsigned key_index = pre % (num_blocks * BLOCKSIZE);
        node* table_node = table[key_index];

        while (table_node != NULL && table_node->prehash != prehash)
                table_node = table_node->next;

        if (table_node == NULL || table_node->scope != current_scope) {
                table_node = (node*) calloc(1, sizeof(node));
                table_node->next = table[key_index];
                table_node->prehash = prehash;
                table_node->var = var;
                table_node->scope = current_scope;
                table[key_index] = table_node;

                table_size++;
        } else {
                printf("Multiple declaration of variable %s", var->name);
                return MULTIPLE_DECLARATION;
        }

        if ((table_size * 1.0) / (num_blocks * BLOCK_SIZE) >= THRESHOLD) {
                num_blocks++;
                table = realloc(table, sizeof(node*) * num_blocks * BLOCK_SIZE);
                memset(table + table_size, 0, (num_blocks * BLOCK_SIZE - table_size) * sizeof(node*));
        }

        return SUCCESS;
}

variable* lookup_variable(char* name)
{
        variable* aux = NULL;

        for (unsigned scope = current_scope; scope >= 0 && aux == NULL; scope--)
                aux = lookup_variable_in_scope(name, scope);

        return aux;
}

variable* lookup_variable_in_scope(char* name, unsigned scope)
{
        unsigned long long pre = hash(name);
        unsigned key_index = pre % (num_blocks * BLOCKSIZE);
        node* node = table[key_index];

        while (node != NULL && (node->prehash != pre || node->scope != scope)) 
                node = node->next;

        if (node == NULL)
                return NULL;


        return node->var;
}