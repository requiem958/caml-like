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

	if (caractere_courant() == ';'){
	  lexeme_en_cours.nature = FIN_INSTR;
	  etat = E_FIN_INSTR;
	}
	else if (caractere_courant() == '.'){
	  lexeme_en_cours.nature = NUM;
	  etat = E_FLOAT;
	}
	else if (est_chiffre(caractere_courant())) {
	  lexeme_en_cours.nature = NUM;
	  lexeme_en_cours.valeur.val_i = caractere_courant() - '0';
	  etat = E_ENTIER;
	}
	else if  (est_symbole(caractere_courant())) {
	  switch (caractere_courant()){
	  case '+':
	    lexeme_en_cours.nature = PLUS;
	    break;
	  case '-':
	    lexeme_en_cours.nature = MOINS;
	    break;
	  case '*':
	    lexeme_en_cours.nature = MUL;
	    break;
	  case '/':
	    lexeme_en_cours.nature = DIV;
	    break;
	  case ')':
	    lexeme_en_cours.nature = PARF;
	    break;
	  case '(':
	    lexeme_en_cours.nature = PARO;
	    break;
	  case '=':
	    lexeme_en_cours.nature = EQUAL;
	    break;
	  default:
	    lexeme_en_cours.nature = ERREUR;
	    break;
	  }
	  etat = E_FIN;
	}
	else {
	  lexeme_en_cours.chaine[strlen(lexeme_en_cours.chaine)] = '\0';
	  avancer_car();
	  while (!est_separateur(caractere_courant()) &&		\
		 !fin_de_sequence_car() &&				\
		 !est_symbole(caractere_courant()) &&			\
		 !(caractere_courant() == ';')
		 ){
	    ajouter_caractere (lexeme_en_cours.chaine,			\
			       tolower(caractere_courant())) ;
	    avancer_car();
	  }
	  ajouter_caractere( lexeme_en_cours.chaine, 0);
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

Nature_Lexeme identificateur(char *chaine){
  if (strcmp(chaine,"let") == 0)
    return LET;
  else if (strcmp(chaine,"and") == 0)
    return AND;
  else if ( strcmp(chaine,":bool") == 0 )
    return TYPE;
  else if ( strcmp(chaine,":int") == 0 )
    return TYPE;
  else if ( strcmp(chaine,":float") == 0 )
    return TYPE;
  else if ( strcmp(chaine,":string") == 0 )
    return TYPE;
  else if ( strcmp(chaine,"in") == 0 )
    return IN;
  else if ( strcmp(chaine,"function") == 0 )
    return FUNCTION;
  else if ( strcmp(chaine,"begin") == 0 )
    return PARO;
  else if ( strcmp(chaine,"end") == 0 )
    return PARF;
  else
    return VAR;
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
    return 1;

  default:
    return 0;
  } 
}

/* --------------------------------------------------------------------- */

// renvoie la chaine de caracteres correspondant a la nature du lexeme
char *Nature_vers_Chaine (Nature_Lexeme nature) {
  switch (nature) {
  case NUM: return "NUM" ;
  case TYPE: return "TYPE";
  case PLUS: return "PLUS" ;
  case MOINS: return "MOINS" ;
  case MUL: return "MUL" ;
  case DIV: return "DIV";
  case IN:return "IN";
  case FUNCTION:return "FUNCTION";
  case EQUAL: return "EQUAL";
  case LET: return "LET";
  case AND: return "AND";
  case FIN_INSTR: return "FIN_INSTR" ;
  case PARO: return "PARO";
  case PARF: return "PARF";
  case VAR: return "VAR";
  case FIN_SEQUENCE: return "FIN_SEQUENCE"; 
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

