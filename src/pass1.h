#ifndef _PASS1_H_
#define _PASS1_H_


#include "defs.h"
#include "utils/context.h"
#include "env.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>


bool flag_global = true; //flag qui nous indique si nous faisons des declarations de variable globales
bool flag_decl = false; 
node_type type_actuel = TYPE_NONE;
bool error_in_program = false;
void passe_1(node_t root);

#endif