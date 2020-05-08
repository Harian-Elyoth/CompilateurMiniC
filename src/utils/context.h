
#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define CODE_ASCII_a 97
// 26 lower + 26 upper case letters, 10 figures, and '_'
#define NB_ELEM_ALPHABET 63

typedef struct _noeud_s {
   bool idf_existant;
   struct _noeud_s * suite_idf[NB_ELEM_ALPHABET];
   char lettre;
   void * data; // node_t corresponding to the variable decl
} noeud_s;

typedef noeud_s * noeud_t;


typedef struct _context_s {
    noeud_t root;
} context_s;

typedef context_s * context_t;


context_t create_context();
void free_context(context_t context);
bool context_add_element(context_t context, char * idf, void * data);
void * get_data(context_t context, char * idf);
bool idf_in_context(context_t context, char * idf);


bool idf_in_context(context_t context, char * idf){
    noeud_t noeud_actuel = context->root;
    char * char_actuel;
    * char_actuel = idf[0];
    int longueur_idf = strlen(idf);
    for(int i = 0 ; i < longueur_idf ; i++){
        if(noeud_actuel->idf_existant){
            //On pars du principe que l'allocation se fasse dans l'ordre
            //aka si on doit allouer un b on le fait a deuxieme case et non a la premiere.
            char * macomparaison;
            * macomparaison = noeud_actuel->suite_idf[*char_actuel - CODE_ASCII_a]->lettre;
            if(!strcmp(char_actuel, macomparaison)){// Si la lettre existe
                //On passe a la lettre suivante
                noeud_actuel = noeud_actuel->suite_idf[*char_actuel - CODE_ASCII_a];
                char_actuel++;
            }
            else {
                return false;
            }
        }
    }
    return true;
}

#endif

