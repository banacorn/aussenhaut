CFLAGS=-Wall -g

go:
	@ make clean
	@ mkdir -p build
	@ cc -o build/main src/main.c  src/ds.c
	@ ./build/main

check:
	@ make go
	@ valgrind ./build/main --track-origins=yes --leak-check=full

clean:
	@ rm -f main
