#ifndef TABVAR_H_INCLUDED
#define TABVAR_H_INCLUDED

#include <stdbool.h>
#define MAXMEM 100

typedef union {
  float val_f;
  int val_i;
  bool val_b;
  char *string;
} Valeur;

typedef enum {
  ERR,
  INT,
  FLOAT,
  BOOL,
  STR,
  UNIT
} Type;

typedef struct {
  Valeur val;
  Type t;
  char nom[50];
} Variable;

typedef struct {
  Variable tab[MAXMEM];
  int taille;
} MemVar;

char* type(Variable v);

int ajouter_var (Variable *w,MemVar *m);

int chercher_var (char *var,MemVar *m,Variable *res);

void copie_environnement (MemVar *m ,MemVar *m2);

char *str_type(Type t);
void print_value(Variable v);


#endif
