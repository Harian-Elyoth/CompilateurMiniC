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

// Alloue un objet de type context_t et le retourne

context_t create_context()
{
	context_t context = malloc(sizeof(context_t));
	context->root = malloc(sizeof(noeud_s));
	noeud_t root = context->root;
	root->data = malloc(sizeof(int));
	root->data = NULL;
	root->idf_existant = true;
	root->lettre = '/';
	for(int i = 0 ; i < NB_ELEM_ALPHABET ; i++){
		root->suite_idf[i] = NULL;
	}
	return context;
}

// Retourne le noeud précédemment associé à idf dans context, 
// ou null si idf n’existe pas dans context.

void * get_data(context_t context, char * idf)
{
	void * data;

	if(idf_in_context(context, idf) == false){
		return NULL;
	}
	else {
		
		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		int longueur_idf = strlen(idf);
		//permet de passer le noeud racine
		noeud_actuel = noeud_actuel->suite_idf[char_actuel[0] - CODE_ASCII_A];

		for(int i = 1 ; i <= longueur_idf ; i ++){

			if(i == longueur_idf){
				
				data = noeud_actuel->data;

				return data;
			}
			else
			{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			}
		}
		data = noeud_actuel->data;
		return data;
	}
}

void * get_node(context_t context, char *idf){
	
	void * node;

	//printf("je rentre dans get_node\n");

	if(idf_in_context(context, idf) == false){
		return NULL;
	}
	else {
		
		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		int longueur_idf = strlen(idf);
		//permet de passer le noeud racine
		noeud_actuel = noeud_actuel->suite_idf[char_actuel[0] - CODE_ASCII_A];

		for(int i = 1 ; i <= longueur_idf ; i ++){

			if(i == longueur_idf){
				
				node = noeud_actuel->node;
				return node;
			}
			else
			{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			}
		}
		node = noeud_actuel->node;
		return node;
	}
}

// Libère la mémoire allouée pour context.

void free_context(context_t context)
{
	//printf("On rentre dans free_context\n");
	noeud_t noeud_actuel = context->root;

	free_noeud(noeud_actuel);
	free(context);
}

//Fonction récursive
//Free tout les noeuds herité de son parametre noeud
//Ne fonctionne pas encore bien !
//Ne free pas le champ data du noeud ..

void free_noeud(noeud_t noeud)
{
	//printf("On rentre dans free_noeud\n");
	//printf("lettre actuelle : %c\n", noeud->lettre);
	if (noeud->data != NULL)
	{
		//printf("on free la data : %d\n", *((int *)noeud->data));
		//free((int *)noeud->data);
		//printf("le free data marche pas !\n");
	}
	for (int i = 0; i < NB_ELEM_ALPHABET; ++i)
	{
		if (noeud->suite_idf[i] != NULL)
		{
			free_noeud(noeud->suite_idf[i]);
		}
	}

	free(noeud);

}

// Verifie si un idf est present dans un contexte
// Renvoie true si l'idf existe et false sinon.

bool idf_in_context(context_t context, char * idf){

    noeud_t noeud_actuel = context->root;
    char * char_actuel = idf;
    int longueur_idf = strlen(idf);

	//printf("Je rentre dans IDF in context\n\n\n");
    for(int i = 0 ; i < longueur_idf ; i++){

			//printf("char actuel = %c\n",char_actuel[i]);

			if(noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] == NULL){
				//printf("IDF IN CONTEXT RETURN FALSE\n");
				return false;
			}
			else{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
				if(i == longueur_idf - 1){
					//printf("IDF IN CONTEXT RETURN %d\n", noeud_actuel->idf_existant);
					return noeud_actuel->idf_existant;
				}
			}

	}
	return true;
}

// Ajoutel’association entre le nom idf et le noeud data dans le contexte context. 
// Si le nom idf est déjà présent, l’ajout échoue et la fonction retourne false. 
// Sinon, la fonction retourne true.

bool context_add_element(context_t context, node_t node, char * idf, void * data_argument){

	if(idf_in_context(context, idf)){
		return false;
	}
	else {
		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		int longueur_idf = strlen(idf);

		for(int i = 0 ; i < longueur_idf; i++)
		{

			//printf("char_actuel : %c\n",char_actuel[i]);
			if (noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] == NULL)
			{
				noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] = malloc(sizeof(noeud_s));
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
				for (int i = 0; i < NB_ELEM_ALPHABET; ++i)
				{
					noeud_actuel->suite_idf[i] = NULL;
				}
			}
			else
			{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			}
			
			noeud_actuel->lettre = char_actuel[i];

			if(i == longueur_idf - 1){
				noeud_actuel->data = malloc(sizeof(int));
				noeud_actuel->data = (void *)data_argument;
				noeud_actuel->node = node;
				noeud_actuel->idf_existant = true;
			}
		}
	}
	return true;
}