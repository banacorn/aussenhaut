CFLAGS=-Wall -g

go:
	@ make clean
	@ cc src/main.c -o dist/main
	@ ./dist/main

clean:
	@ rm -f main
