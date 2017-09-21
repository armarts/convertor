all: conv delete

conv: main convert
	gcc -o conv main.o convert.o -lpng

main: main.c
	gcc -c main.c

convert: convert.c
	gcc -c convert.c

delete:
	rm -rf *.o
