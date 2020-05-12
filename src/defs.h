#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>
#include <stdbool.h>


#define NUM_ARCH_REGS 8
#define DEFAULT_OUTFILE "out.s"
#define DEFAULT_TRACE_LEVEL 0
#define DEFAULT_MAX_REGS NUM_ARCH_REGS

//extern int local_offset;

#define printf_level(level, ...) ({         \
    if (level < trace_level) {              \
        printf(__VA_ARGS__);                \
    }                                       \
})



typedef enum node_nature_s {
    NONE,
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_LIST,
    NODE_DECLS,
    NODE_DECL,
    NODE_IDENT,
    NODE_TYPE,
    NODE_INTVAL,
    NODE_BOOLVAL,
    NODE_STRINGVAL,
    NODE_FUNC,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_DOWHILE,
    NODE_PLUS,
    NODE_MINUS,
    NODE_MUL,
    NODE_DIV,
    NODE_MOD,
    NODE_LT,
    NODE_GT,
    NODE_LE,
    NODE_GE,
    NODE_EQ,
    NODE_NE,
    NODE_AND,
    NODE_OR,
    NODE_BAND,
    NODE_BOR,
    NODE_BXOR,
    NODE_NOT,
    NODE_BNOT,
    NODE_SLL,
    NODE_SRA,
    NODE_SRL,
    NODE_UMINUS,
    NODE_AFFECT,
    NODE_PRINT,

} node_nature;


typedef enum node_type_s {
    TYPE_NONE,
    TYPE_INT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_VOID,  
} node_type;


typedef struct _node_s {
    node_nature nature; 
    node_type type; //TYPE_NONE if nature != NODE_TYPE

    int64_t value; //NODE_INTVAL | NODE_BOOLVAL
    int32_t offset; //Passe 1
    bool global_decl; //Passe 1
    int32_t lineno;
    int32_t stack_size; //Passe 1

    int32_t nops; 
    struct _node_s ** opr;
    
    struct _node_s * decl_node;

    char * ident; //NODE_IDENT
    char * str; //NODE_STRINGVAL

    // Pour l'afichage du graphe
    int32_t node_num;

} node_s;

typedef node_s * node_t;

#endif

