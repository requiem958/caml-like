#include <stdio.h>
#include "type_ast.h"
#include "ast_construction.h"

int main(void){
  Ast arbre;
  arbre = creer_operation(N_PLUS,NULL,NULL);
  libere(arbre);
  return 0;
}
