#include <stdio.h>
#include "analyse_syntaxique.h"
#include "analyse_lexicale.h"
#include "lecture_caracteres.h"
#include <string.h>

void analyser(char *nom_fichier, float* resultat){
  float acc = 0; //Where we place resulting real
  Nature_Lexeme flag = 0; //What operation we have to do
  typedef enum {E_I, E_S, E_N} Etat;
  Etat e = E_I;
  Lexeme l;
  demarrer(nom_fichier);
  l = lexeme_courant();

  if (l.nature == MOINS){
    e = E_S;
    flag = MOINS;
  }
  else if (l.nature == NUM){
    if (strcmp(l.chaine,".") == 0){
      afficher(lexeme_courant());
      printf(". solo est mort, comme Han solo");
      return;
    }
    e = E_N;
    acc = l.valeur;
  }
  else{
    afficher(lexeme_courant());
    printf("Abberation au démarrage\n");
  return;
  }

  avancer();
  l = lexeme_courant();
  
  while(l.nature != FIN_SEQUENCE){
    switch(e){
    case E_S:
      if (strcmp(l.chaine,".") == 0){
	afficher(lexeme_courant());
	printf(". solitaire non autorisé\n");
	return;
      }
      
      if(l.nature == NUM){
	switch(flag){
	case PLUS:
	  acc += l.valeur;
	  break;
	case MOINS:
	  acc -= l.valeur;
	  break;
	case MUL:
	  acc *= l.valeur;
	  break;
	case DIV:
	  if (l.valeur == 0){
	    afficher(lexeme_courant());
	    printf("Division par zero\n");
	    return;
	  }
	  acc /= l.valeur;
	  break;
	default:
	  printf("Rien ne va plus\n");
	  return;
	}
	e = E_N;
      }
      else{
	afficher(lexeme_courant());
	printf("Symbole non suivi d'un entier\n");
	return;
      }
      break;
      
    case E_N:
      if (l.nature < ERREUR){
	e = E_S;
	flag = l.nature;
      }
      else if (l.nature == ERREUR){
	afficher(lexeme_courant());
	printf("Lexeme abberant\n");
	return;
      }
      else if (l.nature == NUM){
	afficher(lexeme_courant());
	printf("Deux entiers cote à cote\n");
	return;
      }
      break;
    default:
      printf("Une abberation est arrivée, Fuyez pauvres fous\n");
      return;
    }
    avancer();
    l = lexeme_courant();
  }
  if ( e != E_N){
    afficher(lexeme_courant());
    printf("symbole en dernier\n");
    return;
  }
  printf("Tout va pour le mieux dans le meilleur des mondes\n");
  arreter();
  *resultat = acc;
}
