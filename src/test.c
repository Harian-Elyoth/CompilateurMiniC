#include "context.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char const *argv[])
{

    /* TEST DE CONTEXT.C */

    context_t moncontext = create_context();
    char * idf = "bonjour";
    int data = 5;
    int * p_data = &data;
    bool result = context_add_element(moncontext, idf, p_data);
    printf("Create_add_element renvoie %d\n", result);
    
    idf_in_context(moncontext, idf);

    int *p_int = (int *)get_data(moncontext, idf);

    if (p_int == NULL)
    {
        printf("ERROR GET DATA\n");
        return 0;
    }

    printf("ma data vaut %d\n", *p_int);

    //free_context(moncontext);
    

    /* TEST DE ENV.C */
    //Test pratiquement impossible parcequ'il faudrait utiliser la fonction yyparse
    
    node_t program_root = malloc(sizeof(node_s));
    yyparse(program_root);
    program_root->nature = NODE_PROGRAM;
    program_root->nops = 2;

    printf("1\n");
    env_t env_actuel = push_global_context(program_root);
    printf("2\n");
    env_actuel = push_context(env_actuel);
    printf("3\n");
    env_actuel = pop_context(env_actuel);


    return 0;
}
