#ifndef AST_CONSH
#define AST_CONSH

#include "type_ast.h"
#include "analyse_lexicale.h"

Ast creer_noeud(TypeAst a, Ast droit, Ast gauche);
Ast creer_operation(TypeOperateur opr , Ast opde_gauche , Ast opde_droit) ;
// renvoie un arbre abstrait de nature OPERATEUR,  d'operateur opr 
// et ayant opde_gauche et opde_droit pour fils gauche et droit

Ast creer_valeur(Valeur val, Type t) ; 
// renvoie un arbre abstrait "feuille", de nature OPERATION  
// et de valeur val

void libere(Ast a);


#endif
