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
        	//pour les utilisation
        	//d'autre trucs
        	break;

        case NODE_FUNC :
        	flag_data = false;

			for (int i = 0; i < global_strings_number; ++i)
			{	
				char * label = malloc(sizeof(char)*10);
				sprintf(label,"_ch%d",i);
				create_asciiz_inst(label, global_string[i]);
			}

			create_text_sec_inst();

			create_addiu_inst(29, 29, (node_actuel->stack_size)*(-1));

        	set_temporary_start_offset(node_actuel->stack_size);
        	break;

        case NODE_DECL :
            action_decl(node_actuel);
        	break;

        case NODE_MUL :
        case NODE_MINUS :
        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
        case NODE_SLL :
        case NODE_SRL :
        case NODE_SRA :
        case NODE_PLUS :
        case NODE_MOD :    
        case NODE_DIV :
        case NODE_AND : 
        case NODE_OR :
        case NODE_EQ :
        case NODE_NE :
        case NODE_AFFECT :
            action_op(node_actuel);
            break;
        
        default:
            break;


    }

	//APPELS RECURISF 

    for (int i = 0; i < node_actuel->nops; ++i)
    {
        if (node_actuel->opr[i] != NULL)
        {
            passe_2(node_actuel->opr[i]);
        }
    }

	//TRAITEMENT EN REMONTANT

	// switch(node_actuel->nature)
	// {

	// }

}


void action_decl(node_t root)
{
    if (flag_data)
    {
        create_word_inst(root->opr[0]->ident,(int32_t)(root->opr[1]->value));
    }
    else
    {
        switch(root->opr[1]->nature)
        {
            case NODE_INTVAL :
            case NODE_BOOLVAL :
                create_ori_inst(8, 0, root->opr[1]->value);
                create_sw_inst(8, root->opr[0]->offset, 29);
                break;

            case NODE_IDENT :
                if (root->opr[1]->global_decl)
                {
                    //create_lui_inst(8, int32_t imm); // load adress ??
                    create_lw_inst(8, root->decl_node->offset, 8);
                    create_sw_inst(8, root->offset, 29);
                }
            case NODE_MUL :
            case NODE_MINUS :
            case NODE_LT : 
            case NODE_LE : 
            case NODE_GT : 
            case NODE_GE : 
            case NODE_BAND : 
            case NODE_BOR : 
            case NODE_BXOR :
            case NODE_SLL :
            case NODE_SRL :
            case NODE_SRA :
            case NODE_PLUS :
            case NODE_MOD :    
            case NODE_DIV :
            case NODE_AND : 
            case NODE_OR :
            case NODE_EQ :
            case NODE_NE :
            case NODE_AFFECT :

                //Appeler une fonction qui traite les operateurs
                break;
        }
        
    }
}

int32_t action_op(node_t root)
{
    // for (int i = 0; i < root->nops; ++i)
    // {
    //     if (root->opr[i] != NULL)
    //     {
    //         action_op(root->opr[i]);
    //     }
    // }
    // if (root->opr[0]->nature == NODE_OP) || root->opr[1]->nature == NODE_OP)
    // {
    //     action_op()
    // }

    int32_t res_reg;

    if (root->opr[0]->nature == NODE_INTVAL || root->opr[0]->nature == NODE_BOOLVAL)
    {
        if (root->opr[1]->nature == NODE_INTVAL || root->opr[1]->nature == NODE_BOOLVAL)
        {
            if (reg_available())
            {
                allocate_reg();
                res_reg = get_current_reg();
                create_addiu_inst(res_reg, 0, root->opr[0]->value);
                create_addiu_inst(res_reg, res_reg, root->opr[1]->value);
            }
        }
        else if (root->opr[1]->nature == NODE_IDENT)
        {
            if (reg_available())
            {
                allocate_reg();
                res_reg = get_current_reg();
                //la res_reg label(=root->opr[1]->ident)
                create_addiu_inst(res_reg, res_reg, root->opr[0]->value);
            }
        }
    }
    else if (root->opr[0]->nature == NODE_IDENT)
    {
        if (root->opr[1]->nature == NODE_INTVAL || root->opr[1]->nature == NODE_BOOLVAL)
        {
            if (reg_available())
            {
                allocate_reg();
                res_reg = get_current_reg();
                //la res_reg label(=root->opr[0]->ident)
                create_addiu_inst(res_reg, res_reg, root->opr[1]->value);
            }
        }
        else if (root->opr[1]->nature == NODE_IDENT)
        {
            if (reg_available())
            {
                allocate_reg();
                res_reg = get_current_reg();
                //la res_reg label(=root->opr[0]->ident)
                if (reg_available())
                {
                    allocate_reg();
                    int32_t temp_reg = get_current_reg();
                    create_addu_inst(res_reg, res_reg, temp_reg);
                    release_reg();
                }
            }
        }
    }

    return res_reg;
}