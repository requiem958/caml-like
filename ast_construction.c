#include <stdlib.h>
#include <stdio.h> 

#include "type_ast.h"
#include "analyse_lexicale.h"

Ast creer_operation(TypeOperateur opr , Ast opde_gauche , Ast opde_droit) 
{
      Ast expr ;
      expr = (Ast) malloc (sizeof(NoeudAst));
      expr->nature = OPERATION;
      expr->operateur = opr;
      expr->gauche = opde_gauche;
      expr->droite = opde_droit;
      return expr ;
}

Ast creer_valeur(Valeur val, Type t) {
  Ast expr ;
  expr = (Ast) malloc (sizeof(NoeudAst));
  expr->nature = VALEUR;
  expr->t = t;
  switch(t){
  case INT:
    expr->valeur.val_i = val.val_i;
    break;
  case FLOAT:
    expr->valeur.val_f = val.val_f;
    break;
  }
  return expr ;
}


void liberenoeud (struct noeud* a){
  if (a == NULL)
    return;
  liberenoeud(a->gauche);
  liberenoeud(a->droite);
  free(a);
}


void libere(Ast a){
  if (a == NULL)
    return;
  liberenoeud(a->gauche);
  liberenoeud(a->droite);
  free(a);
}

