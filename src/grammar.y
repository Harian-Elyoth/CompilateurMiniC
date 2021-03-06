%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "defs.h"
#include "pass1.h"
#include "pass2.h"
#include "utils/env.h"
#include "utils/context.h"
#include "common.h"
#include "mips_inst.h"

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
        {
            $$ = NULL;
        }
        ;

listdeclnonnull:
        vardecl 
        {   
            $$ = $1;
             
        }
        | listdeclnonnull vardecl
        {
            $$ = make_node(NODE_LIST, 2, $1, $2 );
            
        }
        ;

vardecl: 
        type listtypedecl TOK_SEMICOL
        {
            $$ = make_node(NODE_DECLS, 2,  $1, $2);
        }
        ;

type:
        TOK_INT
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_INT);
            
        }
        | TOK_BOOL
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_BOOL);
            
        }
        | TOK_VOID
        {
            $$ = make_node(NODE_TYPE, 1, TYPE_VOID);
            
        }
        ;

listtypedecl:
        decl
        {
            $$ = $1;
            
        }
        |listtypedecl TOK_COMMA decl
        {
            $$ = make_node(NODE_LIST, 2, $1, $3);
            
        }
        ;

decl:   
        ident
        {
            $$ = make_node(NODE_DECL, 1, $1, NULL);
            
        }
        | ident TOK_AFFECT expr
        {

            $$ = make_node(NODE_DECL, 2, $1, $3);

            
        }
        ;


maindecl:
        type ident TOK_LPAR TOK_RPAR block
        {
            $$ = make_node(NODE_FUNC, 3, $1, $2, $5);
            
        }
        ;

listinst:
        listinstnonnull
        {
            $$ = $1;
            
        }
        |
        {
            $$ = NULL;
        }
        ;
    
listinstnonnull:
        inst
        {
            $$ = $1;
            
        }
        |
        listinstnonnull inst
        {
            $$ = make_node(NODE_LIST, 2, $1, $2);
            
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
            
        }
        |
        TOK_IF TOK_LPAR expr TOK_RPAR inst %prec TOK_THEN
        {
            $$ = make_node(NODE_IF, 2, $3, $5);
            
        }
        |
        TOK_WHILE TOK_LPAR expr TOK_RPAR inst
        {
            $$ = make_node(NODE_WHILE, 2, $3, $5);
        }
        |
        TOK_FOR TOK_LPAR expr TOK_SEMICOL expr TOK_SEMICOL expr TOK_RPAR inst
        {
            $$ = make_node(NODE_FOR, 4,  $3,  $5, $7, $9);
            
        }
        |
        TOK_DO inst TOK_WHILE TOK_LPAR expr TOK_RPAR TOK_SEMICOL
        {
            $$ = make_node(NODE_DOWHILE, 2,  $2, $5);
            
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
            
        }
        ;

block:
        
        TOK_LACC listdecl listinst TOK_RACC
        {
            $$ = make_node(NODE_BLOCK, 2, $2, $3);
            
        }
        ;

expr:

        expr TOK_MUL expr
        {
            $$ = make_node(NODE_MUL, 2, $1, $3);
            
        }
        | expr TOK_DIV expr
        {
            $$ = make_node(NODE_DIV, 2, $1, $3);
            
        }
        |
        expr TOK_PLUS expr
        {
            $$ = make_node(NODE_PLUS, 2, $1, $3);
            
        }
        |
        expr TOK_MINUS expr
        {
            $$ = make_node(NODE_MINUS, 2, $1, $3);
            
        }
        |
        expr TOK_MOD expr
        {
            $$ = make_node(NODE_MOD, 2, $1, $3);
            
        }
        | 
        expr TOK_LT expr
        {
            $$ = make_node(NODE_LT, 2, $1, $3);
            
        }
        | 
        expr TOK_GT expr
        {
            $$ = make_node(NODE_GT, 2, $1, $3);
            
        }
        | 
        TOK_MINUS expr %prec TOK_UMINUS
        {
            $$ = make_node(NODE_UMINUS, 1, $2);
        }
        | 
        expr TOK_GE expr
        {
            $$ = make_node(NODE_GE, 2, $1, $3);
            
        }
        |
        expr TOK_LE expr
        {
            $$ = make_node(NODE_LE, 2, $1, $3);
            
        }
        | 
        expr TOK_EQ expr
        {
            $$ = make_node(NODE_EQ, 2, $1, $3);
            
        }
        | 
        expr TOK_NE expr
        {
            $$ = make_node(NODE_NE, 2, $1, $3);
            
        }
        | 
        expr TOK_AND expr
        {
            $$ = make_node(NODE_AND, 2, $1,$3);
            
        }
        | 
        expr TOK_OR expr
        {
            $$ = make_node(NODE_OR, 2, $1, $3);
            
        }
        | 
        expr TOK_BAND expr
        {
            $$ = make_node(NODE_BAND, 2, $1, $3);
            
        }
        | 
        expr TOK_BOR expr
        {
            $$ = make_node(NODE_BOR, 2, $1, $3);
            
        }
        | 
        expr TOK_BXOR expr
        {
            $$ = make_node(NODE_BXOR, 2, $1, $3);
            
        }
        | 
        expr TOK_SRL expr
        {
            $$ = make_node(NODE_SRL, 2, $1, $3);
            
        }
        | 
        expr TOK_SRA expr
        {
            $$ = make_node(NODE_SRA, 2, $1, $3);
            
        }
        | 
        expr TOK_SLL expr
        {
            $$ = make_node(NODE_SLL, 2, $1, $3);
            
        }
        | 
        TOK_NOT expr
        {
            $$ = make_node(NODE_NOT, 1, $2);
            
        }
        | 
        TOK_BNOT expr
        {
            $$ = make_node(NODE_BNOT, 1, $2);
            
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
            
        }
        | 
        TOK_INTVAL
        {
            $$ = make_node(NODE_INTVAL, 0);
            
        }
        | 
        TOK_TRUE
        {
            $$ = make_node(NODE_BOOLVAL, 1, "true");
            
        }
        | 
        TOK_FALSE
        {
            $$ = make_node(NODE_BOOLVAL, 1, "false");
            
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
            $$   = make_node(NODE_LIST, 2 , $1, $3);
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
            
        }
        ;

ident:
        TOK_IDENT
        {
            $$ = make_node(NODE_IDENT, 1, yylval.strval); 
        }
        ;

%%

node_t make_node(node_nature nature, int nops, ...) {

    va_list ap;
    node_t res = malloc(sizeof(node_s));
    if(res == NULL){
        fprintf(stderr, "ALLOCATION MEMOIRE DANS LE MAKENODE");
        return 0;
    }
        
    res->opr = malloc(sizeof(node_t)*nops);
    
    res->nature = nature;
    res->lineno = yylineno;
    res->nops = nops;
    va_start(ap, nops);

    switch(nature){
        case NODE_TYPE :
            //printf("nops de TYPE = %d\n\n", nops);
            res->nops = 0;
            res->type = va_arg(ap, node_type);
            break;

        case NODE_IDENT :

            res->type = TYPE_NONE;
            res->nops = 0;
            char * monstr = va_arg(ap, char *);
            //printf("\n\nMon str vaut %s\n\n", monstr);
            res->ident = malloc(sizeof(char)*strlen(monstr));
            res->ident = monstr;
            break;
        
        case NODE_INTVAL :
            res->nops = 0;
            res->type = TYPE_INT;
            res->value = yylval.intval;
            break;
        
        case NODE_STRINGVAL :
            res->nops = 0;
            res->type = TYPE_STRING;
            res->str = malloc(sizeof(char)*(strlen(yylval.strval)));
            res->str = yylval.strval;
            break;

        case NODE_BOOLVAL :
            res->nops = 0;
            res->type = TYPE_BOOL;
            if(va_arg(ap, char *) == "true")
                res->value = 1;
            else
                res->value = 0;
            break;

        default :

            res->type = TYPE_NONE;
            for(int i = 0 ; i < nops ; i++){
                res->opr[i] = va_arg(ap, node_t);
            }
            break;
        }
    va_end(ap);
    return res;
}


void analyse_tree(node_t root) 
{
    if (!stop_after_syntax) 
    {
        passe_1(root);
        //dump_tree(root, "apres_verif.dot");

        if (!stop_after_verif) 
        {
            create_program(); 
            passe_2(root);
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

