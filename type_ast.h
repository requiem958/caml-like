
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
  A_VAL //A ne pas confondre avec l'amont !
} TypeAst ;
typedef enum {N_PLUS, N_MUL, N_MOINS,N_DIV, U_MOINS, N_AND,N_OR,N_NOT} TypeOperateur ;

typedef struct noeud {
  TypeAst nature ;
  TypeOperateur operateur ;
  struct noeud *gauche, *droite ;
  Type t;
  Valeur valeur ;
} NoeudAst ;

typedef NoeudAst *Ast ;

#endif
