C = gcc
YACC = yacc
LEX = flex
FLAGS = -g
LIBS = -lfl

dragon: y.tab.o lex.yy.o hash.o list.o tree.o semantic.o 
	$(CC) $(FLAGS) -o dragon y.tab.o lex.yy.o hash.o list.o tree.o semantic.o $(LIBS)

semantic.o: semantic.c
	$(CC) $(FLAGS) -c semantic.c $(LIBS)

hash.o: hash.c
	$(CC) $(FLAGS) -c hash.c $(LIBS)

list.o: list.c
	$(CC) $(FLAGS) -c list.c $(LIBS)

tree.o: tree.c
	$(CC) $(FLAGS) -c tree.c $(LIBS)

y.tab.o: y.tab.c
	$(CC) $(FLAGS) -c y.tab.c $(LIBS)

lex.yy.o: lex.yy.c
	$(CC) $(FLAGS) -c lex.yy.c $(LIBS)

y.tab.c: dragon.y
	$(YACC) -dv dragon.y

lex.yy.c: dragon.l
	$(LEX) -l dragon.l

clean:
	rm -f y.tab.* lex.yy.* y.output *.o dragon
