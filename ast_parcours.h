
void afficherarbre(Ast expr) ;
// affiche l'arbre abstrait de l'expression arithmetique expr

Variable evaluation(Ast expr,MemVar *m) ;
// calcule la valeur de l'expression arithmetique expr

Variable apply_opAr(TypeOpAr op,Variable v1, Variable v2);

Variable apply_opLog(TypeOpLog op,Variable v1, Variable v2);

Variable apply_opComp(TypeOpComp op,Variable v1, Variable v2);
