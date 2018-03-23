#ifndef SEQUENCES_H
#define SEQUENCES_H

#include "analyse_lexicale.h"

typedef struct variable_s {
  char * nomVar;
  Valeur var;
  Type t;
}var_t;

#define ENSMAX 1000000

struct ensemble_t {
    unsigned int longueur;
    var_t* mem[ENSMAX];
};
typedef struct ensemble_t ensemble;

void assignation(char* nomVar, Valeur v, Type t, ensemble E);

int recherche(char* nomVar, Ensemble E, var_t* var);

#endif
