C = gcc
FLAGS = -g
LIBS = 

my_calc: calc.c tree.o
	$(CC) $(FLAGS) -o my_calc calc.c tree.o $(LIBS)

calc.o: calc.c
	$(CC) $(FLAGS) -c calc.c $(LIBS)

tree.o: tree.c
	$(CC) $(FLAGS) -c tree.c $(LIBS)

clean:
	rm -f *.o my_calc
