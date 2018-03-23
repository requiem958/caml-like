

/* Exemple d'utilisation de l'analyse syntaxique : 
    Affichage d'une sequence de lexemes            */

#include <stdio.h>
#include <stdlib.h>

#include "type_ast.h"
#include "ast_parcours.h"
#include "ast_construction.h"
#include "analyse_syntaxique.h"

int main (int argc, char *argv[]) {
  Ast a_res = NULL;
  if (argc >= 1) {
    analyser(argv[1],&a_res);
  }
  else {
    printf("Il faut donner un nom de fichier en argument !\n") ;
    return 1 ;
  }
   afficherarbre(a_res);
   printf("\nres = %lf\n", evaluation(a_res));
  libere(a_res);
  return 0 ;
}
