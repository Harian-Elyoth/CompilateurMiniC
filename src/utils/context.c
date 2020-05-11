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
	root->idf_existant = true;
	root->lettre = '/';
	return context;
}

// Retourne le noeud précédemment associé à idf dans context, 
// ou null si idf n’existe pas dans context.

void * get_data(context_t context, char * idf)
{
	void * data;

	if(idf_in_context(context, idf) == false){
		printf("%s n'existe pas dans ce context\n",idf);
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

				if (data == NULL)
				{
					printf("DATA NON ALLOUE, FOR\n");
					
				}
				return data;
			}
			else
			{
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			}
		}
		data = noeud_actuel->data;
		if (data == NULL)
				{
					printf("DATA NON ALLOUE, END\n");
				}
		return data;
	}
}

// Libère la mémoire allouée pour context.

void free_context(context_t context)
{
	printf("On rentre dans free_context\n");
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
	printf("lettre actuelle : %c\n", noeud->lettre);
	if (noeud->data != NULL)
	{
		printf("on free la data : %d\n", *((int *)noeud->data));
		//free((int *)noeud->data);
		printf("le free data marche !\n");
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
    for(int i = 0 ; i < longueur_idf ; i++){

            // On pars du principe que l'allocation se fasse dans l'ordre
            // si on doit allouer un b on le fait a deuxieme case et non a la premiere.

            if (noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] != NULL)
            {
            	if((noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A])->idf_existant)// Si la lettre existe
            	{
				//On passe a la lettre suivante				
				noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
            	}
            	else {
                	return false;
            	}
            }
            else
            {
            	return false;
            }
	}
    return true;
}

// Ajoutel’association entre le nom idf et le noeud data dans le contexte context. 
// Si le nom idf est déjà présent, l’ajout échoue et la fonction retourne false. 
// Sinon, la fonction retourne true.

bool context_add_element(context_t context, char * idf, void * data_argument){

	if(idf_in_context(context, idf)){
		return false;
	}
	else {

		noeud_t noeud_actuel = context->root;
		char * char_actuel = idf;
		int longueur_idf = strlen(idf);

		for(int i = 0 ; i < longueur_idf; i++){
			
			noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A] = malloc(sizeof(noeud_s));
			noeud_actuel = noeud_actuel->suite_idf[char_actuel[i] - CODE_ASCII_A];
			noeud_actuel->lettre = char_actuel[i];
			noeud_actuel->idf_existant = true;

			if(i == longueur_idf - 1){
				noeud_actuel->data = malloc(sizeof(int));
				noeud_actuel->data = (void *)data_argument;
			}
		}
	}
	return true;
}

void add_global_from_root(context_t context, node_t root){
	/*Cette fonction fais un DFS sur l'arbre en partant du noeud root, si il trouve un identifier il l'ajoute dans 
	mon_ident et si il trouve une value il l'ajoute dans ma data. Vu que c'est un DFS il ajoutera 
	l'ident et la value d'une meme variable. */
	node_t node_actuel = root;
	int * ident_flag; //flag pour savoir si la variable mon_ident a bien été initialisé.
	int * data_flag; //flag pour savoir si la variable ma_data a été initialisé
	* ident_flag = 0;
	* data_flag = 0;
	char * mon_ident;
	void * ma_data;
	int length = root->nops;
	for(int i = 0 ; i < length ; i++){
		if(root == NODE_PROGRAM){
			length = 1; //On veut que les variables globales.
		}
		if(root->opr[i] == NODE_IDENT){
			mon_ident = root->opr[i]->ident;
			ident_flag = 1; 
		}
		else if(root->opr[i] == NODE_INTVAL || root->opr[i] == NODE_BOOLVAL){
			ma_data = root->opr[i]->value; //A modifier (un concept de pointeur sur void ici)
		}

		add_from_root(context, root->opr[i]);
		//En depilant, si c'est un NODE_DECL on l'ajoute a notre context.
		if(root == NODE_DECL){
			if(data_flag == 0){
				//la variable n'a pas été initialisé elle vaut donc 0;
				* ma_data = 0; //a revoir
			}
			context_add_element(context, mon_ident, ma_data);
		}
	}


}