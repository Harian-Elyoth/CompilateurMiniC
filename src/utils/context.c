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
	context_t context = malloc(sizeof(context_t));
	context->root = malloc(sizeof(noeud_s));
	noeud_t root = context->root;
	root->idf_existant = true;
	root->data = malloc(sizeof(int));
	root->lettre = '/';
	for(int i = 0 ; i < NB_ELEM_ALPHABET ; i++){
		root->suite_idf[i] = malloc(sizeof(noeud_s));
		root->suite_idf[i]->lettre = i + CODE_ASCII_A;
		root->suite_idf[i]->idf_existant = false;
	}
	return context;
}

void * get_data(context_t context, char * idf)
{
	void * data = malloc(sizeof(int));
	
	if(idf_in_context(context, idf) == false){
		printf("%s n'existe pas dans ce context\n",idf);
		return NULL;
	}
	else {
		
		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		int longueur_idf = strlen(idf);
		for(int i = 0 ; i < longueur_idf ; i ++){
			if(i == longueur_idf){
				data = noeud_actuel->data;
				if (data == NULL)
				{
					printf("DATA NON ALLOUE, FOR\n");
					printf("Le noeud actuel est la lettre : %c\n",noeud_actuel->lettre);
				}
				return data;
			}
			else
			{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			}
		}
		printf("on est la\n");
		data = noeud_actuel->data;
		if (data == NULL)
				{
					printf("DATA NON ALLOUE, END\n");
				}
		return data;
	}
}

void free_context(context_t context)
{
	free(context);
}

bool idf_in_context(context_t context, char * idf){

    noeud_t noeud_actuel = context->root;
    char * char_actuel = idf;
    int longueur_idf = strlen(idf);
    for(int i = 0 ; i < longueur_idf ; i++){
            //On pars du principe que l'allocation se fasse dans l'ordre
            //aka si on doit allouer un b on le fait a deuxieme case et non a la premiere.
            if((noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A])->idf_existant){// Si la lettre existe
				//On passe a la lettre suivante				
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
            }
            else {
                return false;
            }
	}
    return true;
}

bool context_add_element(context_t context, char * idf, void * data_argument){

	if(idf_in_context(context, idf)){
		return false;
	}
	else {
		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		//On passe directement a la premiere lettre de l'identifier
		noeud_actuel = noeud_actuel->suite_idf[char_actuel[0] - CODE_ASCII_A];
		noeud_actuel->lettre = char_actuel[0];
		noeud_actuel->idf_existant = true;
		int longueur_idf = strlen(idf);
		for(int i = 1 ; i < longueur_idf; i++){
			
			noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] = malloc(sizeof(noeud_s));
			noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			noeud_actuel->lettre = char_actuel[i];
			noeud_actuel->idf_existant = true;
			if(i == longueur_idf - 1){
				printf("On alloue la data : %d pour Bonjour\n", *((int *)data_argument));
				printf("Le noeud actuel est la lettre : %c\n",noeud_actuel->lettre);
				noeud_actuel->data = malloc(sizeof(int));
				noeud_actuel->data = (void *)data_argument;
			}
		}
	}

	return true;

}