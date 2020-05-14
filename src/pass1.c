#include "pass1.h"

extern bool print_warning;

bool flag_global = true; //flag variable globales
bool flag_decl = false;  // flag de declaration
node_type type_actuel = TYPE_NONE; // flag type courant pour la variable déclarée
bool error_in_program = false; // flag qui indique une erreur dans le prog

void passe_1(node_t root){

    switch(root->nature)
    {
        char * error_msg;
        printf("je rentre dans pass1\n\n\n\n");
        case NODE_PROGRAM :
            push_global_context();
            break;
        
        case NODE_IDENT : //plusieurs cas
            root->global_decl = flag_global; 
            root->type = type_actuel;
            if(flag_decl){ // declaration
                int offset_decl = env_add_element(root->ident, (node_t)root, 4);
                if(offset_decl < 0){
                    sprintf(error_msg, "La variable %s est déjà déclarée\n", root->ident);
                    fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                    exit(1);
                    error_in_program = true;
                }
                else {
                    root->offset = offset_decl;
                    root->decl_node = NULL;    
                }
            }
            else { //utilisation
                root->decl_node = (node_t)get_decl_node(root->ident);
                if(!strcmp(root->ident, "main")){
                    if(root->type != TYPE_VOID){
                        sprintf(error_msg, "le main n'est pas de type void !\n");
                        fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                        exit(1);
                        error_in_program = true;
                    }
                }
                if (root->decl_node == NULL){

                    if (print_warning)
                    {
                        sprintf(error_msg, "La variable %s n'a pas été déclarée précédemment !\n", root->ident);
                        fprintf(stderr, "Warning line %d: %s\n", root->lineno, error_msg);
                    }
        
                    error_in_program = true;
                }
            }
            break;
        case NODE_DECL :
            flag_decl = true;
            break;
        case NODE_TYPE : 
            type_actuel = root->type;
            break;
        case NODE_BLOCK :
            push_context();
            break;
        case NODE_FUNC :
            flag_global = false;
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
        case NODE_PLUS :
        case NODE_MUL :
        case NODE_MINUS :
        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
            if(((root->opr[0]->type) != TYPE_INT) || ((root->opr[1]->type) != TYPE_INT)){
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
            if((root->opr[0]->type != root->opr[1]->type)){
                sprintf(error_msg, "Des opérations se font entre un INT et un BOOL ! \n");
                fprintf(stderr, "Error line %d: %s\n", root->lineno, error_msg);
                exit(1);
                error_in_program = true;
            }
            root->type = root->opr[0]->type;
        default:
            break;

    }

    printf("Je sors du switch de pass1\n");

    for(int i = 0 ; i < root->nops ; i++){

        const char * nature = node_nature2string(root->nature); 
        printf("node actuel (dans for) : %s\n", nature);

        if (root->opr[i] != NULL)
        {
            printf("Je passe au node suivant\n");
            passe_1(root->opr[i]);
        }
        
    }

    const char * nature = node_nature2string(root->nature); 
    printf("node actuel (apres for) : %s\n", nature);

    if(root->nature = NODE_FUNC){
        root->stack_size = global_offset - env_actuel->env_offset;
    }

}