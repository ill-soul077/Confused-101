CC    = gcc
BISON = bison
FLEX  = flex

all: confused.exe

confused.exe: confused.tab.c lex.yy.c
	$(CC) confused.tab.c lex.yy.c -o confused.exe -lm

confused.tab.c confused.tab.h: confused.y
	$(BISON) -d confused.y

lex.yy.c: confused.l confused.tab.h
	$(FLEX) confused.l

clean:
	del confused.tab.c confused.tab.h lex.yy.c confused.exe