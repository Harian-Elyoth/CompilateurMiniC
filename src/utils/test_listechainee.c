#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{

    context_t moncontext = create_context();
    char * idf = "Bonjour";
    int data = 5;
    int * p_data = &data;
    bool result = context_add_element(moncontext, idf, p_data);
    printf("Create_add_element renvoie %d\n", result);

    int * p_int = (int *)malloc(sizeof(int));
    
    if (p_int == NULL)
    {
        printf("ERROR AFFECTATION\n");
        return 0;
    }

    bool estExistant = idf_in_context(moncontext, idf);
    printf("\nBonjour existe dans mon context : %d\n", estExistant);

    p_int = (int *)get_data(moncontext, idf);

    if (p_int == NULL)
    {
        printf("ERROR GET DATA\n");
        return 0;
    }

    printf("Yo !!\n\n\n");
    printf("mon pointeur vaut %p", p_int);

    int deref = * p_int;

    printf("Bonjour !!\n\n\n");
    printf("ma data vaut %d", deref);

    return 0;
}
