C = gcc
LEX = flex
FLAGS = -g
LIBS = -lfl

my_calc: lex.yy.o
	$(CC) $(FLAGS) -o my_calc lex.yy.o $(LIBS)

lex.yy.o: lex.yy.c
	$(CC) $(FLAGS) -c lex.yy.c $(LIBS)

lex.yy.c: calc.l
	$(LEX) -l calc.l

clean:
	rm -f lex.yy.* *.o my_calc
