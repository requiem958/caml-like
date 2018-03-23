all: test_syntaxe test_lexeme

#EXEC

test_lexeme: analyse_lexicale.o  lecture_caracteres.o  test_lexeme.o
	gcc -g -Wall -o test_lexeme analyse_lexicale.o  lecture_caracteres.o  test_lexeme.o

test_syntaxe: analyse_lexicale.o analyse_syntaxique.o lecture_caracteres.o test_syntaxe.o ast_parcours.o ast_construction.o
	gcc -g -Wall -o $@ $^

essai_ast: ast_construction.o ast_parcours.o essai_ast.o
	gcc -g -Wall -o $@ $^

#OBJ_LINK

analyse_lexicale.o: analyse_lexicale.c analyse_lexicale.h lecture_caracteres.h
	gcc -g -Wall -c analyse_lexicale.c

lecture_caracteres.o: lecture_caracteres.h lecture_caracteres.c
	gcc -g -Wall -c lecture_caracteres.c

analyse_syntaxique.o: lecture_caracteres.h analyse_lexicale.h analyse_syntaxique.h analyse_syntaxique.c
	gcc -g -Wall -c analyse_syntaxique.c

ast_parcours.o:ast_parcours.c ast_parcours.h type_ast.h
	gcc -g -Wall -c ast_parcours.c

ast_construction.o: ast_construction.c ast_construction.h type_ast.h
	gcc -g -Wall -c ast_construction.c


#OBJ_EXEC


test_lexeme.o: analyse_lexicale.h test_lexeme.c
	gcc -g -Wall -c test_lexeme.c

test_syntaxe.o: analyse_syntaxique.h analyse_lexicale.h lecture_caracteres.h type_ast.h test_syntaxe.c
	gcc -g -Wall -c test_syntaxe.c

essai_ast.o: ast_construction.h ast_parcours.h type_ast.h essai_ast.c
	gcc -g -Wall -c essai_ast.c

clean:
	rm -f  test_lexeme test_syntaxe essai_ast *.o *~
