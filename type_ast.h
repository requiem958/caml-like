
#ifndef __TYPE_AST__
#define __TYPE_AST__ 

#include "analyse_lexicale.h"

typedef enum {
  A_PRG,
  A_LET,//Les bébés adorent ça
  A_IN,
  A_AND,
  A_OP,
  A_LOG,
  A_COMP,
  A_IF,
  A_THEN,
  A_NAME, //Nom de var ou fonc
  A_VAL //A ne pas confondre avec l'amont !
} TypeAst ;

typedef enum {ERROA,N_PLUS, N_MUL, N_MOINS,N_DIV, U_MOINS} TypeOpAr ;
typedef enum {ERROL,N_AND,N_OR,N_NOT} TypeOpLog;
typedef enum {ERROC,N_EQ,N_NEQ,N_GT,N_LT,N_GTE,N_LTE} TypeOpComp; 

typedef union{
  TypeOpAr opAr;
  TypeOpLog opLog;
  TypeOpComp opComp;
}TypeOp;

typedef struct noeud {
  TypeAst nature ;
  TypeOp operateur;
  struct noeud *gauche, *droite ;
  Type t;
  Valeur valeur ;
} NoeudAst ;

typedef NoeudAst *Ast ;

#endif
