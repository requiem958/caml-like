/* ------------------------------------------------------------------------
-- module analyse_lexicale
--
-- analyse lexicale d'une expression arithmetique
--
-- J-P. Peyrin, G. Serasset (version initiale) : janvier 1999
-- P.Habraken - 18/01/2010
--
-- 10 juin 2006 - PH :
-- remplacement record a champs variants par record "plat"
-- remplacement traitement iteratif par automate
--
-- Aout 2016 - LM : version C
------------------------------------------------------------------------ */

#include <string.h>
#include <ctype.h>
#include <stdlib.h> 
#include <stdio.h> 

#include "lecture_caracteres.h"
#include "analyse_lexicale.h"
/* --------------------------------------------------------------------- */


/* Les variables et fonctions locales au module */

Lexeme lexeme_en_cours ;	/* le lexeme courant */

void ajouter_caractere (char *s, char c);
int est_separateur(char c ) ;
int est_chiffre(char c ) ;
int est_symbole(char c ) ;
void reconnaitre_lexeme();
Nature_Lexeme symb_to_lex(char *chaine);
Nature_Lexeme identificateur(char *chaine);
/* --------------------------------------------------------------------- */

void demarrer(char *nom_fichier) {
  demarrer_car(nom_fichier);
  avancer();
}

/* --------------------------------------------------------------------- */

void avancer() {
  reconnaitre_lexeme();
}

/* --------------------------------------------------------------------- */

Lexeme lexeme_courant() {
  return lexeme_en_cours;
}

/* --------------------------------------------------------------------- */

int fin_de_sequence() {
  return lexeme_en_cours.nature == FIN_SEQUENCE;
}

/* --------------------------------------------------------------------- */

void arreter() {
  arreter_car() ;
}

/* --------------------------------------------------------------------- */


// reconnaissance d'un nouveau lexeme
// etat initial : le caractere courant est soit separateur 
//                soit le 1er caractere d'un lexeme
// etat final : 
//       - un nouveau lexeme est reconnu dans lexeme_en_cours
//       - le caractere courant est soit la fin de fichier, 
//		soit un separateur,  soit le 1er caractere d'un lexeme

void reconnaitre_lexeme() {
  typedef enum {E_INIT, E_ENTIER, E_FIN, E_FIN_INSTR, E_CAR,E_FLOAT} Etat_Automate ;
  Etat_Automate etat=E_INIT;
  float exp=0.1;
  lexeme_en_cours.valeur.val_f=0;
  
  // on commence par lire les separateurs
  while (est_separateur(caractere_courant())) {
    avancer_car();
  }
  lexeme_en_cours.chaine[0] = '\0' ;

  // on utilise ensuite un automate pour reconnaitre le prochain lexeme

  while (etat != E_FIN) {
	  switch (etat) {
		  case E_INIT:
			  if (fin_de_sequence_car() != 0) {
				  lexeme_en_cours.nature = FIN_SEQUENCE;
				  etat = E_FIN;
			  }
			  else {
				  lexeme_en_cours.ligne = numero_ligne();
				  lexeme_en_cours.colonne = numero_colonne();
				  ajouter_caractere(lexeme_en_cours.chaine, caractere_courant());

				  //Reconnaissance fin de l'instruction
				  if (caractere_courant() == ';'){
					  lexeme_en_cours.nature = FIN_INSTR;
					  etat = E_FIN_INSTR;
				  }
				  //Reconnaissance Numérique
				  else if (caractere_courant() == '.'){
					  lexeme_en_cours.nature = NUM;
					  etat = E_FLOAT;
				  }
				  else if (est_chiffre(caractere_courant())) {
					  lexeme_en_cours.nature = NUM;
					  lexeme_en_cours.valeur.val_i = caractere_courant() - '0';
					  etat = E_ENTIER;
				  }
				  //Reconnaissance opérateur symbole
				  else if  (est_symbole(caractere_courant())) {
					  avancer_car();
					  while (est_symbole(caractere_courant()) ){
						  ajouter_caractere( lexeme_en_cours.chaine, caractere_courant() );
						  avancer_car();
					  }
					  lexeme_en_cours.nature = symb_to_lex(lexeme_en_cours.chaine);
					  etat = E_FIN;
				  }
				  //Reconnaissance chaine de caractères sans symboles
				  else {
					  lexeme_en_cours.chaine[strlen(lexeme_en_cours.chaine)] = '\0';
					  avancer_car();
					  while (!est_separateur(caractere_courant()) 	&&\
							 !fin_de_sequence_car() 			&&\
							 !est_symbole(caractere_courant()) 	&&\
							 !(caractere_courant() == ';')
							){
						  ajouter_caractere (lexeme_en_cours.chaine,\
											 tolower(caractere_courant())) ;
						  avancer_car();
					  }
					  ajouter_caractere( lexeme_en_cours.chaine, '\0');
					  etat = E_CAR;
					  goto car;
				  }
			  }
			  avancer_car() ;
			  break ;
		  case E_ENTIER:
			  lexeme_en_cours.type = INT;
			  if (!est_chiffre(caractere_courant()) && !(caractere_courant()=='.'))
				  etat=E_FIN;
			  else{
				  ajouter_caractere(lexeme_en_cours.chaine,caractere_courant());
				  if (est_chiffre(caractere_courant())) {
					  lexeme_en_cours.valeur.val_i *= 10 ;
					  lexeme_en_cours.valeur.val_i += caractere_courant() - '0';
					  etat = E_ENTIER;
				  }
				  else if (caractere_courant()== '.'){
					  lexeme_en_cours.valeur.val_f=lexeme_en_cours.valeur.val_i;
					  etat=E_FLOAT;
				  }
				  avancer_car();
			  }
			  break;
		  case E_FLOAT:
			  lexeme_en_cours.type = FLOAT;
			  if (est_chiffre(caractere_courant())){
				  lexeme_en_cours.valeur.val_f += (caractere_courant() -'0')*exp;
				  ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
				  exp=exp*0.1;
				  avancer_car ();
			  }
			  else
				  etat=E_FIN;
			  break;
car:
		  case E_CAR:
			  lexeme_en_cours.nature = identificateur(lexeme_en_cours.chaine);;
			  etat = E_FIN;
			  break;
		  case E_FIN_INSTR: //On ne considère plus rien après ;; (ocaml sur turing)
			  ajouter_caractere (lexeme_en_cours.chaine, caractere_courant()) ;
			  if (strcmp(lexeme_en_cours.chaine, ";;") != 0 )
				  lexeme_en_cours.nature = ERREUR;
			  else
				  lexeme_en_cours.nature = FIN_INSTR;
			  avancer_car();
			  etat = E_FIN;
			  break;
		  case E_FIN:
			  break ;
	  }
  }
}

/* --------------------------------------------------------------------- */

#define IF_IDF(x) strcmp(chaine,x) == 0)

Nature_Lexeme identificateur(char *chaine){
  IF_IDF("let")
    return LET;
  else IF_IDF("and")
    return AND;
	else IF_IDF("or")
		return LOGIC_OR;
  else IF_IDF(":bool")
    return TYPE;
  else IF_IDF(":int")
    return TYPE;
  else IF_IDF(":float")
    return TYPE;
  else IF_IDF(":string")
    return TYPE;
  else IF_IDF("in")
    return IN;
  else IF_IDF("function")
    return FUNCTION;
  else IF_IDF("begin")
    return PARO;
  else IF_IDF("end")
    return PARF;
  else
    return VAR;
}

Nature_Lexeme symb_to_lex(char *chaine){
	IF_IDF('+')
		return PLUS;
	else IF_IDF('-')
		return MOINS;
	else IF_IDF('*')
		return MUL;
	else IF_IDF('/')
		return DIV;
	else IF_IDF('&')
		return LOGIC_AND;
	else IF_IDF(')')
		return PARF;
	else IF_IDF('(')
		return PARO;
	else IF_IDF('=')
		return EQUAL;
	else IF_IDF('>')
		return GREATER_THAN;
	else IF_IDF('<')
		return LOWER_THAN;
	else IF_IDF('>=')
		return GREATER_OR_EQUAL;
	else IF_IDF('<=')
		return LOWER_OR_EQUAL;
	else IF_IDF('!=')
		return INEQUALITY;
	else
		return ERREUR;
}
// cette fonction ajoute le caractere c a la fin de la chaine s 
// (la chaine s est donc modifiee)

void ajouter_caractere (char *s, char c) {
    int l ;

    l = strlen(s) ;
    s[l] = c ;
    s[l+1] = '\0' ;
} ;

/* --------------------------------------------------------------------- */

// vaut vrai ssi c designe un caractere separateur
int est_separateur(char c) { 
  return c == ' ' || c == '\t' || c == '\n' ;
}

/* --------------------------------------------------------------------- */

// vaut vrai ssi c designe un caractere chiffre
int est_chiffre(char c) {
  return c >= '0' && c <= '9' ;
}

/* --------------------------------------------------------------------- */

// vaut vrai ssi c designe un caractere correspondant a un symbole arithmetique
int est_symbole(char c)  {
	switch (c) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '(':
		case ')':
		case '=':
		case '<':
	  	case '>':
		case '!':
		case '&':
		case '|':
			return 1;
		default:
    	return 0;
  } 
}

/* --------------------------------------------------------------------- */

// renvoie la chaine de caracteres correspondant a la nature du lexeme

#define LEX_NAT_CHN(x) case x: return #x
#define 

char *Nature_vers_Chaine (Nature_Lexeme nature) {
	switch (nature) {
			LEX_NAT_CHN(PLUS);             // +
			LEX_NAT_CHN(MOINS);            // -
			LEX_NAT_CHN(MUL);		           // *
  		LEX_NAT_CHN(DIV);              // /
  
  //Logic op
  		LEX_NAT_CHN(LOGIC_AND);
  		LEX_NAT_CHN(LOGIC_OR);
  		LEX_NAT_CHN(NOT);
  
  //Comparison ops
  		LEX_NAT_CHN(EQUALITY);
  		LEX_NAT_CHN(INEQUALITY);
  		LEX_NAT_CHN(GREATER_THAN);
  		LEX_NAT_CHN(LESS_THAN);
  		LEX_NAT_CHN(GREAT_OR_EQUAL);
  		LEX_NAT_CHN(LESS_OR_EQUAL);
  
  //Delimiters
  		LEX_NAT_CHN(PARO);    //(
  		LEX_NAT_CHN(PARF);    //)
  
  //Various
  		LEX_NAT_CHN(LET); //let
  		LEX_NAT_CHN(AND); //and
  		LEX_NAT_CHN(IN);// in
  		LEX_NAT_CHN(EQUAL); //=
  		LEX_NAT_CHN(IF);
  		LEX_NAT_CHN(THEN);
  		LEX_NAT(ELSE);
  
  //Data types
  		LEX_NAT_CHN(VAR); //Variable
  		LEX_NAT_CHN(FUNCTION); //fonction
  		LEX_NAT_CHN(TYPE); //Data type
  		LEX_NAT_CHN(NUM);// sequence de chiffres
  
  //control lex
			LEX_NAT_CHN(FIN_INSTR); //;;
			LEX_NAT_CHN(FIN_SEQUENCE);
			default: return "ERREUR" ;
  }
} 

/* --------------------------------------------------------------------- */

// affiche a l'ecran le lexeme l
void afficher(Lexeme l) {

  switch (l.nature) {
  case FIN_SEQUENCE:
    break;
  default:
    printf("(ligne %d, ", l.ligne);
    printf("colonne %d) : ",l.colonne);
    printf("[") ;
    printf("nature = %s", Nature_vers_Chaine(l.nature)) ;
    printf(", chaine = %s", l.chaine) ;
    switch(l.nature) {
    case NUM:
      switch(l.type){
      case INT:
	printf(", valeur = %d", l.valeur.val_i);
	break;
      case FLOAT:
	printf(", valeur = %lf", l.valeur.val_f);
	break;
      }
    default:
      break;
    }
    printf("]\n") ;
  }
}

/* --------------------------------------------------------------------- */

