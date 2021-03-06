#include "pass1.h"

extern bool print_warning;
extern int32_t global_strings_number;
extern char ** global_string;
extern int trace_level;
extern node_t program_root;

bool flag_global = true;            //flag variable globales
bool flag_decl = false;             // flag de declaration
node_type type_actuel = TYPE_NONE;  // flag type courant pour la variable déclarée
bool error_in_program = false;      // flag qui indique une erreur dans le prog
int32_t stack_size_decl = 0;

void passe_1(node_t root){

    if (trace_level >= 3)
    {
        printf("Je rentre dans passe 1\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }
    
    //TRAITEMENT EFFECTUE EN DESCENDANT DANS L'ARBRE (DFS)
    char error_msg[100];

    switch(root->nature)
    {
        case NODE_PROGRAM :
            push_global_context();
            break;
        
        case NODE_IDENT :
            actions_node_ident(root);
            break;

        case NODE_DECLS :
            if (root->opr[0]->type == TYPE_VOID)
            {
                sprintf(error_msg, "On ne peut pas déclarer de variable de type VOID !\n");
                fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            break;

        case NODE_UMINUS :
            actions_uminus(root);
            break;

        case NODE_DECL :
            actions_node_decl(root);
            break;

        case NODE_TYPE : 
            type_actuel = root->type;
            break;

        case NODE_BLOCK :
            push_context();
            break;

        case NODE_FUNC :
            reset_temporary_max_offset();
            flag_global = false;
            root->offset = global_offset;
            break;

        case NODE_INTVAL : 
            root->type = TYPE_INT;
            break;

        case NODE_BOOLVAL : 
            root->type = TYPE_BOOL;
            break;

        case NODE_STRINGVAL : 
        {
            root->offset = add_string(root->str);
            root->type = TYPE_STRING;
            global_strings_number++;
            break;
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
        case NODE_BNOT :
        case NODE_NOT :
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
            test_op(root);
            break;

        default:
            break;
    }

    //APPELS RECUSRVIFS DE PASS1

    switch(root->nature)
    {
        case NODE_MUL :
        case NODE_MINUS :
        case NODE_UMINUS :
        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_BAND : 
        case NODE_BXOR :
        case NODE_BNOT :
        case NODE_NOT :
        case NODE_BOR : 
        case NODE_PLUS :
        case NODE_MOD :    
        case NODE_DIV :
        case NODE_AND : 
        case NODE_OR :
        case NODE_EQ :
        case NODE_NE :
        case NODE_AFFECT :
        {
            break;
        }
        default:
        {
            for(int i = 0 ; i < root->nops ; i++){

                if (root->opr[i] != NULL)
                {
                    passe_1(root->opr[i]);
                }  
            }
            break;
        }
    }

    //TRAITEMENT EFFECTUE EN REMONTANT L'ARBRE (DFS)

    switch(root->nature)
    {
        case NODE_UMINUS :
        {
            if (root->opr[0]->type != TYPE_INT)
            {
                sprintf(error_msg, "Un opérateur sur des INT est utilsé sur d'autre type !\n");
                fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            root->type = TYPE_INT;
            break;
        }
        case NODE_FUNC :
        {
            root->stack_size = global_offset - root->offset;
            stack_size_decl = root->stack_size;
            root->stack_size += get_temporary_max_offset();
            break;
        }
        case NODE_IF :
        case NODE_FOR :
        case NODE_WHILE :
        case NODE_DOWHILE :
        {
            test_boucle(root);
            break;
        }
        case NODE_BLOCK :
        {
            pop_context();
            break;
        }
        case NODE_PROGRAM :
            pop_context();
            break;
        default:
            break;
    }

}


void test_boucle(node_t root)
{

    if (trace_level == 4)
    {
        printf("Je rentre dans test_boucle\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    char error_msg[100];

    switch(root->opr[0]->nature)
    {
        case NODE_IDENT :
        case NODE_MUL :
        case NODE_MINUS :
        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
        case NODE_PLUS :
        case NODE_MOD :    
        case NODE_DIV :
        case NODE_AND : 
        case NODE_OR :
        case NODE_EQ :
        case NODE_NE :
        case NODE_AFFECT :
        {
            if (root->opr[0]->type != TYPE_BOOL)
            {
                sprintf(error_msg, "L'expression conditionnel ne renvoie pas un booleen !\n");
                fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            break;
        }
    }

}



void actions_node_ident(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans actions_node_ident\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    char error_msg[100];
    bool is_main = false;
    //printf("je suis dans NODE_IDENT\n");
    root->global_decl = flag_global; 
    root->type = type_actuel;
    if(!(flag_decl))
    {
        //printf("je suis dans une utilisation\n");

        if(strcmp(root->ident, "main") == 0)
        {
            is_main = true;
        }
        if (!(is_main))
        {    
            root->decl_node = (node_t)get_decl_node(root->ident);

            if (root->decl_node == NULL)
            {
                root->type = TYPE_NONE;  
                error_in_program = true;      
                sprintf(error_msg, "La variable %s n'a pas été déclarée précédemment !\n", root->ident);
                fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
                exit(1);
            }
            else
            {
                root->type = (root->decl_node)->type;
                if (root->is_ini)
                {
                    if (print_warning)
                    {
                        error_in_program = true;      
                        sprintf(error_msg, "La variable %s n'a pas été initialisée !\n", root->ident);
                        fprintf(stderr, "Warning line %d: %s\n", program_root->lineno, error_msg);
                    }
                }
            }
        }
        else
        {
            if(root->type != TYPE_VOID)
            {
                sprintf(error_msg, "le main n'est pas de type void !\n");
                fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
        }
    }
    else{flag_decl = false;}
}

void actions_node_decl(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans actions_node_decl\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    char error_msg[100];
    flag_decl = true;

    //printf("Je suis dans une declaration\n");
    int offset_decl = env_add_element(root->opr[0]->ident, root, 4);

    if(offset_decl < 0){
        sprintf(error_msg, "La variable %s est déjà déclarée\n", root->opr[0]->ident);
        fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
        exit(1);
        error_in_program = true;
    }
    else {
        root->opr[0]->offset = offset_decl;
        root->opr[0]->decl_node = NULL;    
    }
}

void test_op(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans test_op\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    switch(root->nature)
    {
        case NODE_MUL :
        case NODE_MINUS :
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
        case NODE_SLL :
        case NODE_SRL :
        case NODE_SRA :
        case NODE_PLUS :
        case NODE_MOD :    
        case NODE_DIV :
        {
            test_op_type(root, 0);
        }
        break;

        case NODE_AND : 
        case NODE_OR :
        {
            test_op_type(root, 1);
        }
        break;

        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_EQ :
        case NODE_NE :
        case NODE_AFFECT :
        {
            test_op_cond(root);
        }
        break;

        case NODE_BNOT :
            test_op_type(root, 0);
            break;

        case NODE_NOT :
            test_op_type(root, 1);
            break;

        default:
            break;
    }
}

void test_op_type(node_t root, int type)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans test_op_type\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    int is_allocate = 0;
    int is_push = 0;

    if (reg_available())
    {
        allocate_reg();
        is_allocate = 1;
        if (reg_available())
        {
            allocate_reg();
            is_allocate = 2;
        }
        else
        {
            push_temporary_virtual();
            is_push = 1;
        }
    }
    else
    {
        is_push = 2;
        push_temporary_virtual();
    }

    char error_msg[100];
    node_type type_op = TYPE_NONE;

    if (type == 0)
    {
        type_op = TYPE_INT;
    }
    else
    {
        type_op = TYPE_BOOL;
    }

    for (int i = 0; i < root->nops; ++i)
    {
        //printf("tour de boucle : %d\n", i);
        if (root->opr[i]->nature != NODE_INTVAL && root->opr[i]->nature != NODE_BOOLVAL)
        {
            passe_1(root->opr[i]);
        }
    }

    if(root->nops==2){
        if(((root->opr[0]->type) != type_op) || ((root->opr[1]->type) != type_op))
        {
            sprintf(error_msg, "Des opérations sur des %s uniquement se font sur d'autres type !\n", node_type2string(type_op));
            fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
            exit(1);
            error_in_program = true;
        }
    }
    else{
        if ((root->opr[0]->type) != type_op)
        {
            sprintf(error_msg, "Des opérations sur des %s uniquement se font sur d'autres type !\n", node_type2string(type_op));
            fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
            exit(1);
            error_in_program = true;
        }
    }


    if (root->nature == NODE_MOD || root->nature == NODE_DIV)
    {
        if((root->opr[1]->value) == 0){

            if (print_warning)
            {
                sprintf(error_msg, "Division par 0 ou Modulo 0 !\n");
                fprintf(stderr, "Warning line %d: %s\n", program_root->lineno, error_msg);
            }
            
            error_in_program = true;
        }
    }


    root->type = type_op;

    if (is_allocate == 1)
    {
        release_reg();
    }
    else if (is_allocate == 2)
    {
        release_reg();
        release_reg();
    }
    if (is_push == 1)
    {
        pop_temporary_virtual();
    }
    else if (is_push == 2)
    {
        pop_temporary_virtual();
        pop_temporary_virtual();
    }

}

void test_op_cond(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans test_op_cond\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    int is_allocate = 0;
    int is_push = 0;

    if (reg_available())
    {
        allocate_reg();
        is_allocate = 1;
        if (reg_available())
        {
            allocate_reg();
            is_allocate = 2;
        }
        else
        {
            push_temporary_virtual();
            is_push = 1;
        }
    }
    else
    {
        is_push = 2;
        push_temporary_virtual();
    }

    char error_msg[100];

    for (int i = 0; i < root->nops; ++i)
    {
        passe_1(root->opr[i]);
    }

    if((root->opr[0]->type != root->opr[1]->type)){
        sprintf(error_msg, "Des opérations se font entre un INT et un BOOL ! \n");
        fprintf(stderr, "Error line %d: %s\n", program_root->lineno, error_msg);
        exit(1);
        error_in_program = true;
    }
    root->type = TYPE_BOOL;
    
    if (is_allocate == 1)
    {
        release_reg();
    }
    else if (is_allocate == 2)
    {
        release_reg();
        release_reg();
    }
    if (is_push == 1)
    {
        pop_temporary_virtual();
    }
    else if (is_push == 2)
    {
        pop_temporary_virtual();
        pop_temporary_virtual();
    }
}

void actions_uminus(node_t root)
{
    if (trace_level == 4)
    {
        printf("Je rentre dans actions_uminus\n");
        printf("avec un node de nature : %s\n\n", node_nature2string(root->nature));
    }

    switch(root->opr[0]->nature)
    {
        case NODE_MUL :
        case NODE_MINUS :
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
        case NODE_SLL :
        case NODE_SRL :
        case NODE_SRA :
        case NODE_PLUS :
        case NODE_MOD :    
        case NODE_DIV :
            passe_1(root->opr[0]);
            break;
    }
}