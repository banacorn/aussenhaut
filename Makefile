CFLAGS=-Wall -g

go:
	@ make clean
	@ mkdir -p build
	@ cc -o build/main src/main.c  src/ds.c
	@ ./build/main

check:
	@ make go
	@ valgrind --leak-check=full --track-origins=yes ./build/main 

clean:
	@ rm -f main
