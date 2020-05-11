#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

#include "env.h"


/* Note : Une fois les fonctions completees, ne PAS supprimer les commentaires explicatifs !*/

/*========================================================================================*/

/*FONCTION A APPELER LORS DE LA PASSE 1*/

env_t push_global_context(node_t program_root)
{
	// A appeler avant l’analyse de la déclaration des variables globales. 
	// Initialise un contexte pour les variables globales et en fait le contexte courant.

	context_t global_context = create_context();//Création du context global
	env_t env_global = malloc(sizeof(env_s));
	env_global->context = global_context;
	//Le but est, ici, d'initialiser les variables globales. Il faut donc pouvoir récupérer
	// les NODE_DECL du programme root ainsi que leur data pour les déclarer.
	//Pour les variables globales on va donc chercher dans le premier fils de program_root
	add_from_root(global_context, program_root->opr[0]);
	return global_context;

}



env_t push_context(env_t env_actuel)
{
	// A appeler avant l’analyse de la déclaration des variables d’un bloc. 
	// Initialise un contexte pour les variables locales et en fait le contexte courant.
	context_t new_context = create_context();
	env_t new_env = malloc(sizeof(env_s));
	new_env->context = new_context;
	new_env->next = env_actuel;
	return new_env;
}

env_t pop_context(env_t env_actuel)
{
	// A appeler à la fin de l’analyse d’un bloc déclarant des variables. 
	// Cette fonction dépile et libère le contexte courant.
	env_t above_env = env_actuel->next;
	free_context(env_actuel->context);
	free(env_actuel);
	return above_env;
}

int32_t env_add_element(char * ident, void * node, int32_t size)
{
	// Ajoute dans le contexte courant l’association entre le nom ident et le noeud node. 
	// Le paramètre size définit la taille à allouer pour la variable 
	// (en pile ou en section .data), et peut être mis toujours à 4. 
	// Si la valeur retournée est positive ou nulle, il s’agit de l’offset de la variable dans l’environnement; 
	// si la valeur retournée est négative, cela signifie qu’une variable du même nom existe déjà dans le contexte courant.

	return 0;
}

void * get_decl_node(char * ident)
{
	// Retourne la définition de la variable ident rencontrée en premier 
	// dans l’empilement des contextes, en commençant par le contexte courant.

	return NULL;
}

void reset_env_current_offset()
{
	// Réinitialise l’offset courant à 0; cette fonction doit
	// être appelée au début de l’analyse d’une fonction.
}

int32_t get_env_current_offset()
{
	// Retourne l’offset courant du contexte.

	return 0;
}

int32_t add_string(char * str)
{
	// Ajoute la déclaration en section .data d’une chaine de
	// caractère littérale et retourne l’offset correspondant.	

	return 0;
}

/*===================================================================*/

/*FONCTIONS A APPELER LORS DE LA PASSE 2*/

int32_t get_global_strings_number()
{
	// Retourne le nombre de chaines de caractères littérales. 
	// Cette fonction devrait être utilisée pour la déclaration des chaines littérales 
	// en section .data.

	return 0;
}

char * get_global_string(int32_t index)
{
	// Retourne la chaine de caractère littérale d’index index, 
	// qui doit être strictement inférieur à la valeur retournée 
	// par get_global_strings_number(). 
	// Cette fonction devrait être utilisée pour la déclaration des chaines littérales 
	// en section .data.

	return NULL;
}

void free_global_strings()
{
	// Libère la mémoire allouée pour les chaines littérales.
	// La valeur de retour des fonctions env_add_element() et add_string() 
	// devrait être stockée dans le champ offset des noeuds adéquats.
}