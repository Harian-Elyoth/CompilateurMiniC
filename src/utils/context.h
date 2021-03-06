
#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../defs.h"

//int global_offset;

#define CODE_ASCII_A 65
// 26 upper case letters 10 figures, and '_' 
#define NB_ELEM_ALPHABET 63

typedef struct _noeud_s {
   bool idf_existant;
   struct _noeud_s * suite_idf[NB_ELEM_ALPHABET];
   char lettre;
   void * data; // node_t corresponding to the variable decl
   node_t node;
} noeud_s;

typedef noeud_s * noeud_t;


typedef struct _context_s {
    noeud_t root;
} context_s;

typedef context_s * context_t;


context_t create_context();
void free_context(context_t context);
void free_noeud(noeud_t noeud);
bool context_add_element(context_t context, node_t node, char * idf, void * data);
void * get_data(context_t context, char * idf);
void * get_node(context_t context, char *idf);
bool idf_in_context(context_t context, char * idf);
//void add_global_from_root(context_t context, node_t root);


#endif

