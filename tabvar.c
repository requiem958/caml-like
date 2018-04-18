#include <stdio.h>
#include <string.h>
#include "tabvar.h"

int ajouter_var (Variable *w,MemVar *m){
  Valeur res;
  int index=chercher_var(w->nom,m,&res);
  if (index!=-1)
    m->tab[index]=*w;
  else {
    if ( m->taille >999){
      printf ("erreur de segmentation\n");
      return -1;
    }
   m->tab[m->taille]=*w;
   m->taille++;
  }
  return 0;
}

int chercher_var (char *var,MemVar *m,Valeur *res){
  int i;
  for(i=0;i<m->taille;i++){
    if (strcmp(m->tab[i].nom,var)==0){
      *res=m->tab[i].val;
      return i;
    }
  }
      return -1; 
}

void copie_environement (MemVar *m ,MemVar *m2){
  for (m2->taille=0;  m2->taille<m->taille;  m2->taille++)
    m2->tab[  m2->taille]=m->tab[m2->taille];
}

char* type(Variable v){
  v=v;
  return NULL;
}
  
  
  
  
  
