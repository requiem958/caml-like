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

typedef enum {
  PLUS,             // +
  MOINS,            // -
  MUL,              // *
  DIV,              // /
  ERREUR,
  PARO,    //(
  PARF,    //)
  VAR, //Variable
  LET, //let
  AND, //and
  IN,// in
  FUNCTION, //fonction
  EQUAL, //=
  TYPE, //Data type
  NUM,// sequence de chiffres
  FIN_INSTR, //;;
  FIN_SEQUENCE
} Nature_Lexeme ;

typedef union {
  float val_f;
  int val_i;
  int val_b;
  char *string;
} Valeur;

typedef enum {
  INT,
  FLOAT
} Type;

typedef struct {
  Nature_Lexeme nature;    // nature du lexeme
  unsigned int ligne;           // numero de ligne
  unsigned int colonne;         // numero de colonne
  char chaine[256];
  Type type;// chaine de caracteres || nom de variable
  Valeur valeur;          // valeur d'un réel
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
