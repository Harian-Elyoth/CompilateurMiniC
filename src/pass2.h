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

#endif