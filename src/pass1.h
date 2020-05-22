#ifndef _PASS1_H_
#define _PASS1_H_


#include "defs.h"
#include "utils/context.h"
#include "utils/env.h"
#include "y.tab.h"
#include "common.h"
#include "utils/registers.h"
#include "utils/mips_inst.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

void passe_1(node_t root);
void test_boucle(node_t root);
void actions_node_ident(node_t root);
void actions_node_decl(node_t root);
void test_op(node_t root);
void test_op_type(node_t root, int type);
void test_op_cond(node_t root);

#endif