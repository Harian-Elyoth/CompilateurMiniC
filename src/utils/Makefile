EXE=test
CFLAGS = -Wall -Werror -std=c99

all: test

test: test_listechainee.o context.o
	gcc $(CFLAGS) test_listechainee.o context.o -o $@ 

test_listechainee.o: test_listechainee.c 
	gcc $(CFLAGS) -o $@ -c $<

context.o: context.c context.h
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o

realclean: clean
	rm -f *.o $(EXE)

