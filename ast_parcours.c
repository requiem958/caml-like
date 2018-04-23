#ifndef AST_P_INCLUDED
#define AST_P_INCLUDED
#include <stdio.h>
#include <limits.h>

#include "tabvar.h"
#include "type_ast.h"

#define CASEOP(op,symb)\
 case op:\
 printf(symb);\
 break


void aff_opAr(TypeOpAr op){
  switch (op) {
    CASEOP(U_MOINS,"-");
    CASEOP(N_PLUS,"+");
    CASEOP(N_MOINS,"-");
    CASEOP(N_MUL,"*");
    CASEOP(N_DIV,"/");
  default:
    printf("#ERR#");
    break;
  }
}
void aff_opLog(TypeOpLog op){
  switch(op){
    CASEOP(N_AND,"&");
    CASEOP(N_OR,"or");
    CASEOP(N_NOT,"!");
  default:
    printf("#ERR#");
    break;
  }
}

void aff_opComp(TypeOpComp op){
  switch(op){
    CASEOP(N_EQ,"==");
    CASEOP(N_NEQ,"!=");
    CASEOP(N_GT,">");
    CASEOP(N_LT,"<");
    CASEOP(N_GTE,">=");
    CASEOP(N_LTE,"<=");
  default:
    printf("#ERR#");
  }
}

void afficherarbre(Ast expr) {
  if (expr == NULL){
    printf("()");
    return;
  }
  switch (expr->nature) {
  case A_PRG:
    afficherarbre(expr->gauche);
    printf(";\n");
    afficherarbre(expr->droite);
    break;
  case A_LET:
    afficherarbre(expr->gauche);
    printf("<-");
    afficherarbre(expr->droite);
    break;
  case A_IN:
    printf("[ ");
    afficherarbre(expr->gauche);
    printf("] in {");
    afficherarbre(expr->droite);
    printf("}");
    break;
  case A_AND:
    printf(" and ");
    afficherarbre(expr->gauche);
    afficherarbre(expr->droite);
    break;
  case A_OP:
    aff_opAr(expr->operateur.opAr);
    if(expr->operateur.opAr == U_MOINS)
      afficherarbre(expr->droite);
    else{
      printf("(");
      afficherarbre(expr->gauche);
      printf(", ");
      afficherarbre(expr->droite);
      printf(")");
    }  
    break;
  case A_LOG:
    aff_opLog(expr->operateur.opLog);
    if(expr->operateur.opLog == N_NOT)
      afficherarbre(expr->droite);
    else{
      printf("(");
      afficherarbre(expr->gauche);
      printf(", ");
      afficherarbre(expr->droite);
      printf(")");
    }  
    break;
  case A_COMP:
    aff_opLog(expr->operateur.opComp);
    printf("(");
    afficherarbre(expr->gauche);
    printf(", ");
    afficherarbre(expr->droite);
    printf(")");
    break;
  case A_IF:
    printf("ITE { cond(");
    afficherarbre(expr->gauche);
    printf("), ");
    afficherarbre(expr->droite);
    printf("}");
    break;
  case A_THEN:
    printf("then(");
    afficherarbre(expr->gauche);
    printf("), else (");
    afficherarbre(expr->droite);
    printf(")");
    break;
  case A_NAME:
    printf(expr->var.nom);
    break;
  case A_VAL:
    if (expr->var.nom != NULL)
       printf("VAR(%s =",expr->var.nom);
    switch(expr->var.t){
    case STR:
      printf(expr->var.val.string);
      break;
    case FLOAT:
      printf("%lf", expr->var.val.val_f);
      break;
    case INT:
      printf("%d", expr->var.val.val_i);
      break;
    case UNIT:
      printf("()");
      break;
    case BOOL:
      printf("%s", expr->var.val.val_b ? "true" : "false");
      break;
    default:
      printf("#ERR");
      break;
    }
    if (expr->var.nom != NULL)
      printf(")");
    break;
  default:
    printf("WHAT");
    break;
  }
}

Variable evaluation(Ast expr, MemVar *mem) {
  Variable vd;
  Variable vg;
  MemVar mem2;
  if (expr == NULL){
    vd.t = UNIT;
    return vd;
  }
  switch (expr->nature) {
  case A_PRG:
    vg = evaluation(expr->gauche, mem);
    if (expr->droite == NULL){
      /* Afficher vd en style caml : val nom : type = valeur*/      
      printf("val %s : %s = ",vg.nom, str_type(vg.t));
      print_value(vg);
      return vg;
    }
    return evaluation(expr->droite, mem);;
  case A_LET:
    vd = evaluation(expr->droite,mem); //Valeur
    vg = evaluation(expr->gauche,mem); //Nom
    vg.t = vd.t;
    vg.val = vd.val;
    /* Ajouter vd à l'environnement courant*/
    ajouter_var(&vd,mem);
    return vd;
  case A_IN:
    /* Creer nouvel environnement, copie ancien*/
    copier_environnement(mem,&mem2);
    /*Tout se passe dans la copie depuis ici*/
    evaluation(expr->gauche, &mem2);
    vd = evaluation(expr->droite,&mem2);
    /* Detruire la copie */
    mem2.taille = 0;
    /*Renvoyer result final */
    return vd;
  case A_AND:
    vd = evaluation(expr->droite,mem);
    /* renvoyer la derniere à afficher */
    return evaluation(expr->gauche,mem);
  case A_OP:
    vd = evaluation(expr->droite,mem);
    vg = evaluation(expr->gauche,mem);

    if (vg.t != INT || vg.t != FLOAT){
      printf("ERR : Opérande arithmétique attendue\n");
      return (Variable) {.t = ERR };
    }
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donné alors que %s attendue\n",\
	     str_type(vd),str_type(vg));
      return (Variable) {.t = ERR };
    }

    return apply_opAr(expr->operateur.opAr,vg,vd);
  case A_LOG:
    vd = evaluation(expr->droite,mem);
    vg = evaluation(expr->gauche,mem);

    if (vg.t != BOOL){
      printf("ERR : Opérande logique attendue\n");
      return (Variable) {.t = ERR };
    }
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donné alors que %s attendue",\
	     str_type(vd),str_type(vg));
      return (Variable) {.t = ERR };
    }
    return apply_opLog(expr->operateur.opLog,vg,vd);
  case A_COMP:
    vg = evaluation(expr->gauche,mem);
    vd = evaluation(expr->droite,mem);
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donné alors que %s attendue\n",\
	     str_type(vd),str_type(vg));
      return (Variable) {.t = ERR };
    }
    return apply_opComp(expr->operateur.opComp,vg,vd);
  case A_IF:
    //Pour ne pas polluer l'environnement global
    copie_environnement(mem,&mem2);
    vg = evaluation(expr->gauche,&mem2);
    mem2.taille = 0;
    if (vd.t != BOOL){
      printf("Booléen attendu au lieu de %s\n",str_type(vg));
      return (Variable) {.t = ERR };
    }

    if (expr->droite->nature == A_THEN){
      if(vd.val.val_b)
	return evaluation(expr->droite->gauche,mem);
      else
	return evaluation(expr->droite->droite,mem);
    }

    if(vd.val.val_b)
      return evaluation(expr->droite,mem);
    else
      return (Variable) {.t = UNIT };
  case A_NAME:
    if( chercher_var(expr->var.nom, mem, &(vg.val)) != -1){
	vg.nom = NULL;
      return vg;
}
    else{
      printf("Valeur inconnue %s\n",expr->var.nom);
      return (Variable){.t = ERR };
    }
  case A_VAL:
    return expr->var;
  default:
    return (Variable){.t = ERR};
  }
}
  
#endif
