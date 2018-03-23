
#ifndef __TYPE_AST__
#define __TYPE_AST__ 

#include "analyse_lexicale.h"

typedef enum {OPERATION, VALEUR} TypeAst ;
typedef enum {N_PLUS, N_MUL, N_MOINS,N_DIV, U_MOINS} TypeOperateur ;

typedef struct noeud {
  TypeAst nature ;
  TypeOperateur operateur ;
  struct noeud *gauche, *droite ;
  Type t;
  Valeur valeur ;
} NoeudAst ;

typedef NoeudAst *Ast ;

#endif  
