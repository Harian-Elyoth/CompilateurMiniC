
#ifndef _ENV_H_
#define _ENV_H_


#include "context.h"
#include "defs.h"


typedef struct _env_s {
    struct _env_s * next;
    context_t context;
} env_s;

typedef env_s * env_t;



env_t push_global_context();
env_t push_context();
env_t pop_context();
void * get_decl_node(char * ident);
int32_t env_add_element(char * ident, void * node, int32_t size);
void reset_env_current_offset();
int32_t get_env_current_offset();
int32_t add_string(char * str);
int32_t get_global_strings_number();
char * get_global_string(int32_t index);
void free_global_strings();

#endif

