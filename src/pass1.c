
void passe_1(node_t root){
    //Partons du principe que push global context met directement les variables
    //globales dans le context global et initialise les offset
    //On pourrait directement appeler pass_1 sur le root->opr[1] soit le NODE FUNC
    
    switch(root->nature)
    {
        char * error_msg;

        case NODE_PROGRAM :
            push_global_context();
            break;
        
        case NODE_IDENT : //plusieurs cas
            root->global_decl = flag_global; 
            root->type = type_actuel;
            if(flag_decl){ // declaration
                int offset_decl = env_add_element(root->ident, (node_t)root, 4) < 0)
                if(offset_decl < 0){
                    sprintf(error_msg, "La variable %s est déjà déclarée\n", root->ident);
                    yyerror(error_msg);
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
                        yyerror(error_msg);
                        error_in_program = true;
                    }
                }
                else if (root->decl_node == NULL){
                    sprintf(error_msg, "La variable %s n'a pas été déclarée précédemment !\n", root->ident);
                    yyerror(error_msg);
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
        case NODE_DIV : 
        case NODE_MOD : 
        case NODE_LT : 
        case NODE_LE : 
        case NODE_GT : 
        case NODE_GE : 
        case NODE_BAND : 
        case NODE_BOR : 
        case NODE_BXOR :
            if(((root->opr[0]->type) != TYPE_INT)||((root->opr[1]->type) != TYPE_INT))){
                sprintf(error_msg, "Des opérations sur des INT uniquement se font sur d'autres type !\n");
                yyerror(error_msg);
                error_in_program = true;
            }
            root->type = TYPE_INT;
            break;
        //operations entre BOOL
        case NODE_AND : 
        case NODE_OR : 
            if(((root->opr[0]->type) != TYPE_BOOL)||((root->opr[1]->type) != TYPE_BOOL))){
                sprintf(error_msg, "Des opérations sur des BOOL uniquement se font sur d'autres type !\n");
                yyerror(error_msg);
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
                yyerror(error_msg);
                error_in_program = true;
            }
            root->type = root->opr[0]

    }

    for(int i = 0 ; i < root->nops ; i++){
        passe_1(root->opr[i]);
    }

    if(root->nature = NODE_FUNC){
        root->stack_size = global_offset - env_actuel->env_offset;
    }
    // *Analyse des declarations des variables globales*



    }
}