#include "context.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    char * monchar = "a";
    char * malettre = "b";
    int monresultat = strcmp(monchar, malettre);
    printf("%d", monresultat);
    context_t moncontext = create_context();
    char * idf = "Bonjour";
    int data = 5;
    int * p_data = &data;
    context_add_element(moncontext, idf, p_data);
    return 0;
}
