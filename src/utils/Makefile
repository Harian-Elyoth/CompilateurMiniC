EXE=test
CFLAGS = -Wall -std=c99

all: test

test: test.o context.o env.o
	gcc $(CFLAGS) test.o context.o env.o -o $@ 

test.o: test.c 
	gcc $(CFLAGS) -o $@ -c $<

context.o: context.c context.h
	gcc $(CFLAGS) -o $@ -c $<

env.o: env.c env.h
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o

realclean: clean
	rm -f *.o $(EXE)

