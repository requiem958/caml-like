#include "analyse_lexical.h"
#DEFINE MAXMEM 100


typedef struct {
  Valeur val;
  Type t;
  char *nom;
} Variable;

typedef struct {
  Variable tab[MAXMEM];
  int taille;
} MemVar;

void ajouter_var (Variable w);

void chercher_var (char *var);










