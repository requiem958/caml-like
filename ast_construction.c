#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "type_ast.h"
#include "analyse_lexicale.h"

Ast creer_noeud(TypeAst t, Ast opde_gauche, Ast opde_droit){
  Ast expr ;
  expr = malloc (sizeof(NoeudAst));
  expr->nature = t;
  expr->gauche = opde_gauche;
  expr->droite = opde_droit;
  return expr ;
}

Ast creer_operation(TypeOp opr , Ast opde_gauche , Ast opde_droit) 
{
  Ast expr ;
  expr = (Ast) malloc (sizeof(NoeudAst));
  expr->nature = A_OP;
  expr->operateur = opr;
  expr->gauche = opde_gauche;
  expr->droite = opde_droit;
  return expr ;
}

Ast creer_valeur(Valeur val, Type t) {
  Ast expr ;
  expr = (Ast) malloc (sizeof(NoeudAst));
  expr->nature = A_VAL;
  expr->var.t = t;
  switch(t){
  case STR:
    strcpy(expr->var.val.string, val.string);
    break;
  case INT:
    expr->var.val.val_i = val.val_i;
    break;
  case FLOAT:
    expr->var.val.val_f = val.val_f;
    break;
  case BOOL:
    expr->var.val.val_b = val.val_b;
  default:
    expr->var.t = ERR;
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

