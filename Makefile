CFLAGS=-Wall -g

go:
	@ make clean
	@ mkdir -p build
	@ cc -o build/main src/main.c  src/list.c
	@ ./build/main

check:
	@ make go
	@ valgrind ./build/main

clean:
	@ rm -f main
