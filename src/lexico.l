%{
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"
#include "common.h"
#include "context.h"

#include "y.tab.h"

void yyerror(char *);
void yyset_debug(int);
int yyparse(node_t * program_root);
int fileno(FILE * stream);
void analyse_tree(node_t root);


char * infile = NULL;
char * outfile = NULL;
bool stop_after_syntax = false;
bool stop_after_verif = false;
int trace_level = DEFAULT_TRACE_LEVEL;
int nb_reg = DEFAULT_MAX_REGS;
bool print_warning = false;
/* A completer */

#if YYDEBUG
extern int yydebug;
#endif


%}

%option yylineno


LETTRE          [a-zA-Z]
/* A completer ; fausses definitions pour que lex ne fasse pas d'erreur */
SPECHAR         [ -/] 
/*Possible source d'erreur*/
CHIFFRE         [0-9]
CHIFFRE_NON_NUL [1-9]
ENTIER_DEC      (0|{CHIFFRE_NON_NUL}){CHIFFRE}*
LETTER_HEXA     [a-fA-Z]
ENTIER_HEX      0x({CHIFFRE}|{LETTER_HEXA})+
IDF             {LETTRE}({LETTRE}|{CHIFFRE}|_)*
ENTIER          {ENTIER_DEC}|{ENTIER_HEX}
CHAINE_CAR      [\x20-\x21\x22-\x5c]
CHAINE          \"(\\.|[^"\\])*\"
COMMENTAIRE     \/\/[\x20-\x7E]*
SPACE           [ \t]+
SAUTDELIGNE     \r\n


%%

"void"          return TOK_VOID;

"int"           return TOK_INT;

"bool"          return TOK_BOOL;

"="             return TOK_AFFECT;

"true"          return TOK_TRUE;

"false"         return TOK_FALSE;

"if"            return TOK_IF;

"else"          return TOK_ELSE;

"while"         return TOK_WHILE;

"for"           return TOK_FOR;

"print"         return TOK_PRINT;

"do"            return TOK_DO;

";"             return TOK_SEMICOL;

","             return TOK_COMMA;

"-"             return TOK_MINUS;

">="            return TOK_GE;

"<="            return TOK_LE;

">"             return TOK_GT;

"<"             return TOK_LT;

"=="            return TOK_EQ;

"!="            return TOK_NE;

"+"             return TOK_PLUS;

"*"             return TOK_MUL;

"/"             return TOK_DIV;

"%"             return TOK_MOD;

"("             return TOK_LPAR;

")"             return TOK_RPAR;

"{"             return TOK_LACC;

"}"             return TOK_RACC;

"||"            return TOK_OR;

"&&"            return TOK_AND;

"!"             return TOK_NOT;

"|"             return TOK_BOR;

"&"             return TOK_BAND;

"~"             return TOK_BNOT;

"^"             return TOK_BXOR;

"<<"            return TOK_SLL;

">>"            return TOK_SRL;

">>>"           return TOK_SRA;


{IDF} {
                yylval.strval = strdup(yytext);
                return TOK_IDENT;
}

{ENTIER} {
                yylval.intval = atoi(yytext);            
                return TOK_INTVAL;
}

{CHAINE} {
                yylval.strval = strdup(yytext);
                return TOK_STRING;
}

{COMMENTAIRE} {
}



{SAUTDELIGNE}|{SPACE} {

}






. {
        fprintf(stderr, "Error line %d: Lexical error\n", yylineno);
        exit(1);
}


%%

int yywrap(void) {
    return 1;
}


int main(int argc, char ** argv) 
{

    node_t program_root;

    if(parse_args(argc, argv) == -1){return 0;}

    set_trace_level(trace_level);
    
    yyin = fopen(infile, "r");

    yyparse(&program_root);

    dump_tree(program_root, "arbre_du_program.dot");
    fclose(yyin);

    analyse_tree(program_root);
    dump_tree(program_root, "post_pass1.dot");
    yylex_destroy();
    return 0;
}

