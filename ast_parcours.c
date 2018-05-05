#ifndef AST_P_INCLUDED
#define AST_P_INCLUDED
#include <stdio.h>
#include <limits.h>
#include <string.h>
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

static Variable  apply_opArF(TypeOpAr op, float v1, float v2){
  Variable ret;
  ret.t = FLOAT;
  ret.nom[0] = 0;
  switch(op){
  case U_MOINS:
    ret.val.val_f = - v1;
    break;
  case N_PLUS:
    ret.val.val_f = v1 + v2;
    break;
  case N_MOINS:
    ret.val.val_f = v1 - v2;
    break;
  case N_MUL:
    ret.val.val_f = v1 * v2;
    break;
  case N_DIV:
    if (v2 != 0)
      ret.val.val_f = v1 / v2;
    else{
      ret.t = ERR;
      ret.val.val_f =0;
      return ret;
    }
    break;
  default:
    ret.t = ERR;
  }
  return ret;
}

static Variable  apply_opArI(TypeOpAr op, int v1, int v2){
  Variable ret;
  ret.t = INT;
  ret.nom[0] = 0;
  switch(op){
  case U_MOINS:
    ret.val.val_i = - v1;
    break;
  case N_PLUS:
    ret.val.val_i = v1 + v2;
    break;
  case N_MOINS:
    ret.val.val_i = v1 - v2;
    break;
  case N_MUL:
    ret.val.val_i = v1 * v2;
    break;
  case N_DIV:
    if (v2 != 0)
      ret.val.val_i = v1 / v2;
    else{
      ret.t = ERR;
    }
    break;
  default:
    ret.t = ERR;
    return ret;
  }
  return ret;
}

Variable apply_opAr(TypeOpAr op,Variable v1, Variable v2){
  switch(v1.t){
  case INT:
    return apply_opArI(op,v1.val.val_i,v2.val.val_i);
  case FLOAT:
    return apply_opArF(op,v1.val.val_f,v2.val.val_f);
  default:
    return (Variable) {.t = ERR };
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

Variable apply_opLog(TypeOpLog op, Variable v1, Variable v2){
  Variable ret;
  ret.t = BOOL;
  ret.nom[0] = 0;
  switch(op){
  case N_AND:
    ret.val.val_b = v1.val.val_b && v2.val.val_b;
    break;
  case N_OR:
    ret.val.val_b = v1.val.val_b || v2.val.val_b;
    break;
  case N_NOT:
    ret.val.val_b = !v1.val.val_b;
    break;
  default:
    ret.t = ERR;
  }
  return ret;
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

static Variable  apply_opCompI(TypeOpComp op, int v1, int v2){
  Variable ret;
  ret.t = BOOL;
  ret.nom[0] = 0;
  switch(op){
  case N_EQ:
    ret.val.val_b = v1 == v2;
    break;
  case N_NEQ:
    ret.val.val_b = v1 != v2;
    break;
  case N_GT:
    ret.val.val_b = v1 > v2;
    break;
  case N_LT:
    ret.val.val_b = v1 < v2;
    break;
  case N_GTE:
    ret.val.val_b = v1 >= v2;
    break;
  case N_LTE:
    ret.val.val_b = v1 <= v2;
    break;
  default:
    ret.t = ERR;
    return ret;
  }
  return ret;
}

static Variable  apply_opCompF(TypeOpComp op, float v1, float v2){
  Variable ret;
  ret.t = BOOL;
  ret.nom[0] = 0;
  switch(op){
  case N_EQ:
    ret.val.val_b = v1 == v2;
    break;
  case N_NEQ:
    ret.val.val_b = v1 != v2;
    break;
  case N_GT:
    ret.val.val_b = v1 > v2;
    break;
  case N_LT:
    ret.val.val_b = v1 < v2;
    break;
  case N_GTE:
    ret.val.val_b = v1 >= v2;
    break;
  case N_LTE:
    ret.val.val_b = v1 <= v2;
    break;
  default:
    ret.t = ERR;
    return ret;
  }
  return ret;
}


Variable apply_opComp(TypeOpComp op, Variable v1, Variable v2){
  switch(v1.t){
  case INT:
    return apply_opCompI(op,v1.val.val_i,v2.val.val_i);
  case FLOAT:
    return apply_opCompF(op,v1.val.val_f,v2.val.val_f);
  default:
    return (Variable) {.t = ERR };
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
    printf(" and ( ");
    afficherarbre(expr->gauche);
    printf(" , ");
    afficherarbre(expr->droite);
    puts(" )");
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
    if (expr->var.nom[0] != 0)
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
    if (expr->var.nom[0] != 0)
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
       printf("val %s : %s = ",vg.nom, str_type(vg.t));
      print_value(vg);
      puts("");
    vd = evaluation(expr->droite, mem);
    /* Afficher vg vd en style caml : val nom : type = valeur*/
   if (vd.t == UNIT)
     return vg;
   /* printf("val %s : %s = ",vd.nom, str_type(vd.t));
      print_value(vd);
      puts("");*/
    return vd;
  case A_LET:
    vd = evaluation(expr->droite,mem); //Valeur
    strcpy(vg.nom, expr->gauche->var.nom);
    vg.t = vd.t;
    vg.val = vd.val;
    /* Ajouter vd à l'environnement courant*/
    ajouter_var(&vg,mem);
    return vg;
  case A_IN:
    /* Creer nouvel environnement, copie ancien*/
    copie_environnement(mem,&mem2);
    /*Tout se passe dans la copie depuis ici*/
    evaluation(expr->gauche, &mem2);
    vd = evaluation(expr->droite,&mem2);
    /* Detruire la copie */
    mem2.taille = 0;
    /*Renvoyer result final */
    return vd;
  case A_AND:
    vg = evaluation(expr->gauche,mem);

    printf("val %s : %s = ",vg.nom, str_type(vg.t));
    print_value(vg);
    puts("");
    /* renvoyer la derniere à afficher */
    return evaluation(expr->droite,mem);
  case A_OP:
    vd = evaluation(expr->droite,mem);
    vg = evaluation(expr->gauche,mem);

    if (vg.t != INT && vg.t != FLOAT){
      printf("ERR : Opérande arithmétique attendue");
          puts("");
      return (Variable) {.t = ERR };
    }
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donné alors que %s attendue",\
	     str_type(vd.t),str_type(vg.t));    puts("");
      return (Variable) {.t = ERR };
    }

    return apply_opAr(expr->operateur.opAr,vg,vd);
  case A_LOG:
    vd = evaluation(expr->droite,mem);
    vg = evaluation(expr->gauche,mem);

    if (vg.t != BOOL){
      printf("ERR : Opérande logique attendue");
          puts("");
      return (Variable) {.t = ERR };
    }
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donné alors que %s attendue",\
	     str_type(vd.t),str_type(vg.t));
          puts("");
      return (Variable) {.t = ERR };
    }
    return apply_opLog(expr->operateur.opLog,vg,vd);
  case A_COMP:
    vg = evaluation(expr->gauche,mem);
    vd = evaluation(expr->droite,mem);
    
    if(vd.t != vg.t){
      printf("Mauvais type : %s donn� alors que %s attendue",\
	     str_type(vd.t),str_type(vg.t));
          puts("");
      return (Variable) {.t = ERR };
    }
    return apply_opComp(expr->operateur.opComp,vg,vd);
  case A_IF:
    //Pour ne pas polluer l'environnement global
    copie_environnement(mem,&mem2);
    vg = evaluation(expr->gauche,&mem2);
    mem2.taille = 0;
    if (vg.t != BOOL){
      printf("Bool�en   attendu au lieu de %s",str_type(vg.t));
          puts("");
      return (Variable) {.t = ERR };
    }

    if (expr->droite->nature == A_THEN){
      if(vg.val.val_b)
	return evaluation(expr->droite->gauche,mem);
      else
	return evaluation(expr->droite->droite,mem);
    }

    if(vd.val.val_b)
      return evaluation(expr->droite,mem);
    else
      return (Variable) {.t = UNIT };
  case A_NAME:
    if( chercher_var(expr->var.nom, mem, &vg) != -1){
	vg.nom[0] = 0;
      return vg;
}
    else{
      printf("Valeur inconnue %s",expr->var.nom);    puts("");
      return (Variable){.t = ERR };
    }
  case A_VAL:
    return expr->var;
  default:
    return (Variable){.t = ERR};
  }
}
  
#endif
