#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

#include "context.h"


context_t create_context()
{
	printf("Je creer un context !\n");
	context_t context = malloc(sizeof(context_t));
	return context;
}

void * get_data(context_t context, char * idf)
{

}

void free_context(context_t context)
{
	free(context);
}