#include "context.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{

    context_t moncontext = create_context();
    char * idf = "Bonjour";
    int data = 5;
    int * p_data = &data;
    bool result = context_add_element(moncontext, idf, p_data);
    printf("Create_add_element renvoie %d\n", result);
    int * p_int = (int *)get_data(moncontext, idf);
    int deref = * p_int;
    printf("Bonjour !!\n\n\n");
    printf("mon pointeur vaut %d", deref);
    return 0;
}
