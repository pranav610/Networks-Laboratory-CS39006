libmsocket.a: mysocket.o
	ar -rcs libmsocket.a mysocket.o

mysocket.o: mysocket.c mysocket.h
	gcc -Wall -c mysocket.c

clean:
	rm mysocket.o 

distclean:
	rm libmsocket.a mysocket.o 

