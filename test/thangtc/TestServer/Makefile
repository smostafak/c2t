CC = cc
PROG = define.c main.c
main: define.o main.o
	${CC} define.o main.o -o main

define.o: define.c define.h
	${CC} -c define.c
	
main.o: main.c define.h
	${CC} main.c
	
clean: 
	rm -f *.o
