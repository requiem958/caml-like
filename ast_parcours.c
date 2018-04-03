
#include <stdio.h>
#include <limits.h>
#include "type_ast.h"

void aff_operateur(TypeOperateur op){
  switch (op) {
  case U_MOINS:
    printf("-");
    break;
  case N_PLUS:
    printf("+") ;
    break;
  case N_MOINS:
    printf("-") ;
    break;
  case N_MUL:
    printf("*") ;
    break;
  case N_DIV:
    printf("/");
    break;
  } 
}

void afficherarbre(Ast expr) {
  switch (expr->nature) {
  case OPERATION:
    aff_operateur(expr->operateur);
    printf("(");    
    if (expr->operateur == U_MOINS){
      afficherarbre(expr->droite);
    }
    else{
      afficherarbre(expr->gauche);
      printf(", ");
      afficherarbre(expr->droite);
    }
    printf(")");
    break ;
  case VALEUR:
    switch(expr->t){
    case STR:
      break;
    case FLOAT:
      printf("%lf", expr->valeur.val_f);
      break;
    case INT:
      printf("%d", expr->valeur.val_i);
      break;
    }
    break ;
  }
}

float evaluation(Ast expr) {
  switch(expr->nature){
  case OPERATION:
    switch(expr->operateur){
    case N_PLUS:
      return evaluation(expr->gauche)+evaluation(expr->droite);
    case N_MOINS:
      return evaluation(expr->gauche)-evaluation(expr->droite);
    case N_MUL:
      return evaluation(expr->gauche)*evaluation(expr->droite);
    case N_DIV:
      if (evaluation(expr->droite) != 0)
	return evaluation(expr->gauche)/evaluation(expr->droite);
      else{
	printf("Erreur division par zéro\n");
	return INT_MAX;
}
    case U_MOINS:
      return -evaluation(expr->droite);
    }
  case VALEUR:
    switch(expr->t){
    case STR:
      break;
    case FLOAT:
      return expr->valeur.val_f;
    case INT:
      return expr->valeur.val_i;
    }
  }
  return 0;
}
  
