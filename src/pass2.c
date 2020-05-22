#include "pass2.h"

extern bool print_warning;
extern int32_t global_strings_number;
extern char ** global_string;

bool flag_data = true;

void passe_2(node_t root){
    // le but de cette fonction est de générer un code assembleur sur le parcours de l'arbre.
    
    node_t node_actuel = root; 

    //TRAITEMENT EN DESCENDANT DANS L'ARBRE

    switch(node_actuel->nature){
        
        case NODE_PROGRAM :
        	if (node_actuel->opr[0] != NULL)
        	{
        		create_data_sec_inst();
        	}
        	break;
        case NODE_IDENT :
        	if (flag_data)
        	{
        		create_word_inst(node_actuel->ident,(int32_t)node_actuel->value);
        	}
        	//d'autre trucs
        	break;

        case NODE_FUNC :
        	flag_data = false;

			for (int i = 0; i < global_strings_number; ++i)
			{	
				char label[10];
				sprintf(label,"_ch%d",i);
				create_asciiz_inst(label, global_string[i]);
			}

			create_text_sec_inst();

			create_addiu_inst(29, 29, (node_actuel->stack_size)*(-1));

        	set_temporary_start_offset(node_actuel->stack_size);
        	break;

        case NODE_DECL :
        	break;


    }

	//APPELS RECURISF 

	///

	//TRAITEMENT EN REMONTANT

	switch(node_actuel->nature)
	{

	}

}