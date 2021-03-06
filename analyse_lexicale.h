#ifndef AN_LEX_INC
#define AN_LEX_INC
/* ------------------------------------------------------------------------
-- module analyse_lexicale
--
-- analyse lexicale d'une expression arithmetique
--
-- P.Habraken - 18/01/2010
--
-- 2 aout 2006 - PH :
-- remplacement record a champs variants par record "plat"
-- Aout 2018 - LM : version en C  
------------------------------------------------------------------------ */

#include "tabvar.h"

typedef enum {
  //Arithmetic op
  PLUS,             // +
  MOINS,            // -
  MUL,              // *
  DIV,              // /
  
  //Logic op
  LOGIC_AND,//&
  LOGIC_OR,//or
  NOT,//!
  TRUE,
  FALSE,
  
  //Comparison ops
  EQUALITY,
  INEQUALITY,
  GREATER_THAN,
  LOWER_THAN,
  GREATER_OR_EQUAL,
  LOWER_OR_EQUAL,
  
  //Delimiters
  PARO,    //( ou begin
  PARF,    //) ou end
  COMMENTO,//(*
  COMMENTF,//*)
  
  //Various
  LET, //let
  AND, //and
  IN,// in
  EQUAL, //=
  IF,
  THEN,
  ELSE,
  
  //Data types
  VAR, //Variable
  FUNCTION, //fonction
  TYPE, //Data type
  NUM,// sequence de chiffres
  STRING,//chaine de caracteres
  
  //control lex
  ERREUR,
  FIN_EXPR,//;
  FIN_PRG, //;;
  FIN_SEQ
} Nature_Lexeme ;

typedef struct {
  Nature_Lexeme nature;    // nature du lexeme
  unsigned int ligne;           // numero de ligne
  unsigned int colonne;         // numero de colonne
  char chaine[256];
  Type type;// chaine de caracteres || nom de variable
  Valeur valeur;          // valeur d'une variable
} Lexeme; 

void afficher(Lexeme l);

void demarrer(char *nom_fichier);
   // e.i. : indifferent
   // e.f. : la sequence de lexemes est lue dans le fichier designe par
   //        nom_fichier
   //        fin_de_sequence <=> lexeme_courant.nature = FIN_SEQUENCE
   //        (non fin_de_sequence) => lexeme courant est le premier
   //        lexeme de la sequence
   //        l'exception Erreur_Lexicale est levee en cas d'erreur

void avancer();
   // pre-condition : la machine sequentielle est demarree
   // e.i. : on pose i = rang du lexeme_courant :
   //        (non fin_de_sequence)
   //        et (non lexeme_courant.nature = FIN_SEQUENCE)
   // e.f. : fin_de_sequence <=> lexeme_courant.nature = FIN_SEQUENCE
   //        (non fin_de_sequence) => lexeme_courant est le lexeme i+1
   //        l'exception Erreur_Lexicale est levee en cas d'erreur

Lexeme lexeme_courant();
   // pre-condition : la machine sequentielle est demarree
   // lexeme_courant est :
   // . le pseudo lexeme FIN_SEQUENCE si fin_de_sequence
   // . le pseudo lexeme ERREUR si une erreur a ete detectee
   // . le lexeme de rang i sinon

int fin_de_sequence();
   // pre-condition : la machine sequentielle est demarree
   // fin_de_sequence vaut :
   // . vrai si tous les lexemes de la sequence ont ete reconnus
   // . faux sinon

void arreter();
   // e.i. : la machine sequentielle est demarree
   // e.f. : la machine sequentielle est arretee

#endif
