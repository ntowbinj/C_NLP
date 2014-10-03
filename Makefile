LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/
CC = gcc
CFLAGS := -g -std=c99 -Wall -pedantic
SQLFLAGS := $(shell mysql_config --cflags)
SQLLIBS := $(shell mysql_config --libs)

all: tests main clean

main: libraries main.c config.h
	$(CC) $(CFLAGS) $(SQLFLAGS) main.c $(wildcard libraries/*) -I$(INCLUDE_PATH) $(SQLLIBS) -ljansson -lm -o main.out

tests: libraries tests/*.c
	for test in $(wildcard tests/*.c); do\
	    exec=`echo $$test | cut -d"." -f1`;\
	    $(CC) $(CFLAGS) $(SQLFLAGS) $$test $(wildcard libraries/*) -I$(INCLUDE_PATH) -o $$exec.out $(SQLLIBS);\
	done;

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
	    cd $$dir; \
	    $(CC) $(CFLAGS) $(SQLFLAGS) -I../ -c *.c $(SQLLIBS) ; \
	    mv *.o ../../libraries; \
	    cd -; \
	done;

clean: 
	rm -rf libraries/
