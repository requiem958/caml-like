#include <stdio.h>
#include "analyse_syntaxique.h"
#include "analyse_lexicale.h"
#include "lecture_caracteres.h"
#include "type_ast.h"
#include "ast_construction.h"
#include <string.h>

//Private functions
static err_syntax let (Lexeme l,Ast* A1);
static err_syntax prg (Lexeme l,Ast* A1);
static err_syntax fin_instr(Lexeme l, Ast* A1);
static err_syntax instr (Lexeme l,Ast* A1);
static err_syntax suite_init (Lexeme l,Ast* A1);
static err_syntax init (Lexeme l,Ast* A1);
static err_syntax type (Lexeme l,Ast* A1);
static err_syntax eag(Lexeme l, Ast* A1);
static err_syntax sterme(Lexeme l,Ast* A2);
static err_syntax ssterme(Lexeme l,Ast* A2, Ast A1);
static err_syntax terme (Lexeme l,Ast* A1);
static err_syntax sfacteur (Lexeme l,Ast* A2);
static err_syntax ssfacteur (Lexeme l,Ast* A2, Ast A1);
static err_syntax facteur  (Lexeme l,Ast* A1);
static err_syntax op1 (Lexeme l,TypeOperateur*t);
static err_syntax op2 (Lexeme l,TypeOperateur*t);
static err_syntax opunaire (Lexeme l,Ast* A1, char is_minus);
static err_syntax fin(Lexeme l);
static err_syntax test_op2 (Lexeme l,TypeOperateur *t);
static void print_err(err_syntax e);
static void show_user_err(const err_syntax current, const Lexeme l);


void analyser(char *nom_fichier, Ast* A1){
  Lexeme l;
  demarrer(nom_fichier);
  prg(lexeme_courant(), A1);
  l = lexeme_courant();
  fin(l);
  l=lexeme_courant();
  if (l.nature != FIN_SEQUENCE)
    printf("ERREUR FIN SEQ : (%d,%d) %s \n",l.ligne,l.colonne,l.chaine);
  arreter();
}

err_syntax eag(Lexeme l, Ast* A1){
  return sterme(l,A1);
}

err_syntax sterme(Lexeme l,Ast* A2){
  Ast A1;
  err_syntax e = NOERR;
  
  //lecture d'un terme
  e= terme(l,&A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_STERME;
  }
  
  //lecture d'une ssterme
  l=lexeme_courant();
  e= ssterme(l,A2, A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_STERME;
  }
  return NOERR;
}

err_syntax ssterme(Lexeme l,Ast* A2, Ast A1){
  TypeOperateur Op;
  Ast A3;
  Ast A4;
  err_syntax e = NOERR;

  //cas epsilon
  if (l.nature == AND || l.nature == TYPE || l.nature==PARF || l.nature == IN || l.nature == FIN_INSTR){
    *A2 = A1;
    return NOERR;
  }
  
  //lecture et construction d'un op1
  e= op1(l, &Op);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSTERME;
  }
  
  //lecture et construction du premier terme suivant op
  l=lexeme_courant();
  e= terme(l,&A3);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSTERME;
  }
  A4 = creer_operation(Op,A1,A3);

  //lecture et construction de la suite des termes si existante
  l=lexeme_courant();
  e= ssterme(l,A2,A4);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSTERME;
  }
  return NOERR;
}

err_syntax terme (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  // lecture d'un sfacteur et on parle pas du postier
  e= sfacteur(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_TERME;
  }
  return NOERR;
}

err_syntax sfacteur (Lexeme l,Ast* A2){
  Ast A1;
  err_syntax e = NOERR;
  
  e = facteur(l,&A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFACTEUR;
  }
  l=lexeme_courant();
  e = ssfacteur(l,A2, A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFACTEUR;
  }
  return NOERR;
}

err_syntax ssfacteur (Lexeme l,Ast* A2, Ast A1) {
  TypeOperateur Op;
  Ast A3;
  Ast A4;
  err_syntax e=NOERR;

  //cas epsilon
  if (test_op2(l,&Op)== ERR_OP2){
    *A2 = A1;
    return NOERR;
  }

  //Lecture MUL | DIV
  e = op2(l,&Op);

  //Lecture du prochain facteur
  l=lexeme_courant();
  e= facteur(l,&A3);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSFACTEUR;
  }
  A4 = creer_operation(Op,A1,A3);
  
  //Lecture des facteurs restants
  l=lexeme_courant();
  e=ssfacteur(l, A2, A4);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSFACTEUR;
  }
  return NOERR;
}

err_syntax opunaire (Lexeme l,Ast* A1, char is_minus) {
  Ast A2;
  err_syntax e = NOERR;

  switch(l.nature){
  case NUM:
    A2 = creer_valeur(l.valeur,l.type);
    break;
  case PARO:
    avancer();
    l = lexeme_courant();
    e = eag(l,&A2);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_FACTEUR;
    }
    l = lexeme_courant();
    if (l.nature != PARF)
      return ERR_PARF;
    break;
  case VAR:
    break;
  default:
    return ERR_FACTEUR;
  }
  avancer();
  if (is_minus)
    *A1 = creer_operation(U_MOINS,NULL,A2);
  else
    *A1 = A2;
  return NOERR;
}

err_syntax facteur  (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  char is_minus = 0;
  if (l.nature==MOINS){
    avancer();
    is_minus = 1;
  }
  l=lexeme_courant();
  e= opunaire (l,A1, is_minus);
  if (e != NOERR)
    return ERR_FACTEUR;
  return NOERR;
}


err_syntax op1 (Lexeme l,TypeOperateur*t ){
  switch (l.nature){
  case MOINS:
    *t = N_MOINS;
    break;
  case PLUS:
    *t = N_PLUS;
    break;
  default :
    return ERR_OP1;
  }
  avancer();
  return NOERR;
}

err_syntax op2 (Lexeme l,TypeOperateur *t) {
  switch (l.nature){
  case MUL:
    *t=N_MUL;
    break;
  case DIV:
    *t=N_DIV;
    break;
  default :
    return ERR_OP2;
  }
  avancer();
  return NOERR;
}

err_syntax test_op2 (Lexeme l,TypeOperateur *t) {
  switch (l.nature){
  case MUL:
    *t=N_MUL;
    break;
  case DIV:
    *t=N_DIV;
    break;
  default :
    return ERR_OP2;
  }
  return NOERR;
}

err_syntax fin(Lexeme l){
  while (l.nature == ERREUR){
    printf("Attention, on va continuer malgré : (%d,%d) %s\n",l.ligne,l.colonne,\
	   l.chaine);
    avancer();
    l = lexeme_courant();
  }
  avancer();
  return NOERR;
}

void show_user_err(const err_syntax current, const Lexeme l){
  printf("(%d,%d) : %s ",l.ligne,l.colonne,l.chaine);
  print_err(current);
}

void print_err(const err_syntax e){
  switch(e){
  case ERR_PRG:
    printf("programme mal formé \n");
    break;
  case ERR_INSTR :
    printf("instruction mal formée \n");
    break;
  case ERR_SINIT:
    printf("suite de l'initialisation incorect \n");
    break;
  case ERR_INIT:
    printf("initialisation incorect \n");
    break;
  case ERR_LET:
    printf("manque de let \n");
    break;
  case ERR_TYPE:
    printf("ptdr tes qui?????\n");
    break;
  case ERR_OP1:
    printf("Opérande à priorité basse incorrecte\n");
    break;
  case ERR_OP2:
    printf("Opérande à priorité haute inccorrecte\n");
    break;
  case ERR_STERME:
        printf("sequence de terme non valide \n");
    break;
  case ERR_SSTERME:
    printf("suite de la sequence de terme \n");
    break;
  case ERR_TERME:
    printf("terme incorect et on parle pas des bains\n");
    break;
  case ERR_SFACTEUR:
    printf("sequence de facteur incorect\n");
    break;
  case ERR_SSFACTEUR:
    printf("suite de sequence de facteur incorect \n");
    break;
  case ERR_FACTEUR:
    printf("facteur incorect \n");
    break;
  case ERR_PARF:
    printf("parenthese fermante oublié \n");
    break;
  default:
    break;
  }
}


err_syntax prg (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  e = instr(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_PRG;
  }
  l = lexeme_courant();
  e = fin_instr(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_PRG;
  }
  return NOERR;
}

err_syntax fin_instr(Lexeme l, Ast *A1){
  avancer();
  if ( l.nature == FIN_INSTR)
    return NOERR;
  return ERR_FIN;
}

err_syntax instr (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  e = let(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INSTR;
  }
  l=lexeme_courant();
  e = init(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INSTR;
  }
  l=lexeme_courant();
  e = suite_init(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INSTR;
  }
  return NOERR;
}

err_syntax let (Lexeme l,Ast* A1){
  avancer();
  if (l.nature == LET)
    return NOERR;
  return ERR_LET;
}

err_syntax suite_init (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  switch(l.nature){
  case FIN_INSTR:
    return NOERR;
  case AND:
    avancer();
    e = init(lexeme_courant(),A1);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_SINIT;
    }
    e = suite_init(lexeme_courant(),A1);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_INSTR;
    }
    break;
    
  case IN:
    avancer();
    e = init(lexeme_courant(),A1);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_SINIT;
    }
    e = suite_init(lexeme_courant(),A1);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_INSTR;
    }
    break;
  default:
    return ERR_INSTR;
  }
  return NOERR;
}

err_syntax init (Lexeme l,Ast* A1){
  err_syntax e=NOERR;
  if (l.nature != VAR){
    return ERR_INIT;
  }
  avancer();
  l = lexeme_courant();
  if (l.nature != EQUAL){
    return ERR_INIT;
  }
  avancer();
  l = lexeme_courant();
  e = eag(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INIT;
  }
  //avancer();
  l = lexeme_courant();
  e = type(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INIT;
  }
  return NOERR;
  
}
err_syntax type (Lexeme l,Ast* A1){
  if (l.nature == TYPE)
    avancer();
  return NOERR;
}
