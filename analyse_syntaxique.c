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
static err_syntax identificateur(Lexeme l);

static err_syntax nom_var(Lexeme l);

static err_syntax seq_param(Lexeme l);
//static err_syntax ss_param(Lexeme l);
//static err_syntax param(Lexeme l);

/* Sous expressions de affectation */

static err_syntax seq_affect(Lexeme l, Ast* A1);
static err_syntax ss_affect(Lexeme l,Ast *A1);
static err_syntax affect(Lexeme l, Ast *A1);

static err_syntax object(Lexeme l);

static err_syntax seq_fparam(Lexeme l);
//static err_syntax ss_fparam(Lexeme l);
//static err_syntax fparam(Lexeme l);

/* Sous expressions de operation */

//arithmetic op
static err_syntax op1 (Lexeme l,TypeOperateur*t);
static err_syntax op2 (Lexeme l,TypeOperateur*t);

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

static err_syntax op_compar(Lexeme l,Ast*A1);

/* Enlever les lexemes de types ERREUR à la fin du programme */
static err_syntax fin(Lexeme l);

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
  if (l.nature != FIN_SEQ)
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
		e=affectation(l,A1);
		ERR(e,ERR_EXPR,l);
		break;
	case IF:
	  e=condition(l,NULL);
		ERR(e,ERR_EXPR,l);
		break;
	default:
	  e=operation(l,NULL);
		ERR(e,ERR_EXPR,l);
		break;
	}
	return NOERR;
}



/* Definitions des affectations */

static err_syntax affectation (Lexeme l, Ast* A1) {
  err_syntax e = NOERR;
  if (l.nature != LET ) 
    return ERR_LET;
	AVNC(l);
  e=seq_affect(l,A1);
ERR(e,ERR_AFF,l);
  if (l.nature != IN ) 
    return ERR_AFFE;
AVNC(l);
 e=expression(l,A1);
ERR(e,ERR_AFF,l);
	return NOERR;
}

static err_syntax seq_affect (Lexeme l, Ast *A1){
  err_syntax e = NOERR;
  A1=A1;
  e = affect(l,NULL);
 ERR(e,ERR_SAFF,l);
  l=lexeme_courant();
  e = ss_affect(l,NULL);
 ERR(e,ERR_SAFF,l);
  return NOERR;
}

static err_syntax ss_affect (Lexeme l, Ast* A1){
  err_syntax e = NOERR;
  A1=A1;

  /* Cas epsilon */
  if (l.nature != AND)
    return NOERR;

  /* Cas suite d'expression */
AVNC(l);
  e = affect(l,NULL);
ERR(e,ERR_SSAFF,l);
  l = lexeme_courant();
  e = ss_affect(l,NULL);
ERR(e,ERR_SSAFF,l);
  return NOERR;
}


static err_syntax affect (Lexeme l , Ast *A1){
  err_syntax e = NOERR;

  e = object(l);
  ERR(e,ERR_AFF,l);
  l = lexeme_courant();
  if (l.nature != EQUAL){
    show_user_err(ERR_EQU,l);
    return ERR_AFF;
  }
  AVNC(l);
  e=expression(l,A1);
  ERR(e,ERR_AFF,l);
  return NOERR;
}


static err_syntax object(Lexeme l){
  err_syntax e = NOERR;
  e = nom_var(l);
ERR(e,ERR_OBJ,l);
  l = lexeme_courant();
  e = seq_fparam(l);
 ERR(e,ERR_OBJ,l);
return NOERR;
}

static err_syntax seq_fparam(Lexeme l){
 /* err_syntax e = NOERR;
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
  }*/
	l=l;
  return NOERR;
}
/*
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
	l=l;
  return NOERR;
}

static err_syntax fparam(Lexeme l)
{
 err_syntax e = NOERR;
  if( (e =nom_var(l)) != NOERR){
    show_user_err(e,l);
    return ERR_FPARAM;
  }
	l=l;
  return NOERR;
}
*/


/* Definitions des opérations */

static err_syntax operation (Lexeme l ,Ast* A1) {
	err_syntax e=NOERR;
	e=seq_terme(l,A1);
	ERR(e,ERR_OPE,l);
	return NOERR;
}

static err_syntax seq_terme(Lexeme l,Ast* A2){
  Ast A1;
  err_syntax e = NOERR;
  e= terme(l,&A1);
 ERR(e,ERR_STERME,l); 
 l=lexeme_courant();
  e= ss_terme(l,A2, A1);
ERR(e,ERR_STERME,l);
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
ERR(e,ERR_SSTERME,l);
  A4 = creer_operation(Op,A1,A3);

  //lecture et construction de la suite des termes si existante
  l=lexeme_courant();
  e= ss_terme(l,A2,A4);
ERR(e,ERR_SSTERME,l);
  return NOERR;
}

static err_syntax terme (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  // lecture d'une séquence de facteur et on parle pas d'une armée de postiers
  e= seq_facteur(l,A1);
ERR(e,ERR_TERME,l);
  return NOERR;
}

static err_syntax seq_facteur (Lexeme l,Ast* A2){
  Ast A1;
  err_syntax e = NOERR;
  
  e = facteur(l,&A1);
 ERR(e,ERR_SFACTEUR,l);
  l=lexeme_courant();
  e = ss_facteur(l,A2, A1);
 ERR(e,ERR_SFACTEUR,l);
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
  ERR(e,ERR_SSFACTEUR,l);
  A4 = creer_operation(Op,A1,A3);
  
  //Lecture des facteurs restants
  l=lexeme_courant();
  e=ss_facteur(l, A2, A4);
  ERR(e,ERR_SSFACTEUR,l);
  return NOERR;
}

static err_syntax facteur  (Lexeme l,Ast* A1){
  err_syntax e = NOERR;
  char is_minus = 0;
  if(is_minus)is_minus++;
  A1=A1;
  if (l.nature==MOINS){
    AVNC(l);
    is_minus = 1;
  }
if (l.nature == PARO){
	  AVNC(l);
	  e=operation(l,NULL);
	 ERR(e,ERR_FACTEUR,l);
	  l=lexeme_courant();
	  if ( l.nature != PARF){
		ERR(ERR_PARF,ERR_FACTEUR,l);
	  }
	 AVNC(l);
	return NOERR;
}
//Attention au minus
  e = valeur(l,NULL);
 ERR(e,ERR_FACTEUR,l);
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
  A1=A1;
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
  AVNC(l);
  return NOERR;
}

static err_syntax identificateur(Lexeme l){
  err_syntax e = NOERR;

  e = nom_var(l);
ERR(e,ERR_IDF,l);
  l = lexeme_courant();
  e = seq_param(l);
ERR(e,ERR_IDF,l);
  return NOERR;
  
}

static err_syntax seq_param(Lexeme l){

  /*err_syntax e = NOERR;
  e= param(l);
 ERR(e,ERR_SPARAM,l);
  
  //lecture d'une ssterme
  l=lexeme_courant();
  e= ss_param(l);
  if (e != NOERR){
    show_user_err(e,l);
    return ERR_SPARAM;
  }*/
	l=l;
  return NOERR;
}
/*
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
  l=l;
  return NOERR;
}
//Actuellement on ne donne en parametre que variables et constantes, 
//dans l'idéal faudrait pouvoir donner des appels de fonctions
//donc revoir la structure pour ne lire que le nombre d'arguments nécessaires..
//bizarre
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
	l=l;
  return NOERR;
}
*/
static err_syntax nom_var(Lexeme l){
  if (l.nature != VAR){
    return ERR_NOMVAR;
  }
	AVNC(l);
  return NOERR;
}

/* Definitions des conditions */

static err_syntax condition (Lexeme l , Ast* A1 ) {
	err_syntax e=NOERR;
  if (l.nature != IF ){
    return ERR_COND;
  }
  AVNC(l);
  e= seq_boolor(l) ;
    ERR(e,ERR_COND,l);
  if (l.nature != THEN ){
    show_user_err(ERR_THEN,l);
    return ERR_COND;
  }
AVNC(l);
 e=expression(l,A1) ;
 ERR(e,ERR_COND,l);
 e=suite_condition(l,A1);
 ERR(e,ERR_COND,l);
  return NOERR;
}

static err_syntax suite_condition (Lexeme l,Ast*A1){
	err_syntax e=NOERR;
	A1=A1;
	if (l.nature!=ELSE)
		return NOERR;
	AVNC(l);
	e=expression(l,NULL);
	ERR(e,ERR_SCOND,l);
  return NOERR;
}

static err_syntax seq_boolor(Lexeme l){
	err_syntax e=NOERR;
	e=boolor(l);
	ERR(e,ERR_SBOOLEAU,l);
	l=lexeme_courant();
	e=ss_boolor(l);
	ERR(e,ERR_SBOOLEAU,l);		
	return NOERR;
}

static err_syntax ss_boolor(Lexeme l){
	err_syntax e=NOERR;
  if(l.nature!=LOGIC_OR){
    return NOERR;
  }
  AVNC(l);
  e= boolor(l);
ERR(e,ERR_SSBOOLEAU,l);	
  l=lexeme_courant();
  e=ss_boolor(l);
ERR(e,ERR_SSBOOLEAU,l);	
  return NOERR;
}

static err_syntax boolor(Lexeme l){
	err_syntax e=NOERR;
	if (l.nature ==NOT){
		AVNC(l);
	}
	e=seq_booland(l);
	ERR(e,ERR_BOOLEAU,l);		
  return NOERR;
}
/* booland le parc d'attraction de tout les booléen age<99 && age >0 */ 
static err_syntax seq_booland(Lexeme l){
	err_syntax e=NOERR;
	e =booland(l);
	ERR(e,ERR_SBOOLAND,l);	
	l=lexeme_courant();
	e=ss_booland(l);
	ERR(e,ERR_SBOOLAND,l);	
	return NOERR;
}

static err_syntax ss_booland(Lexeme l){
	err_syntax e=NOERR;
	if(l.nature!=LOGIC_AND)
		return NOERR;
	AVNC(l);
	e=booland(l);
	ERR(e,ERR_SSBOOLAND,l);	
	l=lexeme_courant();
	e=ss_booland(l);	
	ERR(e,ERR_SSBOOLAND,l);	
 	return NOERR;
}

static err_syntax booland(Lexeme l){
	err_syntax e=NOERR;
	if (l.nature ==NOT){
		AVNC(l);			
	}
	if(l.nature==VAR||l.nature== NUM){
	  valeur(l,NULL);
	  return NOERR;
	}
	e=comparaison(l,NULL);
	ERR(e,ERR_BOOLAND,l);
    return NOERR;		
}

static err_syntax comparaison (Lexeme l , Ast *A1){
	err_syntax e=NOERR;
	A1=A1;
	if (l.nature != PARO)
		return ERR_COMP;
	AVNC(l);
	e=expression(l,NULL);
	ERR(e,ERR_COMP,l);
	l=lexeme_courant();
	e=op_compar(l,NULL);
	ERR(e,ERR_COMP,l);		
	l=lexeme_courant();
	e=expression(l,A1);
	ERR(e,ERR_COMP,l);
	l=lexeme_courant();
	if (l.nature != PARF)
		return ERR_COMP;
	AVNC(l);
	return NOERR;
}

static err_syntax op_compar(Lexeme l, Ast*A1){
  A1=A1;
	switch(l.nature){
		case EQUALITY:
			break;
		case INEQUALITY:
			break;
		case GREATER_THAN:
			break;
		case LOWER_THAN:
			break;
		case LOWER_OR_EQUAL:
			break;
		case GREATER_OR_EQUAL:
			break;
			default :
			return ERR_OPCOMP;
	}
	AVNC(l);
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

#define STR_ERR(e,m) \
case e:\
printf(m);\
break

void print_err(const err_syntax e){
  switch(e){
STR_ERR( ERR_PRG,"programme mal forme \n");
STR_ERR(ERR_FPRG," ptdr tu sais pas finir des programmes \n");
STR_ERR( ERR_COMM," commentaire NON FERME \n");
STR_ERR(ERR_SEXPR," sequence d'expression incorect \n");
STR_ERR(ERR_SSEXPR,"suite de sequence d'expression incorect \n");
STR_ERR(ERR_EXPR,"expression incorect \n");
STR_ERR(ERR_AFFE,"affectation incorect \n");
STR_ERR(ERR_SAFF,"sequence d'affectation incorect \n");
STR_ERR(ERR_SSAFF,"Suite de sequence d'affectation incorect \n");
STR_ERR(ERR_AFF," affectation solitaire incorect \n");
STR_ERR(ERR_EQU," egale oublie \n");
STR_ERR(ERR_OBJ," affectation impossible\n");
STR_ERR(ERR_OPE," operation incorect \n");
STR_ERR ( ERR_STERME ,"sequence de terme non valide \n");
STR_ERR ( ERR_SSTERME,"suite de la sequence de terme \n");
STR_ERR( ERR_TERME,"terme incorect et on parle pas des bains\n");
STR_ERR( ERR_SFACTEUR,"sequence de facteur incorect\n");
STR_ERR( ERR_SSFACTEUR,"suite de sequence de facteur incorect \n");
STR_ERR( ERR_FACTEUR,"facteur incorect \n");
STR_ERR( ERR_VAL,"valeur incorrect \n");
STR_ERR(ERR_NOMVAR," nom de variable impossible\n");
STR_ERR(ERR_IDF,"identificateur incorect \n");
STR_ERR(ERR_COND,"condition incorect \n ");
STR_ERR(ERR_THEN," le Then à disparu.... disparu ...disparu ...\n");
STR_ERR(ERR_SCOND,"la suite de condition est incorect \n" );
STR_ERR(ERR_SBOOLEAU," la sequence de booléen de 'or' est incorect \n");
STR_ERR(ERR_SSBOOLEAU," la suite de sequence de booléen de 'or' est incorect \n");
STR_ERR(ERR_BOOLEAU," le booléen ou est incorect \n");
STR_ERR(ERR_LOR," operateur 'or' manquant\n");
STR_ERR(ERR_SBOOLAND," la sequence de booléen de 'and' est incorect \n");
STR_ERR(ERR_SSBOOLAND," la suite de sequence de booléen de 'and' est incorect \n");
STR_ERR(ERR_BOOLAND," le booléen 'and' est incorect \n");
STR_ERR(ERR_COMP,"comparaison incorect \n");
STR_ERR(ERR_OP1,"Operande a  priorite basse incorrecte\n");
STR_ERR(ERR_OP2,"Operande a  priorite haute inccorrecte\n");
STR_ERR(ERR_LET,"manque de let \n");
STR_ERR(ERR_PARF,"parenthese fermante oubliee \n");
  default:
    break;
  }
}
