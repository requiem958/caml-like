#include <stdio.h>
#include "analyse_syntaxique.h"
#include "analyse_lexicale.h"
#include "lecture_caracteres.h"
#include "type_ast.h"
#include "ast_construction.h"
#include <string.h>

//Private functions

//General program functions

/* Reconnait un programme CAML */
static err_syntax programme (Lexeme l,Ast* A1); //Axiome : programme

//CAML expressions

/* Reconnaitre une expression (grammaire.tkt)*/

static err_syntax seq_expression(Lexeme l);
static err_syntax ss_expression(Lexeme l);
static err_syntax expression (Lexeme l,Ast *A1);

/* Expressions disponibles */

static err_syntax valeur(Lexeme l,Ast *A1);
static err_syntax affectation(Lexeme l,Ast *A1);
static err_syntax operation(Lexeme l,Ast *A1);
static err_syntax condition(Lexeme l,Ast *A1);


/* Sous expressions de valeurs */

static err_syntax constante(Lexeme l);
static err_syntax identificateur(Lexeme l);

static err_syntax nom_var(Lexeme l);

static err_syntax seq_param(Lexeme l);
static err_syntax ss_param(Lexeme l);
static err_syntax param(Lexeme l);

/* Sous expressions de affectation */

static err_syntax seq_affect(Lexeme l, Ast* A1);
static err_syntax ss_affect(Lexeme l,Ast *A1);
static err_syntax affect(Lexeme l, Ast *A1);

static err_syntax object(Lexeme l);

static err_syntax seq_fparam(Lexeme l);
static err_syntax ss_fparam(Lexeme l);
static err_syntax fparam(Lexeme l);

/* Sous expressions de operation */

//arithmetic op
static err_syntax op1 (Lexeme l,TypeOperateur*t);
static err_syntax op2 (Lexeme l,TypeOperateur*t);
static err_syntax opunaire (Lexeme l,Ast* A1, char is_minus);

//Termes : a + b
static err_syntax terme (Lexeme l,Ast* A1);
static err_syntax seq_terme(Lexeme l,Ast* A2);
static err_syntax ss_terme(Lexeme l,Ast* A2, Ast A1);

//Facteurs : a * b
static err_syntax facteur  (Lexeme l,Ast*A1);
static err_syntax seq_facteur (Lexeme l,Ast* A2);
static err_syntax ss_facteur (Lexeme l,Ast* A2, Ast A1);

//
static err_syntax test_op2 (Lexeme l,TypeOperateur *t);

/* Sous expressions de conditions */
static err_syntax suite_condition(Lexeme l, Ast*A1);

static err_syntax seq_boolor(Lexeme l);
static err_syntax ss_boolor(Lexeme l);
static err_syntax boolor(Lexeme l);

static err_syntax seq_booland(Lexeme l);
static err_syntax ss_booland(Lexeme l);
static err_syntax booland(Lexeme l);

static err_syntax comparaison(Lexeme l, Ast*A1);

static err_syntax op1log(Lexeme l);
static err_syntax op2log(Lexeme l);
static err_syntax op_compar(Lexeme l,Ast*A1);

/* Enlever les lexemes de types ERREUR à la fin du programme */
static err_syntax fin(Lexeme l);

static err_syntax instr (Lexeme l,Ast* A1);
static err_syntax fin_instr(Lexeme l, Ast* A1);

//instructions functions
static err_syntax let (Lexeme l,Ast* A1);
static err_syntax init (Lexeme l,Ast* A1);
static err_syntax suite_init (Lexeme l,Ast* A1);
static err_syntax type (Lexeme l,Ast* A1);

//logic expr functions

//Error control functions
static void print_err(err_syntax e);
static void show_user_err(const err_syntax current, const Lexeme l);

//functions for delete the commantery ptdr j'ai tenter l'anglais
static void nocoment (Lexeme l);

//fonction preprocesseur permetant de raccourcir drastiquement le code
#define AVNC(x) 	do{avancer();l=lexeme_courant();nocoment (x) ;l=lexeme_courant();}while(0);


// cas des fin de fichier à gérer 
void nocoment (Lexeme l){
	if (l.nature ==COMMENTO)
		while(l.nature != COMMENTF){
			avancer();
			l=lexeme_courant();
		}
}


void analyser(char *nom_fichier, Ast* A1){
  Lexeme l;
  demarrer(nom_fichier);
  programme(lexeme_courant(), A1);
  l = lexeme_courant();
  fin(l);
  l=lexeme_courant();
  if (l.nature != FIN_SEQUENCE)
    printf("ERREUR FIN SEQ : (%d,%d) %s \n",l.ligne,l.colonne,l.chaine);
  arreter();
}


static err_syntax programme (Lexeme l,Ast* A1){
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

static err_syntax seq_expression(Lexeme l){
	if (expression(l)!=NOERR){
    show_user_err(e,l);
    return ERR_PRG;
  }
	avancer();
	l=lexeme_courant();
	nocoment (Lexeme l);
	l=lexeme_courant();
  return NOERR;
}

static err_syntax ss_expression(Lexeme l){
  return NOERR;
}

static err_syntax expression (Lexeme l,Ast* A1){
	
	if (valeur(l,A1)==NOERR)
		return NOERR;
	else if (affectation (l,A1)==NOERR)
		return NOERR;
	else if (operation(l,A1)==NOERR)
		return NOERR;
	else if (condition(l,A1)==NOERR)
		return NOERR;
	 show_user_err(ERR_EXPR,l);
	return ERR_EXPR;
	
}

/* Definitions des valeurs */

static err_syntax valeur (Lexeme l, Ast* A1){
	if ( l.nature == VAR)
		return NOERR;
	show_user_err(ERR_VAL,l);
	return ERR_VAL;		
}

static err_syntax constante(Lexeme l){
  return NOERR;
}

static err_syntax identificateur(Lexeme l){
  return NOERR;
}

static err_syntax seq_param(Lexeme l){
  return NOERR;
}

static err_syntax ss_param(Lexeme l){
  return NOERR;
}

static err_syntax param(Lexeme l){
  return NOERR;
}

static err_syntax nom_var(Lexeme l){
  return NOERR;
}

/* Definitions des affectations */

static err_syntax affectation (Lexeme l, Ast* A1) {
	if (l.nature != LET ) {
		show_user_err(ERR_AFF,l);
		return ERR_AFF;
	}
	avancer();
	l=lexeme_courant();
	if (seq_affect(l,A1) != NOERR ) {
		show_user_err(ERR_AFF,l);
		return ERR_AFF;
	}
	avancer();
	l=lexeme_courant();
	if (l.nature != IN ) {
		show_user_err(ERR_AFF,l);
		return ERR_AFF;
	}
	avancer();
	l=lexeme_courant();
	if (expression(l,A1) != NOERR ) {
		show_user_err(ERR_AFF,l);
		return ERR_AFF;
	}	
}

static err_syntax affect (Lexeme l , Ast *A1){
	if (l.nature != VAR){
		show_user_err(ERR_AFFS,l);
		return ERR_AFFS;
	}
		avancer();
	l=lexeme_courant();
	if (l.nature != EQUAL) {
		show_user_err(ERR_AFFS,l);
		return ERR_AFFS;
	}
		avancer();
	l=lexeme_courant();
	if (expression(l,A1) != NOERR) {
		show_user_err(ERR_AFFS,l);
		return ERR_AFFS;
	}
	return NOERR;
}

static err_syntax ss_affect (Lexeme l, Ast* A1){
	if (l.nature != AND)
		return NOERR;
	avancer();
	l=lexeme_courant();
	if (affect_solo(l,A1) != NOERR ) {
		show_user_err(ERR_SAFFS,l);
		return ERR_SAFFS;
	}
	avancer();
	l=lexeme_courant();
	if (suite_affect_solo (l,A1) != NOERR ) {
		show_user_err(ERR_AFF,l);
		return ERR_AFF;
	}
	return NOERR;
}

static err_syntax seq_affect (Lexeme l, Ast *A1){
  return NOERR;
}

static err_syntax object(Lexeme l){
  return NOERR;
}

static err_syntax seq_fparam(Lexeme l){
  return NOERR;
}

static err_syntax ss_fparam(Lexeme l){
  return NOERR;
}

static err_syntax fparam(Lexeme l){
  return NOERR;
}

/* Definitions des opérations */

static err_syntax operation (Lexeme l ,Ast* A1) {
  return seq_terme(l,A1);
}

static err_syntax seq_terme(Lexeme l,Ast* A2){
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
  e= ss_terme(l,A2, A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_STERME;
  }
  return NOERR;
}

static err_syntax ss_terme(Lexeme l,Ast* A2, Ast A1){
  TypeOperateur Op;
  Ast A3;
  Ast A4;
  err_syntax e = NOERR;

  //cas epsilon
  if (l.nature == AND || l.nature == TYPE || l.nature==PARF || l.nature == IN || l.nature == FIN_EXPR){
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
  e= ss_terme(l,A2,A4);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSTERME;
  }
  return NOERR;
}

static err_syntax terme (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  // lecture d'une séquence de facteur et on parle pas d'une armée de postiers
  e= seq_facteur(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_TERME;
  }
  return NOERR;
}

static err_syntax seq_facteur (Lexeme l,Ast* A2){
  Ast A1;
  err_syntax e = NOERR;
  
  e = facteur(l,&A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFACTEUR;
  }
  l=lexeme_courant();
  e = ss_facteur(l,A2, A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFACTEUR;
  }
  return NOERR;
}

static err_syntax ss_facteur (Lexeme l,Ast* A2, Ast A1) {
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
  e=ss_facteur(l, A2, A4);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSFACTEUR;
  }
  return NOERR;
}

static err_syntax opunaire (Lexeme l,Ast* A1, char is_minus) {
  Ast A2;
  err_syntax e = NOERR;

  switch(l.nature){
  case NUM:
    A2 = creer_valeur(l.valeur,l.type);
    break;
  case PARO:
    avancer();
    l = lexeme_courant();
    e = operation(l,&A2);
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

static err_syntax facteur  (Lexeme l,Ast* A1){
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


static err_syntax op1 (Lexeme l,TypeOperateur*t ){
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

static err_syntax op2 (Lexeme l,TypeOperateur *t) {
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

static err_syntax test_op2 (Lexeme l,TypeOperateur *t) {
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



/* Definitions des conditions */

static err_syntax condition (Lexeme l , Ast* A1 ) {
	if (l.nature != IF ){
		show_user_err(ERR_COND,l);
		return ERR_COND;
	}
	avancer();
	l=lexeme_courant();
	if(seq_comparaison(l,A1) != NOERR) {
		show_user_err(ERR_COND,l);
		return ERR_COND;
	}
	avancer();
	l=lexeme_courant();
	if (l.nature != THEN ){
		show_user_err(ERR_COND,l);
		return ERR_COND;
	}
	avancer();
	l=lexeme_courant();
	if(expression(l,A1) != NOERR) {
		show_user_err(ERR_COND,l);
		return ERR_COND;
	}
	avancer();
	l=lexeme_courant();	
	if(suite_condition(l,A1) != NOERR) {
		show_user_err(ERR_COND,l);
		return ERR_COND;
	}
	return NOERR;
}

static err_syntax suite_condition (Lexeme l,Ast*A1){
  return NOERR;
}

static err_syntax seq_boolor(Lexeme l){
  return NOERR;
}

static err_syntax ss_boolor(Lexeme l){
  return NOERR;
}

static err_syntax boolor(Lexeme l){
  return NOERR;
}

static err_syntax seq_booland(Lexeme l){
  return NOERR;
}

static err_syntax ss_booland(Lexeme l){
  return NOERR;
}

static err_syntax booland(Lexeme l){
  return NOERR;
}

static err_syntax comparaison (Lexeme l , Ast A1){
	if(expression(l,A1) == NOERR) {
		avancer();
		l=lexeme_courant();
		if (op_compar(l,A1) != NOERR){
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		avancer();
		l=lexeme_courant();
		if(expression(l,A1) != NOERR) {
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		return NOERR;
	}
	else if( l.nature == NOT){
		avancer();
		l=lexeme_courant();
		if( l.nature != PARO){
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		avancer();
		l=lexeme_courant();
		if(expression(l,A1) != NOERR) {
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		avancer();
		l=lexeme_courant();
		if (op_compar(l,A1) != NOERR){
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		avancer();
		l=lexeme_courant();
		if(expression(l,A1) != NOERR) {
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		avancer();
		l=lexeme_courant();
		if( l.nature != PARF){
			show_user_err(ERR_COMP,l);
			return ERR_COMP;
		}
		return NOERR;
	}
		show_user_err(ERR_COMP,l);
			return ERR_COMP;
						
}

static err_syntax seq_comparaison (Lexeme l,Ast A1){
	if (comparaison(l,A1)!= NOERR){
		show_user_err(ERR_SCOMP,l);
		return ERR_SCOMP;
	}
	avancer();
	l=lexeme_courant();
	if (suite_comparaison (l,A1)!= NOERR){
		show_user_err(ERR_SCOMP,l);
		return ERR_SCOMP;
	}
	return NOERR;	
}

static err_syntax op1log(Lexeme l){
  return NOERR;
}

static err_syntax op2log(Lexeme l){
  return NOERR;
}

static err_syntax op_compar(Lexeme l){
  return NOERR;
}



/* Autres fonctions plus ou moins utiles mais on y croit*/

static err_syntax fin(Lexeme l){
  while (l.nature == ERREUR){
    printf("Attention, on va continuer malgrÃ© : (%d,%d) %s\n",l.ligne,l.colonne,\
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
    printf("programme mal formÃ© \n");
    break;
  case ERR_INSTR :
    printf("instruction mal formÃ©e \n");
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
    printf("OpÃ©rande Ã  prioritÃ© basse incorrecte\n");
    break;
  case ERR_OP2:
    printf("OpÃ©rande Ã  prioritÃ© haute inccorrecte\n");
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
    printf("parenthese fermante oubliÃ© \n");
    break;
  default:
    break;
  }
}

static err_syntax fin_instr(Lexeme l, Ast *A1){
  avancer();
  if ( l.nature == FIN_INSTR)
    return NOERR;
  return ERR_FIN;
}

static err_syntax instr (Lexeme l,Ast* A1){
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

static err_syntax let (Lexeme l,Ast* A1){
  avancer();
  if (l.nature == LET)
    return NOERR;
  return ERR_LET;
}

static err_syntax suite_init (Lexeme l,Ast* A1){
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

static err_syntax init (Lexeme l,Ast* A1){
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
  l = lexeme_courant();
  e = type(l,A1);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_INIT;
  }
  return NOERR;
  
}
static err_syntax type (Lexeme l,Ast* A1){
  if (l.nature == TYPE)
    avancer();
  return NOERR;
}
