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
static err_syntax nocomment (Lexeme l);

//fonction preprocesseur permetant de raccourcir drastiquement le code ,point virgule à ajouter lors de l'appel
#define AVNC(x) 	do{\
						avancer();\
						l=lexeme_courant();\
						if (nocomment (x)!=NOERR)\
								return ERR_COMM ;\
						l=lexeme_courant();\
					}while(0)
//prend l'erreur de la sous fonction en e et l'erreur de la fonction actuelle en const ,point virgule à ajouter lors de l'appel
#define ERR(e,e_const,l) \
do{\
  if (e != NOERR){\
    show_user_err(e,l);\
    return e_const;\
  }\
}while(0)


// permet d'ignorer les commentaire et de verifier si ils sont correct
static err_syntax nocomment (Lexeme l){
	int i= 0 ;
	if (l.nature ==COMMENTO){
		i++;
		while(i!=0&&l.nature!=FIN_SEQ){
			if(l.nature ==COMMENTO)
				i++;
			if(l.nature==COMMENTF)
				i--;
			avancer();
			l=lexeme_courant();
			
		}
	}
	if (l.nature==FIN_SEQ && i!=0)
		return ERR_COMM;
	return NOERR;
}

//fonction global pour analyser syntaxiquement un fichier 
void analyser(char *nom_fichier, Ast* A1){
  err_syntax e = NOERR;
  Lexeme l;
  demarrer(nom_fichier);
  l = lexeme_courant();
	if (nocomment(l)!=NOERR){
		printf ("erreur critique: commentaire non fini dès le début du fichier #tusaispascoder\n");
		arreter();
		return;
	}
	l=lexeme_courant();
  e=programme(l, A1);
  if (e != NOERR){
    show_user_err(e,lexeme_courant());
    arreter();
    return;
  }
  l = lexeme_courant();
  fin(l);
  l=lexeme_courant();
  if (l.nature != FIN_SEQUENCE)
    printf("ERREUR FIN SEQ : (%d,%d) %s \n",l.ligne,l.colonne,l.chaine);
  arreter();
}

// Lecture du debut du lexeme ce referencer au fichier grammaire.tkt pour les fonction ci-dessous
static err_syntax programme (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  A1 = A1;
  e = seq_expression(l);
	ERR(e,ERR_PRG,l);
  l = lexeme_courant();
  if (l.nature != FIN_PRG)
    e = ERR_FPRG;
	AVNC(l);
	return e;
}

static err_syntax seq_expression(Lexeme l){
  err_syntax e = NOERR;
  e = expression(l,NULL);
 ERR(e,ERR_SEXPR,l);
  l=lexeme_courant();
  e = ss_expression(l);
 ERR(e,ERR_SEXPR,l);
  return NOERR;
}

static err_syntax ss_expression(Lexeme l){
  err_syntax e = NOERR;

  /* Cas epsilon */
  if (l.nature != FIN_EXPR)
    return NOERR;

  /* Cas suite d'expression */
AVNC(l);
  e = expression(l,NULL);
ERR(e,ERR_SSEXPR,l);
  l = lexeme_courant();
  e = ss_expression(l);
ERR(e,ERR_SSEXPR,l);
  return NOERR;
}

static err_syntax expression (Lexeme l,Ast* A1){	
	err_syntax e=NOERR;
	switch (l.nature) {
	case LET:
		e=expression(l,A1);
		ERR(e,ERR_EXPR,l);
		break;
	case IF:
		e=condition(l);
		ERR(e,ERR_EXPR,l);
		break;
	default:
		e=operation(l);
		ERR(e,ERR_EXPR,l);
		break;
	}
	return NOERR;
}



/* Definitions des affectations */

static err_syntax affectation (Lexeme l, Ast* A1) {
  err_syntax e = NOERR;
  if (l.nature != LET ) 
    return ERR_AFF;
	AVNC(l);
  e=seq_affect(l,A1)
	ERR(e,ERR_AFF,l);
  if (l.nature != IN ) 
    return ERR_AFF;
  avancer();
  l=lexeme_courant();
  if ((e=expression(l,A1)) != NOERR ) {
    show_user_err(e,l);
    return ERR_AFF;
  }	
}

static err_syntax affect (Lexeme l , Ast *A1){
  err_syntax e = NOERR;

  e = object(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_AFF;
  }
  l = lexeme_courant();
  if (l.nature != EQUAL){
    show_user_err(ERR_EQU,l);
    return ERR_AFF;
  }
  avancer();
  l=lexeme_courant();
  if ((e=expression(l,A1)) != NOERR) {
    show_user_err(e,l);
    return ERR_AFF;
  }
  return NOERR;
}

static err_syntax ss_affect (Lexeme l, Ast* A1){
  err_syntax e = NOERR;
  if (l.nature != AND)
    return NOERR;
  
  avancer();
  l=lexeme_courant();
  if ((e=affect(l,A1)) != NOERR ) {
    show_user_err(e,l);
    return ERR_SSAFF;
  }
  
  l=lexeme_courant();
  if ((e=ss_affect(l,A1)) != NOERR ) {
    show_user_err(e,l);
    return ERR_SSAFF;
  }
  return NOERR;
}

static err_syntax seq_affect (Lexeme l, Ast *A1){
  err_syntax e = NOERR;
  if ((e=affect(l,A1)) != NOERR ) {
    show_user_err(e,l);
    return ERR_SAFF;
  }
  
  l=lexeme_courant();
  if ((e=ss_affect(l,A1)) != NOERR ) {
    show_user_err(e,l);
    return ERR_SAFF;
  }
  return NOERR;  
}

static err_syntax object(Lexeme l){
  err_syntax e = NOERR;

  e = nom_var(l);

  if (e != NOERR){
    show_user_err(e,l);
    return ERR_OBJ;
  }

  l = lexeme_courant();

  e = seq_fparam(l);

  if (e != NOERR){
    show_user_err(e,l);
    return ERR_OBJ;
  }
}

static err_syntax seq_fparam(Lexeme l){
  err_syntax e = NOERR;
  e= fparam(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFPARAM;
  }
  
  l=lexeme_courant();
  e= ss_fparam(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SFPARAM;
  }
  return NOERR;
}

static err_syntax ss_fparam(Lexeme l){
  err_syntax e = NOERR;
  
  e= fparam(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSFPARAM;
  }
  
  l=lexeme_courant();
  e= ss_fparam(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSFPARAM;
  }
  return NOERR;
}

static err_syntax fparam(Lexeme l)
{
  err_syntax e = NOERR;
  if( (e =nom_var(l)) != NOERR){
    show_user_err(e,l);
    return ERR_FPARAM;
  }
  return NOERR;
}



/* Definitions des opérations */

static err_syntax operation (Lexeme l ,Ast* A1) {
	if ( seq_terme(l,A1)!= NOERR){
		show_user_err(e,l);
		return ERROP;
	}
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
  
  //lecture et construction d'un op1 on supoose ss-therme = epsilon si op faux
  e= op1(l, &Op);
  if (e != NOERR){
    return NOERR;
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

static err_syntax facteur  (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  char is_minus = 0;
  if (l.nature==MOINS){
    AVNC(l)
    is_minus = 1;
  }
  e = valeur(l);
  if (e != NOERR){
      show_user_err(e,l);
      return ERR_FACTEUR;
  }
  return NOERR;
	
}
//inutil
static err_syntax opunaire (Lexeme l,Ast* A1, char is_minus) {
  Ast A2 = NULL;
  err_syntax e = NOERR;

  e = valeur(l);
  if (e != NOERR){
      show_user_err(e,l);
      return ERR_FACTEUR;
    }
  
  /*
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
  */
  avancer();
  if (is_minus)
    *A1 = creer_operation(U_MOINS,NULL,A2);
  else
    *A1 = A2;
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
  AVNC(l);
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
  AVNC(l);
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

/* Definitions des valeurs */

static err_syntax valeur (Lexeme l, Ast* A1){
  err_syntax e = NOERR;

  switch(l.nature){
  case NUM:
    break;
  case STRING:
    break;
  default:
    e=identificateur(l);
    if (e != NOERR){
      show_user_err(e,l);
      return ERR_VAL;
    }
  }
  //Cas constante
  avancer();
  return NOERR;
}

static err_syntax identificateur(Lexeme l){
  err_syntax e = NOERR;

  e = nom_var(l);

  if (e != NOERR){
    show_user_err(e,l);
    return ERR_IDF;
  }

  l = lexeme_courant();
  e = seq_param(l);

  if (e != NOERR){
    show_user_err(e,l);
    return ERR_IDF;
  }

  return NOERR;
  
}

static err_syntax seq_param(Lexeme l){

  err_syntax e = NOERR;
  e= param(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SPARAM;
  }
  
  //lecture d'une ssterme
  l=lexeme_courant();
  e= ss_param(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SPARAM;
  }
  return NOERR;
}

static err_syntax ss_param(Lexeme l){
  err_syntax e = NOERR;
  
  e= param(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSPARAM;
  }
  
  l=lexeme_courant();
  e= ss_param(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SSPARAM;
  }
  return NOERR;
}
/*Actuellement on ne donne en parametre que variables et constantes, 
dans l'idéal faudrait pouvoir donner des appels de fonctions
donc revoir la structure pour ne lire que le nombre d'arguments nécessaires........ bizarre */
static err_syntax param(Lexeme l){
  err_syntax e = NOERR;
  switch(l.nature){
  case NUM:
  case STRING:
    avancer();
    break;
  default:
    if( (e =nom_var(l)) != NOERR){
      show_user_err(e,l);
      return ERR_PARAM;
    }
    break;
  }
  return NOERR;
}

static err_syntax nom_var(Lexeme l){
  if (l.nature != VAR){
    show_user_err(ERR_NOMVAR,l);
    return ERR_VAR;
  }
  return NOERR;
}

/* Definitions des conditions */

static err_syntax condition (Lexeme l , Ast* A1 ) {
  if (l.nature != IF ){
    show_user_err(ERR_COND,l);
    return ERR_COND;
  }
  AVNC(l);
  if(seq_boolor(l,A1) != NOERR) {
    show_user_err(ERR_COND,l);
    return ERR_COND;
  }
AVNC(l);
  if (l.nature != THEN ){
    show_user_err(ERR_COND,l);
    return ERR_COND;
  }
AVNC(l);
  if(expression(l,A1) != NOERR) {
    show_user_err(ERR_COND,l);
    return ERR_COND;
  }
AVNCL(l);
  if(suite_condition(l,A1) != NOERR) {
    show_user_err(ERR_COND,l);
    return ERR_COND;
  }
  return NOERR;
}

static err_syntax suite_condition (Lexeme l,Ast*A1){
	if (l.nature!=ELSE)
		return NOERR;
	AVNC(l);
	if(expression(l)!=NOERR){
		    show_user_err(ERR_COND,l);
    return ERR_SCOND;
	}	
  return NOERR;
}

static err_syntax seq_boolor(Lexeme l){
	if(boolor(l)==NOERR){
		l=lexeme_courant();
		if(ss_boolor(l)!=NOERR){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
		return NOERR;
	}
	if(l.nature!=PARO){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	AVNC(l);
	if(boolor(l)!=NOERR)
	{
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	l=lexeme_courant();
	if(l.nature!=PARF){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	AVNC(l);
		if(ss_boolor(l)!=NOERR)
	{
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}	
  return NOERR;
}

static err_syntax ss_boolor(Lexeme l){
  if(l.nature!=LOGIC_OR){
    return NOERR;
  }
  AVNC(l);
  if(boolor(l)!=NOERR)
    {
      show_user_err(ERR_COND,l);
      return ERR_SSBOOL;
    }	
  l=lexeme_courant();
  if(ss_boolor(l)!=NOERR)
    {
      show_user_err(ERR_COND,l);
      return ERR_SSBOOL;
    }	
  return NOERR;
}

static err_syntax boolor(Lexeme l){
	if (l.nature ==NOT){
		AVNC(l);
	}
	if(saq_booland(l)!=NOERR)
		{
			show_user_err(ERR_BOOL,l);
    		return ERR_BOOL;
	}	
  return NOERR;
}

static err_syntax seq_booland(Lexeme l){
		if(booland(l)==NOERR){
		l=lexeme_courant();
		if(ss_booland(l)!=NOERR){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
		return NOERR;
	}
	if(l.nature!=PARO){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	AVNC(l);
	if(booland(l)!=NOERR)
	{
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	l=lexeme_courant();
	if(l.nature!=PARF){
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}
	AVNC(l);
		if(ss_booland(l)!=NOERR)
	{
			show_user_err(ERR_COND,l);
    		return ERR_SBOOL;
	}	
  return NOERR;
}

static err_syntax ss_booland(Lexeme l){
	if(l.nature!=LOGIC_AND){
		return NOERR;
	}
	AVNC(l);
	if(booland(l)!=NOERR)
			{
			show_user_err(ERR_COND,l);
    		return ERR_SSBOOL;
	}	
	l=lexeme_courant();
		if(ss_booland(l)!=NOERR)
			{
			show_user_err(ERR_COND,l);
    		return ERR_SSBOOL;
	}	
  return NOERR;
}


static err_syntax booland(Lexeme l){
	if (l.nature ==NOT){
		AVNC(l);			
	}
	if(l.nature==VALEUR)
		return NOERR
	if (comparaison(l)==NOERR)
		return NOERR
	show_user_err(ERR_SSBOOL,l);
    return ERR_SSBOOL;		
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
	nocomment(l);
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
