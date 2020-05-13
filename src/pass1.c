#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

#include "defs.h"
#include "context.h"
#include "env.h"


void passe_1(node_t root){
    //Partons du principe que push global context met directement les variables
    //globales dans le context global et initialise les offset
    //On pourrait directement appeler pass_1 sur le root->opr[1] soit le NODE FUNC
    
    switch(root->nature)
    {
        case NODE_PROGRAM :
            push_global_context();
            passe_1(root->opr[1]);
            break;
        
        case NODE_IDENT : //plusieurs cas 

    }

    // *Analyse des declarations des variables globales*



    }
}