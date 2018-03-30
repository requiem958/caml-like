#ifndef AN_SYN_INC
#define AN_SYN_INC

#include "type_ast.h"
#include "analyse_lexicale.h"

typedef enum {
  //Jesavaispasouvousmettre
  ERR_PRG,
  ERR_FPRG,
  ERR_COMM,
  //Expression
  ERR_SEXPR,
  ERR_SSEXPR,
  ERR_EXPR,
  //Affectation
  ERR_AFFE,
  ERR_SAFF,
  ERR_SSAFF,
  ERR_AFF,

  ERR_EQU,
  ERR_OBJ,
  //Operations
  ERR_OPE,
  ERR_STERME,
  ERR_SSTERME,
  ERR_TERME,
  ERR_SFACTEUR,
  ERR_SSFACTEUR,
  ERR_FACTEUR,
  ERR_VAL,
  ERR_NOMVAR,
  ERR_IDF,
  /*Condition */
  ERR_COND,
  ERR_THEN,
  ERR_SCOND,
  
  ERR_SBOOLEAU,
  ERR_SSBOOLEAU,
  ERR_BOOLEAU,
  ERR_LOR,
  
  ERR_S_BOOLAND,
  ERR_SSBOOLAND,
  ERR_BOOLAND,
  
  ERR_COMP,
  ERR_OPCOMP,
  
  ERR_OP1,
  ERR_OP2,
  ERR_LET,
  ERR_PARF,
  NOERR
}err_syntax;

/* Fonction d'analyse syntaxique*/
void analyser(char *nom_fichier, Ast* res);

#endif
