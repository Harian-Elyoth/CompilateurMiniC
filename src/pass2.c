#include "pass2.h"

extern bool print_warning;
extern int32_t global_strings_number;
extern char ** global_string;
extern int32_t stack_size_decl;
extern int32_t var_globales_offset;
extern int nb_reg;
extern int trace_level;
extern node_t program_root;

bool flag_data = true;
bool reg_push_op = false;
bool reg_push_loop = false;
bool reg_push_uminus = false;
bool reg_push_plus_minus = false;

// le but de cette fonction est de générer un code assembleur sur le parcours de l'arbre.
void passe_2(node_t root){

    if (trace_level >= 3)
    {
        printf("Je rentre dans passe 2\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    set_max_registers(nb_reg);
    node_t node_actuel = root; 
    int32_t num_registre;

    //TRAITEMENT EN DESCENDANT DANS L'ARBRE

    switch(node_actuel->nature){
        
        case NODE_PROGRAM :

        	create_data_sec_inst();
        	break;

        case NODE_FUNC :
        	flag_data = false;

			for (int i = 0; i < global_strings_number; ++i)
			{	
				create_asciiz_inst(NULL, global_string[i]);
			}

			create_text_sec_inst();

			create_addiu_inst(29, 29, (node_actuel->stack_size)*(-1));

        	set_temporary_start_offset(stack_size_decl);
        	break;

        case NODE_DECL :
            action_decl(node_actuel);
        	break;

        case NODE_AFFECT :
            num_registre = action_op(node_actuel);
            store_ident(node_actuel->opr[0], num_registre, false);
            if (reg_push_op)
            {
                pop_temporary(num_registre);
                reg_push_op = false;
            }
            else if (reg_push_uminus)
            {
                pop_temporary(num_registre);
                reg_push_uminus = false;
            }
            else{release_reg();}
            break;

        case NODE_WHILE :
        case NODE_FOR :
        case NODE_IF :
        case NODE_DOWHILE :
            action_loop(node_actuel);
            break;
        case NODE_PRINT :
            action_print(node_actuel);

        default:
            break;
    }

	//APPELS RECURISF 
    switch(node_actuel->nature){
        case NODE_AFFECT :
        case NODE_DECL :
        case NODE_WHILE :
        case NODE_FOR :
        case NODE_IF :
        case NODE_DOWHILE :
        case NODE_PRINT :
            break;
        default : 
            for (int i = 0; i < node_actuel->nops; ++i)
            {
                if (node_actuel->opr[i] != NULL)
                {
                    passe_2(node_actuel->opr[i]);
                }
            }
            break;
    }

	//TRAITEMENT EN REMONTANT
	switch(node_actuel->nature)
	{
        case NODE_FUNC :
            create_ori_inst(2, 0, 10);
            create_syscall_inst();
            break;
	}

}


void action_decl(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans action_decl\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    int32_t num_registre;

    if (flag_data)
    {
        if (root->opr[1] != NULL)
        {
            if (root->opr[1]->nature == NODE_UMINUS)
            {
                create_word_inst(root->opr[0]->ident,(int32_t)((root->opr[1]->opr[0]->value)*-1));
            }
            else
            {
                create_word_inst(root->opr[0]->ident,(int32_t)(root->opr[1]->value));
            }
        }
        else
        {
            create_word_inst(root->opr[0]->ident,0);
        }
        
    }
    else if(root->opr[1] != NULL)
    {
        switch(root->opr[1]->nature)
        {
            case NODE_INTVAL :
            case NODE_BOOLVAL :
                create_ori_inst(8, 0, root->opr[1]->value);
                create_sw_inst(8, root->opr[0]->offset, 29);
                break;

            case NODE_IDENT :
                if (root->opr[1]->decl_node->global_decl)
                {
                    load_ident(root->opr[1], 8);
                    create_sw_inst(8, root->opr[0]->offset, 29);
                }
                break;
            case NODE_UMINUS :
            case NODE_NOT :
            case NODE_BNOT :

                num_registre = action_op_unaire(root->opr[1]);
                store_ident(root->opr[0], num_registre, true);

                if (reg_push_op)
                {
                    pop_temporary(num_registre);
                    reg_push_op = false;
                }
                else if (reg_push_uminus)
                {
                    pop_temporary(num_registre);
                    reg_push_uminus = false;
                }
                else{release_reg();}
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
                num_registre = action_op(root->opr[1]);
                store_ident(root->opr[0], num_registre, true);
                if (reg_push_op)
                {
                    pop_temporary(num_registre);
                    reg_push_op = false;
                }
                else{release_reg();}
                break;
            default:
                break;
        }   
    }
}

int32_t action_op(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans action_op\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    int32_t registre_courant_op1;
    int32_t registre_courant_op2;
    int32_t res_reg;


    int son_operator_flag = -1; 
    /*
    0 : Il n'a pas de fils opérateur
    1 : Son fils operateur est le premier fils (opr[0])
    2 : Sin fils operateur est le deuxieme fils (opr[1])
    */

    for(int i = 0 ; i < root->nops ; i++){
        
        switch(root->opr[i]->nature){

            case NODE_MINUS :
            case NODE_PLUS :
            case NODE_MUL :
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
            case NODE_MOD :    
            case NODE_DIV :
            case NODE_AND : 
            case NODE_OR :
            case NODE_EQ :
            case NODE_NE :
            case NODE_AFFECT :
                if (i == 0)
                {
                    son_operator_flag = i;
                    registre_courant_op1 = action_op(root->opr[i]);
                    if (reg_push_op)
                    {
                        pop_temporary(registre_courant_op1);
                        reg_push_op = false;
                    }
                    else{release_reg();}
                }
                else
                {
                    son_operator_flag = i;
                    registre_courant_op2 = action_op(root->opr[i]);
                    if (reg_push_op)
                    {
                        pop_temporary(registre_courant_op2);
                        reg_push_op = false;
                    }
                    else{release_reg();}
                }
                break;

            // case NODE_IDENT :
            //     if (i == 0)
            //     {
            //         load_ident(root->opr[i], 8);
            //         registre_courant_op1 = 8;
            //     }
            //     else
            //     {
            //         load_ident(root->opr[i], 8);
            //         registre_courant_op2 = 8;
            //     }
            //     break;

            default :
                break;
                
        }
    }
    
    if (reg_available())
    {
        allocate_reg();
        res_reg = get_current_reg();
    }
    else
    {
        reg_push_op = true;
        push_temporary(get_current_reg());
        allocate_reg();
        res_reg = get_current_reg();
    }

    
    //OP UNAIRE
    if (root->opr[1]->nature == NODE_UMINUS || root->opr[1]->nature == NODE_BNOT || root->opr[1]->nature == NODE_NOT)
    {   
        res_reg = action_op_unaire(root->opr[1]);
        return res_reg;
    }
        


    //printf("NOEUD ACTUEL (action_op) : %s\n\n\n", node_nature2string(root->nature));
    //printf("Son operator flag vaut : %d\n\n\n", son_operator_flag);
    if(son_operator_flag >= 0){
        if(son_operator_flag == 0){
            if(root->opr[1]->nature == NODE_IDENT)
            {
                load_ident(root->opr[1], 8);
                gen_ope_r_code(root, res_reg, 8, registre_courant_op1);
            }
            else if(root->opr[1]->nature == NODE_INTVAL || root->opr[1]->nature == NODE_BOOLVAL)
            {
                gen_ope_i_code(root, res_reg, registre_courant_op1, root->opr[1]->value);                    
            }
        }
        else if(son_operator_flag == 1)
        {
            gen_ope_r_code(root, res_reg, registre_courant_op1, registre_courant_op2);
        }
    }

    else if (root->opr[0]->nature == NODE_INTVAL || root->opr[0]->nature == NODE_BOOLVAL)
    {
        if ((root->opr[1]->nature == NODE_INTVAL || root->opr[1]->nature == NODE_BOOLVAL))
        {
            //printf("MES DEUX FILS SONT DES LITERRAUX \n\n");
            create_ori_inst(8, 0, root->opr[0]->value);
            gen_ope_i_code(root, res_reg, 8, (int32_t)root->opr[1]->value);

        }
        else if (root->opr[1]->nature == NODE_IDENT)
        {
            //printf("UN FILS LITTERAL ET UN IDENT\n");
            load_ident(root->opr[1], 8);
            gen_ope_i_code(root, res_reg, 8, root->opr[0]->value);
        }
        //op unaire
        else
        {
            gen_ope_i_code(root, res_reg, res_reg, root->opr[0]->value);
        }
    }
    else if (root->opr[0]->nature == NODE_IDENT)
    {
        if ((root->opr[1]->nature == NODE_INTVAL) || (root->opr[1]->nature == NODE_BOOLVAL))
        {
            //printf("UN FILS IDENT ET UN LITTERAL\n");

            if(root->nature != NODE_AFFECT)
            {
                load_ident(root->opr[0], 8);
                gen_ope_i_code(root, res_reg, 8, root->opr[1]->value);
            }
            else
            {
                gen_ope_i_code(root, res_reg, 8, root->opr[1]->value);
            }
        }
        else if (root->opr[1]->nature == NODE_IDENT)
        {
            //printf("DEUX FILS IDENT\n");
            load_ident(root->opr[0], 8);
            if (reg_available())
            {
                allocate_reg();
                int32_t source2_reg = get_current_reg();
                load_ident(root->opr[1], source2_reg);
                gen_ope_r_code(root, res_reg, 8, source2_reg);
                release_reg();
            }
            else
            {
                push_temporary(get_current_reg());
                allocate_reg();
                int32_t source2_reg = get_current_reg();
                load_ident(root->opr[1], source2_reg);
                gen_ope_r_code(root, res_reg, 8, source2_reg);
                pop_temporary(source2_reg);
            }
        }
    }
    return res_reg;
}

void gen_ope_r_code(node_t node, int32_t r_dest, int32_t r_source, int32_t r_source2){
    
    if (trace_level == 4)
    {
        printf("Je rentre dans gen_ope_r_code\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(node->nature));
    }

    int32_t new_reg;
    int32_t new_label_nb;
    int32_t new_label_nb_2;
    
    switch(node->nature){
            case NODE_MUL :
                create_mult_inst(r_source, r_source2);
                create_mflo_inst(r_dest);
            break;
            case NODE_MINUS :
                create_subu_inst(r_dest, r_source, r_source2);
                break;
            case NODE_LT : 
                create_sltu_inst(r_dest, r_source, r_source2);
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_bne_inst(r_dest, 0, new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_label_inst(new_label_nb_2);
                break;
            case NODE_LE : 
                create_sltu_inst(r_dest, r_source2, r_source);
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_bne_inst(r_dest, 0, new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_label_inst(new_label_nb_2);

                break;
            case NODE_GT : 
                create_sltu_inst(r_dest, r_source2, r_source);
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_bne_inst(r_dest, 0, new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_label_inst(new_label_nb_2);
                break;

            case NODE_GE : 
                create_sltu_inst(r_dest, r_source, r_source2);
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_bne_inst(r_dest, 0, new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_label_inst(new_label_nb_2);
                break;

            case NODE_BAND : 
                create_and_inst(r_dest, r_source, r_source2);
                break;
            case NODE_BOR : 
                create_or_inst(r_dest, r_source, r_source2);
                break;
            case NODE_BXOR :
                create_xor_inst(r_dest, r_source, r_source2);
                break;
            case NODE_SLL :
                create_sllv_inst(r_dest, r_source, r_source2);
                break;
            case NODE_SRL :
                create_srlv_inst(r_dest, r_source, r_source2);
                break;
            case NODE_SRA :
                create_srav_inst(r_dest, r_source, r_source2);
                break;
            case NODE_PLUS :
                create_addu_inst(r_dest, r_source, r_source2);
                break;
            case NODE_MOD :
                create_teq_inst(r_source2, 0);    
                create_div_inst(r_source, r_source2);
                create_mfhi_inst(r_dest);
                break;
            case NODE_DIV :
                create_teq_inst(r_source2, 0);
                create_div_inst(r_source, r_source2);
                create_mflo_inst(r_dest);
                break;
            case NODE_AND : 
                create_and_inst(r_dest, r_source, r_source2);
                break;
            case NODE_OR :
                create_or_inst(r_dest, r_source, r_source2);
                break;
            case NODE_EQ :
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_beq_inst(r_source, r_source2, new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_label_inst(new_label_nb_2);
                break;
            case NODE_NE :
                new_label_nb = get_new_label();
                new_label_nb_2 = get_new_label();
                create_bne_inst(r_source, r_source2, new_label_nb);
                create_ori_inst(r_dest, 0, 0);
                create_j_inst(new_label_nb_2);
                create_label_inst(new_label_nb);
                create_ori_inst(r_dest, 0, 1);
                create_label_inst(new_label_nb_2);
                break;
            case NODE_AFFECT :
                r_dest = r_source2;
                //create_addu_inst(r_dest, 0, r_source2);
                break;
    }
}

void gen_ope_i_code(node_t node, int32_t dest, int32_t source, int32_t imm)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans gen_ope_i_code\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(node->nature));
    }

    int32_t new_reg;
    int32_t new_label_nb;
    int32_t new_label_nb_2;

    switch(node->nature){

        case NODE_LT : 
            create_sltiu_inst(dest, source, imm);
            new_label_nb = get_new_label();
            new_label_nb_2 = get_new_label();
            create_bne_inst(dest, 0, new_label_nb);
            create_ori_inst(dest, 0, 0);
            create_j_inst(new_label_nb_2);
            create_label_inst(new_label_nb);
            create_ori_inst(dest, 0, 1);
            create_label_inst(new_label_nb_2);
            break;

        case NODE_GE : 
            create_sltiu_inst(dest, source, imm);
            new_label_nb = get_new_label();
            new_label_nb_2 = get_new_label();
            create_bne_inst(dest, 0, new_label_nb);
            create_ori_inst(dest, 0, 1);
            create_j_inst(new_label_nb_2);
            create_label_inst(new_label_nb);
            create_ori_inst(dest, 0, 0);
            create_label_inst(new_label_nb_2);
            break;

        case NODE_BXOR :
            create_xori_inst(dest, source, imm);
            break;
        case NODE_PLUS :
            create_addiu_inst(dest, source, imm);
            break;
        case NODE_AND :
            create_andi_inst(dest, source, imm);
            break; 
        case NODE_OR :
            create_ori_inst(dest, source, imm);
            break;
        case NODE_EQ :
        case NODE_NE :
        case NODE_SLL :
        case NODE_SRL :
        case NODE_SRA :
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_MOD :    
        case NODE_DIV :
        case NODE_GT : 
        case NODE_LE :
        case NODE_MUL :
        case NODE_MINUS :
            if (reg_available())
            {
                allocate_reg();
                new_reg = get_current_reg();
                create_ori_inst(new_reg, 0, imm);
                gen_ope_r_code(node, dest, source, new_reg);
                release_reg();
            }
            else
            {
                push_temporary(get_current_reg());
                allocate_reg();
                new_reg = get_current_reg();
                create_ori_inst(new_reg, 0, imm);
                gen_ope_r_code(node, dest, source, new_reg);
                pop_temporary(new_reg);
            }
            break;

        case NODE_AFFECT :
            create_ori_inst(dest, 0, imm);
            break;

        default:
            break;
    }
}

void load_ident(node_t root, int32_t dest)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans load_ident\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    if(root->decl_node->global_decl){
            create_lui_inst(dest, data_start);
            create_lw_inst(dest, root->decl_node->offset, dest);
    }
    else {
        if(root->nature == NODE_IDENT){
            create_lw_inst(dest, root->decl_node->offset, stack_reg);
        }
    }
}

void store_ident(node_t root, int32_t source, bool is_decl)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans store_ident\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    if (is_decl)
    {
        if(root->global_decl)
        {
            create_lui_inst(8, data_start);
            create_sw_inst(source, root->offset, 8);
        }
        else {
            create_sw_inst(source, root->offset, stack_reg);
        }
    }
    else
    {
        if(root->decl_node->global_decl)
        {
            create_lui_inst(8, data_start);
            create_sw_inst(source, root->decl_node->offset, 8);
        }
        else {
            create_sw_inst(source, root->decl_node->offset, stack_reg);
        }
    }
}

void action_loop(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans action_loop\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    int32_t num_registre;
    int32_t true_register;
    int32_t label_loop = get_new_label();
    int32_t label_suite = get_new_label();

    for (int i = 0; i < root->nops; ++i)
    {
        if (root->nature == NODE_FOR && i == 2)
        {
            continue;
        }
        switch(root->opr[i]->nature)
        {
            case NODE_AFFECT :

                num_registre = action_op(root->opr[i]);
                store_ident(root->opr[i]->opr[0], num_registre, false);
                create_label_inst(label_loop);
                break;

            case NODE_IDENT :
                load_ident(root->opr[i], 8);
                num_registre = 8;
                break;

            case NODE_EQ :
            case NODE_NE :
            case NODE_LT :
            case NODE_LE :
            case NODE_GT :
            case NODE_GE :
            case NODE_AND :
            case NODE_OR :

                num_registre = action_op(root->opr[i]);
                break;

            case NODE_BNOT :
            case NODE_NOT :
                num_registre = action_op_unaire(root->opr[i]);

            case NODE_BLOCK :

                if (root->nature == NODE_IF)
                {
                    create_bne_inst(num_registre, 0, label_suite);
                    passe_2(root->opr[i]);
                    create_label_inst(label_suite);
                }
                else if (root->nature == NODE_WHILE) 
                {
                    create_label_inst(label_loop);
                    create_beq_inst(num_registre, 0, label_suite);
                    passe_2(root->opr[i]);
                    create_j_inst(label_loop);
                    create_label_inst(label_suite);

                }
                else if (root->nature == NODE_FOR)
                {
                    
                    create_beq_inst(num_registre, 0, label_suite);
                    passe_2(root->opr[i]);
                    num_registre = action_op(root->opr[2]);
                    store_ident(root->opr[2]->opr[0], num_registre, false);
                    create_j_inst(label_loop);
                    create_label_inst(label_suite);

                }
                else if (root->nature == NODE_DOWHILE)
                {
                    create_label_inst(label_loop);
                    passe_2(root->opr[i]);
                    num_registre = action_op(root->opr[1]);
                    create_beq_inst(num_registre, 0, label_loop);
                }
                break;
        } 
    }
}

void action_print(node_t root)
{   
    if (trace_level == 4)
    {
        printf("Je rentre dans action_print\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    for (int i = 0; i < root->nops; ++i)
    {
        switch(root->opr[i]->nature)
        {
            case NODE_STRINGVAL :
                create_lui_inst(4, data_start);
                //printf("offset de la string : %d\n", root->opr[i]->offset);
                create_ori_inst(4, 4, root->opr[i]->offset);
                create_ori_inst(2, 0, 4);
                create_syscall_inst();
                break;

            case NODE_LIST :
                action_print(root->opr[i]);
                break;

            case NODE_IDENT :
                load_ident(root->opr[i], 4);
                create_ori_inst(2, 0, 1);
                create_syscall_inst();
                break;
            default :
                break;
        }
    }
}

int32_t action_op_unaire(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans action_op_unaire\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    //ON ALLOUE UN REG
    int32_t res_reg;

    if (reg_available())
    {
        allocate_reg();
        res_reg = get_current_reg();
    }
    else
    {
        reg_push_uminus = true;
        push_temporary(get_current_reg());
        allocate_reg();
        res_reg = get_current_reg();
    }

    if (root->nature == NODE_BNOT || root->nature == NODE_NOT)
    {
        switch(root->opr[0]->nature)
        {
            case NODE_BOOLVAL :
                create_ori_inst(res_reg, 0, root->opr[0]->value);
                create_nor_inst(res_reg, 0, res_reg);
                break;

            case NODE_IDENT :
                load_ident(root->opr[0], res_reg);
                create_nor_inst(res_reg, 0, res_reg);
                break;

            default :
                res_reg = action_op(root->opr[0]);
                break;
        }
    }
    else if(root->nature == NODE_UMINUS)
    {
        switch(root->opr[0]->nature)
        {
            case NODE_IDENT :
                load_ident(root->opr[0], res_reg);
                create_subu_inst(res_reg, 0, res_reg);
                break;

            case NODE_INTVAL :
                create_ori_inst(res_reg, 0, root->opr[0]->value);
                create_subu_inst(res_reg, 0, res_reg);
                break;

            case NODE_PLUS :
            case NODE_MINUS :
            case NODE_BAND : 
            case NODE_BOR : 
            case NODE_BXOR :
            case NODE_SLL :
            case NODE_SRL :
            case NODE_SRA :
            case NODE_MUL :
            case NODE_MOD :    
            case NODE_DIV :
                res_reg = action_op(root->opr[0]);
                create_subu_inst(res_reg, 0, res_reg);
                break;

            default :
                break;
        }
    }
    return res_reg;
}

