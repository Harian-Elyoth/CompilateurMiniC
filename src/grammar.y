%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "defs.h"
#include "common.h"
#include "mips_inst.h"



/* Global variables */
/* A completer */
extern bool stop_after_syntax;
extern bool stop_after_verif;
extern char * infile;
extern char * outfile;


/* prototypes */
int yylex(void);
extern int yylineno;

void yyerror(node_t * program_root, char * s);
void analyse_tree(node_t root);
node_t make_node(node_nature nature, int nops, ...);
/* A completer */

%}

%parse-param { node_t * program_root }

%union {
    int32_t intval;
    char * strval;
    node_t ptr;
};


/* Definir les token ici avec leur associativite, dans le bon ordre */
%token TOK_VOID TOK_INT TOK_INTVAL TOK_BOOL TOK_TRUE TOK_FALSE
%token TOK_IDENT TOK_IF TOK_ELSE TOK_WHILE  TOK_FOR  TOK_PRINT
%token TOK_AFFECT TOK_GE TOK_LE TOK_GT TOK_LT TOK_EQ
%token TOK_NE TOK_PLUS TOK_MINUS TOK_MUL TOK_DIV TOK_MOD
%token TOK_UMINUS TOK_SEMICOL TOK_COMMA TOK_LPAR TOK_RPAR TOK_LACC
%token TOK_RACC TOK_STRING TOK_DO

%nonassoc TOK_THEN
%nonassoc TOK_ELSE

%right TOK_AFFECT

%left TOK_OR
%left TOK_AND
%left TOK_BOR
%left TOK_BXOR
%left TOK_BAND
%left TOK_EQ TOK_NE
%left TOK_GT TOK_LT TOK_GE TOK_LE
%left TOK_SRL TOK_SRA TOK_SLL

%left TOK_PLUS TOK_MINUS
%left TOK_MUL TOK_DIV TOK_MOD

%left TOK_UMINUS TOK_NOT TOK_BNOT


%type <intval> TOK_INTVAL
%type <strval> TOK_IDENT TOK_STRING
%type <ptr> program listdecl listdeclnonnull vardecl ident type listtypedecl decl maindecl
%type <ptr> listinst listinstnonnull inst block expr listparamprint paramprint

%%


/* Regles ici */
program:
        listdeclnonnull maindecl
        {
            $$ = make_node(NODE_PROGRAM, 2, $1, $2);
            *program_root = $$;
        }
        | maindecl
        {
            $$ = make_node(NODE_PROGRAM, 2, NULL, $1);
            *program_root = $$;
            
        }
        ;

listdecl:
        listdeclnonnull
        {
            $$ = $1;
        }
        |
        ;

listdeclnonnull:
        vardecl 
        {   
            $$ = make_node(NODE_LIST, 1, $1);
            *program_root = $$; 
        }
        | listdeclnonnull vardecl
        {
            $$ = make_node(NODE_LIST, 2, $1, $2);
            *program_root = $$;
            
        }
        ;

vardecl: 
        type listtypedecl TOK_SEMICOL
        {
            $$ = make_node(NODE_DECLS,3, $1, $2);
        }
        ;

type:
        TOK_INT
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_INT);
            *program_root = $$;
        }
        | TOK_BOOL
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_BOOL);
            *program_root = $$;
        }
        | TOK_VOID
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_VOID);
            *program_root = $$;
        }
        ;

listtypedecl:
        decl
        {
            $$ = make_node(NODE_LIST, 1, $1);
            *program_root = $$;
        }
        |listtypedecl TOK_COMMA decl
        {
            $$ = make_node(NODE_LIST, 2, $1, $3);
            *program_root = $$;
        }
        ;

decl:   
        ident
        {
            $$ = make_node(NODE_DECL, 1, $1);
            *program_root = $$;
        }
        | ident TOK_AFFECT expr
        {

            $$ = make_node(NODE_DECL, 2, $1, $3);

            *program_root = $$;
        }
        ;


maindecl:
        type ident TOK_LPAR TOK_RPAR block
        {
            $$ = make_node(NODE_FUNC, 3, $1, $2, $5);
            *program_root = $$;
        }
        ;

listinst:
        listinstnonnull
        {
            $$ = make_node(NODE_LIST, 1, $1);
            *program_root = $$;
        }
        |
        ;
    
listinstnonnull:
        inst
        {
            $$ = make_node(NODE_LIST, 1, $1);
            *program_root = $$;
        }
        |
        listinstnonnull inst
        {
            $$ = make_node(NODE_LIST, 2, $1, $2);
            *program_root = $$;
        }
        ;

inst:
        expr TOK_SEMICOL
        {
            $$ = $1;
        }
        | 
        TOK_IF TOK_LPAR expr TOK_RPAR inst TOK_ELSE inst
        {
            $$ = make_node(NODE_IF, 3, $3, $5, $7);
            *program_root = $$;
        }
        |
        TOK_IF TOK_LPAR expr TOK_RPAR inst %prec TOK_THEN
        {
            $$ = make_node(NODE_IF, 2, $3, $5);
            *program_root = $$;
        }
        |
        TOK_WHILE TOK_LPAR expr TOK_RPAR inst
        {
            $$ = make_node(NODE_WHILE, 2, $3, $5);
            * program_root = $$;
        }
        |
        TOK_FOR TOK_LPAR expr TOK_SEMICOL expr TOK_SEMICOL expr TOK_RPAR inst
        {
            $$ = make_node(NODE_FOR, 4, $3, $5,$7,$9);
            *program_root = $$;
        }
        |
        TOK_DO inst TOK_WHILE TOK_LPAR expr TOK_RPAR TOK_SEMICOL
        {
            $$ = make_node(NODE_DOWHILE, 2, $2,  $5);
            *program_root = $$;
        }
        |
        block
        {
            $$ = $1;
        }
        |
        TOK_SEMICOL
        {
            $$ = NULL;

        }
        |
        TOK_PRINT TOK_LPAR listparamprint TOK_RPAR TOK_SEMICOL
        {
            $$ = make_node(NODE_PRINT, 1, $3);
            *program_root = $$;
        }
        ;

block:
        
        TOK_LACC listdecl listinst TOK_RACC
        {
            $$ = make_node(NODE_BLOCK, 2,  $2, $3);
            *program_root = $$;
        }
        ;

expr:

        expr TOK_MUL expr
        {
            $$ = make_node(NODE_MUL, 2, $1,  $3);
            *program_root = $$;
        }
        | expr TOK_DIV expr
        {
            $$ = make_node(NODE_DIV, 2, $1, $3);
            *program_root = $$;
        }
        |
        expr TOK_PLUS expr
        {
            $$ = make_node(NODE_PLUS, 2, $1,  $3);
            *program_root = $$;
        }
        |
        expr TOK_MINUS expr
        {
            $$ = make_node(NODE_MINUS, 2, $1, $3);
            *program_root = $$;
        }
        |
        expr TOK_MOD expr
        {
            $$ = make_node(NODE_MOD, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_LT expr
        {
            $$ = make_node(NODE_LT, 2, $1,  $3);
            *program_root = $$;
        }
        | 
        expr TOK_GT expr
        {
            $$ = make_node(NODE_GT, 2, $1,  $3);
            *program_root = $$;
        }
        | 
        TOK_MINUS expr %prec TOK_UMINUS
        {
            $$ = $2;
        }
        | 
        expr TOK_GE expr
        {
            $$ = make_node(NODE_GE, 2, $1, $3);
            *program_root = $$;
        }
        |
        expr TOK_LE expr
        {
            $$ = make_node(NODE_LE, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_EQ expr
        {
            $$ = make_node(NODE_EQ, 2, $1,  $3);
            *program_root = $$;
        }
        | 
        expr TOK_NE expr
        {
            $$ = make_node(NODE_NE, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_AND expr
        {
            $$ = make_node(NODE_AND, 2, $1,  $3);
            *program_root = $$;
        }
        | 
        expr TOK_OR expr
        {
            $$ = make_node(NODE_OR, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_BAND expr
        {
            $$ = make_node(NODE_BAND, 2, $1,$3);
            *program_root = $$;
        }
        | 
        expr TOK_BOR expr
        {
            $$ = make_node(NODE_BOR, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_BXOR expr
        {
            $$ = make_node(NODE_BXOR, 2, $1,  $3);
            *program_root = $$;
        }
        | 
        expr TOK_SRL expr
        {
            $$ = make_node(NODE_SRL, 2, $1, $3);
            *program_root = $$;
        }
        | 
        expr TOK_SRA expr
        {
            $$ = make_node(NODE_SRA, 2, $1,$3);
            *program_root = $$;
        }
        | 
        expr TOK_SLL expr
        {
            $$ = make_node(NODE_SLL, 2, $1, $3);
            *program_root = $$;
        }
        | 
        TOK_NOT expr
        {
            $$ = make_node(NODE_NOT, 1,$2);
            *program_root = $$;
        }
        | 
        TOK_BNOT expr
        {
            $$ = make_node(NODE_BNOT, 1, $2);
            *program_root = $$;
        }
        | 
        TOK_LPAR expr TOK_RPAR
        {
            $$ = $2;
        }
        | 
        ident TOK_AFFECT expr
        {
            $$ = make_node(NODE_AFFECT, 2, $1, $3);
            *program_root = $$;
        }
        | 
        TOK_INTVAL
        {
            $$ = make_node(NODE_INTVAL, 1 );
            *program_root = $$;
        }
        | 
        TOK_TRUE
        {
            $$ = make_node(NODE_BOOLVAL, 1 );
            *program_root = $$;
        }
        | 
        TOK_FALSE
        {
            $$ = make_node(NODE_BOOLVAL, 1);
            *program_root = $$;
        }
        | 
        ident
        {
           $$ = $1; 
        }
        ;

listparamprint: 
        listparamprint TOK_COMMA paramprint
        {
            $$ = make_node(NODE_LIST, 3, $1,$3);
            *program_root = $$;
        }
        |
        paramprint
        {
            $$ = $1;
        }
        ;

paramprint:
        ident
        {
            $$ = $1;
        }
        |
        TOK_STRING
        {
            $$ = make_node(NODE_STRINGVAL, 1);
            *program_root = $$;
        }
        ;

ident:
        TOK_IDENT
        {
            $$ = make_node(NODE_IDENT, 1, yylval.strval); 
            *program_root = $$;

        }
        ;

%%

/* A completer et/ou remplacer avec d'autres fonctions */
node_t make_node(node_nature nature, int nops, ...) {


    va_list ap;
    node_t res = malloc(sizeof(node_s));
    if(res == NULL){
        printf("ERREUR D'ALLOCATION MEMOIRE DANS LE MAKENODE");
        return 0;
    }
    else {
        
        if(nops < 1){

            printf("ERREUR DE NOPS DANS LE MAKENODE");

            return NULL;
        }
    res->opr = malloc(sizeof(node_t)*nops);
    
    res->nature = nature;
    res->lineno = yylineno;
    res->nops = nops;
    va_start(ap, nops);
    switch(nature){
        case NODE_TYPE :
            res->type = va_arg(ap, node_type);
            break;

        case NODE_IDENT :

            res->type = TYPE_NONE;
            char * monstr = va_arg(ap, char *);
            res->ident = malloc(sizeof(char)*strlen(monstr));
            res->ident = monstr;
            break;
        
        case NODE_INTVAL :
            res->type = TYPE_NONE;
            res->value = yylval.intval;
            break;
        
        case NODE_STRINGVAL :
            res->type = TYPE_NONE;
            res->str = yylval.strval;
            break;

        case NODE_BOOLVAL :
            res->type = TYPE_NONE;
            res->value = yylval.intval;
            break;

        default :
            res->type = TYPE_NONE;
            for(int i = 0 ; i < nops ; i++){
                node_t arg_res = va_arg(ap, node_t);
                if(arg_res != NULL)
                    res->opr[i] = arg_res;
            }
            break;
    }
    va_end(ap);
    return res;
}
    /*node_t res;

    if(nops < 1)
    {
        return NULL;
    }

    res.nature = nature;

    va_start(ap,nops)
    
    

    switch(nature)
    {
        case: NODE_IDENT
            res.ident = strval
    }
    va_end(ap);
    return res;*/
}



/* A completer */
void analyse_tree(node_t root) {
    if (!stop_after_syntax) {
        // Appeler la passe 1

        if (!stop_after_verif) {
            create_program(); 
            // Appeler la passe 2

            dump_mips_program(outfile);
            free_program();
        }
        free_global_strings();
    }
}



void yyerror(node_t * program_root, char * s) {
    fprintf(stderr, "Error line %d: %s\n", yylineno, s);
    exit(1);
}

