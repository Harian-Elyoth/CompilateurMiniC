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
#include "context.h"
#include "../common.h"

extern bool flag_global;

int32_t global_offset = 0;
int32_t var_globales_offset = 0;
int32_t global_strings_number = 0;
char ** global_string;


/*========================================================================================*/

/*FONCTION A APPELER LORS DE LA PASSE 1*/

void push_global_context()
{
	// A appeler avant l’analyse de la déclaration des variables globales. 
	// Initialise un contexte pour les variables globales et en fait le contexte courant.

	context_t global_context = create_context();//Création du context global
	env_actuel = malloc(sizeof(env_s));

	
	env_actuel->next = NULL;
	env_actuel->context = global_context;
	env_actuel->env_offset = 0;

	//Le but est, ici, d'initialiser les variables globales. Il faut donc pouvoir récupérer
	// les NODE_DECL du programme root ainsi que leur data pour les déclarer.
	//Pour les variables globales on va donc chercher dans le premier fils de program_root


}



void push_context()
{
	// A appeler avant l’analyse de la déclaration des variables d’un bloc. 
	// Initialise un contexte pour les variables locales et en fait le contexte courant.
	context_t new_context = create_context();
	env_t new_env = malloc(sizeof(env_s));
	new_env->context = new_context;
	new_env->next = env_actuel;
	env_actuel = new_env;
	env_actuel->env_offset = global_offset;

}

void pop_context()
{
	//printf("ON RENTRE DANS pop_context ! \n\n\n");
	// A appeler à la fin de l’analyse d’un bloc déclarant des variables. 
	// Cette fonction dépile et libère le contexte courant.
	env_t above_env = env_actuel->next;
	free_context(env_actuel->context);
	free(env_actuel);
	env_actuel = above_env;

}

int32_t env_add_element(char * ident, void * node, int32_t size)
{
		// Ajoute dans le contexte courant l’association entre le nom ident et le noeud node.
		// Le paramètre size définit la taille à allouer pour la variable
		// (en pile ou en section .data), et peut être mis toujours à 4.
		// Si la valeur retournée est positive ou nulle, il s’agit de l’offset de la variable dans l’environnement;
		// si la valeur retournée est négative, cela signifie qu’une variable du même nom existe déjà dans le contexte courant.

	void *node_value;
	node_t node_actuel = (node_t)node;

	if (flag_global)
	{
		var_globales_offset += 4;
	}

	if (node_actuel->opr[1] == NULL)
	{
		if (flag_global)
		{
			int intval_global = 0;
			int * intval_global_pt = &intval_global;
			node_value = (void *)(intval_global_pt);
		}
		else
		{
			node_actuel->opr[0]->is_ini = false;
			int intval_global = rand();
			int * intval_global_pt = &intval_global;
			node_value = (void *)(intval_global_pt);
		}
	}
	else
	{
		node_actuel->opr[0]->is_ini = true;
		if (node_actuel->opr[1]->nature == NODE_UMINUS)
		{
			node_t nd = (node_t)(node_actuel->opr[1]->opr[0]);
			int val = nd->value*-1;
			node_value = (int *)(&val);
		}
		else
		{
			node_value = (int *)(&((node_t)(node_actuel->opr[1]))->value);
		}
		//printf("valeur de %s = %d\n",ident,(*((int*)node_value)));
	}

	bool ret = context_add_element(env_actuel->context, (node_t)(node_actuel->opr[0]), ident, node_value);

	if(ret == false)
	{
		return -1;
	}
	else 
	{
		int offset_ret = global_offset - env_actuel->env_offset;
		global_offset += size;
		return offset_ret;
	}

}

void * get_decl_node(char * ident)
{
	// Retourne la définition de la variable ident rencontrée en premier 
	// dans l’empilement des contextes, en commençant par le contexte courant.
	env_t env_courant = env_actuel;
	int i = 0;
	while(env_courant != NULL){
		if(idf_in_context(env_courant->context, ident))
		{
			return get_node(env_courant->context, ident);
		}
		else 
		{
			env_courant = env_courant->next;
		}
		i++;
	}
	printf("ON RENVOIE NULL ICI\n");
	return NULL;
}

void reset_env_current_offset()
{
	// Réinitialise l’offset courant à 0; cette fonction doit
	// être appelée au début de l’analyse d’une fonction.
	env_actuel->env_offset = 0;
}

int32_t get_env_current_offset()
{
	// Retourne l’offset courant du contexte.
	return env_actuel->env_offset;
}

int32_t add_string(char * str)
{
	// Ajoute la déclaration en section .data d’une chaine de
	// caractère littérale et retourne l’offset correspondant.
	char ** tab_str = realloc(global_string, sizeof(char*));
	
    if(tab_str == NULL)
    {
        fprintf(stderr, "ALLOCATION MEMOIRE ADD STRING\n");
        exit(1);
    }
    else
    {
        global_string = tab_str;
    }

    global_string[global_strings_number] = str;

	int32_t res = var_globales_offset;
	var_globales_offset += strlen(str) - 1;

	return res;
		
}

/*===================================================================*/

/*FONCTIONS A APPELER LORS DE LA PASSE 2*/

int32_t get_global_strings_number()
{
	// Retourne le nombre de chaines de caractères littérales. 
	// Cette fonction devrait être utilisée pour la déclaration des chaines littérales 
	// en section .data.

	return global_strings_number;
}

char * get_global_string(int32_t index)
{
	// Retourne la chaine de caractère littérale d’index index, 
	// qui doit être strictement inférieur à la valeur retournée 
	// par get_global_strings_number(). 
	// Cette fonction devrait être utilisée pour la déclaration des chaines littérales 
	// en section .data.
	if (index < global_strings_number)
	{
		return global_string[index];
	}
	else
	{
		return NULL;
	}
	
}

void free_global_strings()
{
	// Libère la mémoire allouée pour les chaines littérales.
	// La valeur de retour des fonctions env_add_element() et add_string() 
	// devrait être stockée dans le champ offset des noeuds adéquats.
	free(global_string);
	//boucles sur le tableau global_string pour libérer chaque chaine
	//Puis liberer le tableau
}