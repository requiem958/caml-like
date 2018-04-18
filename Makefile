BUILD_REP=build
CC=gcc
DEBUG=-g
CFLAGS=-Wall -Wextra -Werror

all: test_lex test_syn

test_lex: $(BUILD_REP)/test_lexeme
test_syn: $(BUILD_REP)/test_syntaxe

#EXEC

$(BUILD_REP)/test_lexeme: analyse_lexicale.o  lecture_caracteres.o  test_lexeme.o
	$(CC) $(DEBUG) $(CFLAGS) -o $@ $^

$(BUILD_REP)/test_syntaxe: analyse_lexicale.o analyse_syntaxique.o lecture_caracteres.o test_syntaxe.o ast_parcours.o ast_construction.o tabvar.o
	$(CC) $(DEBUG) $(CFLAGS) -o $@ $^

essai_ast: ast_construction.o ast_parcours.o essai_ast.o
	$(CC) $(DEBUG) $(CFLAGS) -o $@ $^

#OBJ_LINK

lecture_caracteres.o: lecture_caracteres.c lecture_caracteres.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

ast_parcours.o:ast_parcours.c ast_parcours.h type_ast.h tabvar.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

ast_construction.o: ast_construction.c ast_construction.h type_ast.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

analyse_lexicale.o: analyse_lexicale.c analyse_lexicale.h lecture_caracteres.h grammaire.tkt
	$(CC) $(DEBUG) $(CFLAGS) -c $<

analyse_syntaxique.o: analyse_syntaxique.c lecture_caracteres.h analyse_lexicale.h analyse_syntaxique.h ast_construction.h type_ast.h grammaire.tkt
	$(CC) $(DEBUG) $(CFLAGS) -c $<

tabvar.o: tabvar.c tabvar.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

#OBJ_EXEC


test_lexeme.o: test_lexeme.c analyse_lexicale.h 
	$(CC) $(DEBUG) $(CFLAGS) -c $<

test_syntaxe.o: test_syntaxe.c analyse_syntaxique.h analyse_lexicale.h lecture_caracteres.h ast_parcours.h ast_construction.h type_ast.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

essai_ast.o: essai_ast.c ast_construction.h ast_parcours.h type_ast.h
	$(CC) $(DEBUG) $(CFLAGS) -c $<

clean:
	rm -f  $(BUILD_REP)/test_lexeme $(BUILD_REP)/test_syntaxe essai_ast *.o *.gch *~
