#include "context.h"
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

    free_context(moncontext);


    /* TEST DE ENV.C */

    

    return 0;
}
