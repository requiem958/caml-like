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

void ajouter_var (Variable w,MemVar m);

int chercher_var (char *var,MemVar m,Valeur res);

void copie_environement (MemVar m ,MemVar m2);










