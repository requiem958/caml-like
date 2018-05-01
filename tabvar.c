#include <stdio.h>
#include <string.h>
#include "tabvar.h"

char *str_type(Type t){
  switch(t){
  case INT:
    return "int";
  case FLOAT:
    return "float";
  case BOOL:
    return "boolean";
  case STR:
    return "string";
  case UNIT:
    return "unit";
  default:
    return "no types";
  }
}

void print_value(Variable v){
  switch(v.t){
  case INT:
    printf("%d",v.val.val_i);
    break;
  case FLOAT:
    printf("%f",v.val.val_f);
    break;
  case BOOL:
    printf("%s",v.val.val_b ? "true":"false");
    break;
  case STR:
    printf("%s",v.val.string);
    break;
  default:
    printf("#ERRINVALUE");
  }
}

int ajouter_var (Variable *w,MemVar *m){
  Variable res;
  int index=chercher_var(w->nom,m,&res);
  if (index!=-1)
    m->tab[index]=*w;
  else {
    if ( m->taille >999){
      printf ("erreur de segmentation\n");
      return -1;
    }
    if ( m->taille < 0)
      m->taille = 0;
   m->tab[m->taille]=*w;
   m->taille++;
  }
  return 0;
}

int chercher_var (char *var,MemVar *m,Variable *res){
  int i;
  for(i=0;i<m->taille;i++){
    if (strcmp(m->tab[i].nom,var)==0){
      *res=m->tab[i];
      return i;
    }
  }
      return -1; 
}

void copie_environnement (MemVar *m ,MemVar *m2){
  for (m2->taille=0;  m2->taille<m->taille;  m2->taille++)
    m2->tab[  m2->taille]=m->tab[m2->taille];
}
  
  
  
  
  
