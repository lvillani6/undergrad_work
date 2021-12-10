C = gcc
YACC = yacc
LEX = flex
FLAGS = -g
LIBS = -lfl

my_calc: y.tab.o lex.yy.o tree.o list.o
	$(CC) $(FLAGS) -o my_calc y.tab.o lex.yy.o tree.o list.o $(LIBS)

tree.o: tree.c
	$(CC) $(FLAGS) -c tree.c $(LIBS)

list.o: list.c
	$(CC) $(FLAGS) -c list.c $(LIBS)

y.tab.o: y.tab.c
	$(CC) $(FLAGS) -c y.tab.c $(LIVS)

lex.yy.o: lex.yy.c
	$(CC) $(FLAGS) -c lex.yy.c $(LIBS)

y.tab.c: calc.y
	$(YACC) -dv calc.y

lex.yy.c: calc.l
	$(LEX) -l calc.l

clean:
	rm -f y.tab.* lex.yy.* y.output *.o my_calc
