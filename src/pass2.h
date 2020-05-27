#ifndef _PASS2_H_
#define _PASS2_H_

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

void passe_2(node_t root);
void action_decl(node_t root);
int32_t action_op(node_t root);
void gen_ope_r_code(node_t node, int32_t r_dest, int32_t r_source, int32_t r_source2);
void gen_ope_i_code(node_t node, int32_t dest, int32_t source, int32_t imm);
void load_ident(node_t node, int32_t dest);
void store_ident(node_t node, int32_t source, bool is_decl);
void action_loop(node_t root);
void action_print(node_t root);
int32_t action_uminus(node_t root);
int32_t action_plus_minus(node_t root);
#endif