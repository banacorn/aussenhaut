CFLAGS=-Wall -g

go:
	@ make clean
	@ cc -o dist/main src/main.c  src/list.c
	@ ./dist/main

check:
	@ make go
	@ valgrind ./dist/main

clean:
	@ rm -f main
