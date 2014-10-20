LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/
CC = gcc
CFLAGS := -g -std=c99 -Wall -pedantic 
SQLFLAGS := $(shell mysql_config --cflags)
SQLLIBS := $(shell mysql_config --libs)

all: tests mains clean

mains: src/*.[ch]
	for main in $(wildcard src/*.c); do\
	    exec=`echo $$main | cut -d"." -f1`;\
	    $(CC) $(CFLAGS) $(SQLFLAGS) $$main $(wildcard libraries/*) -I$(INCLUDE_PATH) $(SQLLIBS) -ljansson -lm -o $$exec.out; \
	done;

tests: libraries tests/*.c
	for test in $(wildcard tests/*.c); do\
	    exec=`echo $$test | cut -d"." -f1`;\
	    $(CC) $(CFLAGS) $(SQLFLAGS) $$test $(wildcard libraries/*) -I$(INCLUDE_PATH) -o $$exec.out $(SQLLIBS);\
	done;

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
	    cd $$dir; \
	    $(CC) $(CFLAGS) $(SQLFLAGS) -I../ -c *.c  ; \
	    mv *.o ../../libraries; \
	    cd -; \
	done;

clean: 
	rm -rf libraries/
