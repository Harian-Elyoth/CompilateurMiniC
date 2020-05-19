
EXE=minicc
UTILS=utils
LIBUTILS=$(UTILS)/libminiccutils.a

DEBUG=0

ifeq ($(DEBUG),1)
	DEBUG_FLAGS=-DYYDEBUG=1
else
	DEBUG_FLAGS=
endif

CFLAGS = -O0 -g -std=c99 -DYY_NO_LEAKS
INCLUDE = -I$(UTILS)

all: minicc

minicc: y.tab.o lex.yy.o common.o context.o env.o pass1.o
	gcc $(CFLAGS) $(INCLUDE) -L$(UTILS) $^ -o $@ -lminiccutils

y.tab.c: grammar.y Makefile
	yacc -d grammar.y --debug --verbose

lex.yy.c: lexico.l Makefile
	flex -d lexico.l  

lex.yy.o: lex.yy.c
	gcc $(DEBUG_FLAGS) $(CFLAGS) $(INCLUDE) -o $@ -c $<

y.tab.o: y.tab.c
	gcc $(DEBUG_FLAGS) $(CFLAGS) $(INCLUDE) -o $@ -c $<

common.o: common.c common.h defs.h Makefile
	gcc $(CFLAGS) $(INCLUDE) -o $@ -c $<

context.o: $(UTILS)/context.c $(UTILS)/context.h defs.h 
	gcc $(CFLAGS) $(INCLUDE) -o $@ -c $<

env.o: $(UTILS)/env.c $(UTILS)/env.h defs.h 
	gcc $(CFLAGS) $(INCLUDE) -o $@ -c $<

pass1.o : pass1.c pass1.h defs.h
	gcc $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -f *.o

realclean: clean
	rm -f y.tab.c y.tab.h lex.yy.c $(EXE)

