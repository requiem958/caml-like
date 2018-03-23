#include <stdio.h> 

#include "type_ast.h"
#include "ast_construction.h"
#include "ast_parcours.h"

int main(void) {

  Ast val12 ,val3,val5,val2,op1,op2,op3,op4,op5,op6,op7,op8;
 val12  = creer_valeur(12) ;
 val3  = creer_valeur(3) ;
 val5  = creer_valeur(5) ;
 val2  = creer_valeur(2) ;
 op1=  creer_operation(N_PLUS, val12, val3) ; // 12+3
 op2= creer_operation(N_MOINS, op1, val5) ; //(12+3)-5
 op3 = creer_operation(N_PLUS, val2, val3) ;// 2+3
 op4 = creer_operation(N_MOINS, val5, val2) ;//5-2
 op5 = creer_operation(N_MUL, op3, op4) ;//((2+3)*(5-2))
 op6 = creer_operation(N_DIV, op3, op4) ;//((2+3)/(5-2))
 op7 = creer_operation(N_MOINS, op4, val3) ;// (5-2)-3
 op8= creer_operation(N_DIV, val3, op7);// 3/((5-2)-3)


 
 printf("Arbre abstrait de l'expression\n") ;
 afficherarbre(op5) ;
 printf("Valeur de l'expression : %lf\n", evaluation(op5)) ;

  printf("Arbre abstrait de l'expression\n") ;
 afficherarbre(op6) ;
 printf("Valeur de l'expression : %lf\n", evaluation(op6)) ;


  printf("Arbre abstrait de l'expression\n") ;
 afficherarbre(op8) ;
 printf("Valeur de l'expression : %lf\n", evaluation(op8)) ;


printf("Arbre abstrait de l'expression\n") ;
 afficherarbre(op2) ;
 printf("Valeur de l'expression : %lf\n", evaluation(op2)) ;
 return 0 ;
 

}
