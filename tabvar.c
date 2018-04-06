#include <stdio.h>
#include "analyse_lexical.h"
#include "tabvar.h"

void ajouter_var (Variable *w,MemVar *m){
  Valeur res;
  int index;
  if (index=chercher_var(w->nom,m,res)!=-1)
    m->tab[index]=*w;
  else {
    if ( m->taille >999)
      printf ("erreur de segmentation\n");
    
  }
    
  
  
  
}

int chercher_var (char *var,MemVar m,Valeur res);

void copie_environement (MemVar m ,MemVar m2);
