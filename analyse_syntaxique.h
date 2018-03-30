#ifndef AN_SYN_INC
#define AN_SYN_INC

#include "type_ast.h"
#include "analyse_lexicale.h"

typedef enum {
  ERR_PRG,
  ERR_FPRG,
  ERR_COMM,
  ERR_SEXPR,
  ERR_SSEXPR,
  ERR_EXPR,
  ERR_COND,
  ERR_COMP,
  ERR_VAL,
  ERR_AFF,
  ERR_AFFS,
  ERR_SAFFS,
  ERR_INSTR,
  ERR_INIT,
  ERR_SINIT,
  ERR_FIN,
  ERR_TYPE,
  ERR_OP1,
  ERR_OP2,
  ERR_STERME,
  ERR_SSTERME,
  ERR_TERME,
  ERR_SFACTEUR,
  ERR_SSFACTEUR,
  ERR_FACTEUR,
  ERR_LET,
  ERR_PARF,
  NOERR
}err_syntax;

/* Fonction d'analyse syntaxique*/
void analyser(char *nom_fichier, Ast* res);

#endif
