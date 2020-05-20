#include "pass1.h"

extern bool print_warning;

bool flag_global = true; //flag variable globales
bool flag_decl = false;  // flag de declaration
node_type type_actuel = TYPE_NONE; // flag type courant pour la variable déclarée
bool error_in_program = false; // flag qui indique une erreur dans le prog

void passe_1(node_t root){

    switch(root->nature)
    {
        char error_msg[100];// = malloc(sizeof(char)*100);
        printf("je rentre dans pass1\n\n");
        case NODE_PROGRAM :
            push_global_context();
            break;
        
        case NODE_IDENT : //plusieurs cas
            printf("je suis dans NODE_IDENT\n");
            root->global_decl = flag_global; 
            root->type = type_actuel;
            if(!(flag_decl))
            {
                printf("je suis dans une utilisation\n");

                if(!strcmp(root->ident, "main")){
                    if(root->type != TYPE_VOID){
                        sprintf(error_msg, "le main n'est pas de type void !\n");
                        fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                        exit(1);
                        error_in_program = true;
                    }
                }
                else
                {    
                    root->decl_node = (node_t)get_decl_node(root->ident);
                }
                if (root->decl_node == NULL)
                {
                    if (print_warning)
                    {                        
                        sprintf(error_msg, "La variable %s n'a pas été déclarée précédemment !\n", root->ident);
                        fprintf(stderr, "Warning line %d: %s\n", root->lineno, error_msg);
                    }
                    error_in_program = true;
                }
                else
                {
                    root->type = (root->decl_node)->type;
                }
            }
            else
            {
                flag_decl = false;
            }
            break;

        case NODE_DECL :
            flag_decl = true;
            printf("Je suis dans une declaration\n");
                int offset_decl = env_add_element(root->opr[0]->ident, (node_t)root->opr[0], 4);

                if(offset_decl < 0){
                    sprintf(error_msg, "La variable %s est déjà déclarée\n", root->opr[0]->ident);
                    fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                    exit(1);
                    error_in_program = true;
                }
                else {
                    printf("on attribut l'offset\n");
                    root->opr[0]->offset = offset_decl;
                    root->opr[0]->decl_node = NULL;    
                }
            break;
        case NODE_TYPE : 
            type_actuel = root->type;
            break;
        case NODE_BLOCK :
            push_context();
            break;
        case NODE_FUNC :
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
            root->type = TYPE_STRING;
            root->offset = add_string(root->str);
        //operations entre INT
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

            for (int i = 0; i < root->nops; ++i)
            {
                passe_1(root->opr[i]);
            }
            
            if(((root->opr[0]->type) != TYPE_INT) || ((root->opr[1]->type) != TYPE_INT))
            {
                sprintf(error_msg, "Des opérations sur des INT uniquement se font sur d'autres type !\n");
                fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            root->type = TYPE_INT;

            
            break;

        // Ici, on gere en plus la div par 0 ou le modulo 0    
        case NODE_MOD :    
        case NODE_DIV :

            for (int i = 0; i < root->nops; ++i)
            {
                passe_1(root->opr[i]);
            }

            if(((root->opr[0]->type) != TYPE_INT) || ((root->opr[1]->type) != TYPE_INT)){

                sprintf(error_msg, "Des opérations sur des INT uniquement se font sur d'autres type !\n");
                fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            if((root->opr[1]->value) == 0){

                if (print_warning)
                {
                    sprintf(error_msg, "Division par 0 ou Modulo 0 !\n");
                    fprintf(stderr, "Warning line %d: %s\n", root->lineno, error_msg);
                }
                
                error_in_program = true;
            }
            root->type = TYPE_INT;
            break;
        //operations entre BOOL
        case NODE_AND : 
        case NODE_OR :

            for (int i = 0; i < root->nops; ++i)
            {
                passe_1(root->opr[i]);
            } 

            if(((root->opr[0]->type) != TYPE_BOOL)||((root->opr[1]->type) != TYPE_BOOL)){
                sprintf(error_msg, "Des opérations sur des BOOL uniquement se font sur d'autres type !\n");
                fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            root->type = TYPE_BOOL;

            break;
        //operations sur les deux
        case NODE_EQ :
        case NODE_NE :
        case NODE_AFFECT :

            for (int i = 0; i < root->nops; ++i)
            {
                passe_1(root->opr[i]);
            }

            if((root->opr[0]->type != root->opr[1]->type)){
                sprintf(error_msg, "Des opérations se font entre un INT et un BOOL ! \n");
                fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            root->type = root->opr[0]->type;
            break;

        case NODE_IF:

            break;
        default:
            break;

    }

    switch(root->nature)
    {
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
            break;
        }
        default:
        {
            for(int i = 0 ; i < root->nops ; i++){
                const char * nature = node_nature2string(root->nature); 
                printf("node actuel : %s\n\n", nature);

                if (root->opr[i] != NULL)
                {
                    passe_1(root->opr[i]);
                }  
            }
            break;
        }
    }

    if(root->nature == NODE_FUNC){
        root->stack_size = global_offset - env_actuel->env_offset;
    }

}