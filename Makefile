CFLAGS=-Wall -g

go:
	@ make clean
	@ mkdir -p bin
	@ cc src/main.c src/type.c src/string.c src/parser.c src/net.c src/env.c -Lbuild -lCello -o bin/main 
	@ ./bin/main

check:
	@ make go
	@ valgrind --leak-check=full --track-origins=yes ./bin/main

clean:
	@ rm -f ./bin/main

setup-bin:
	@ make clean-bin
	@ mkdir -p ras/
	@ mkdir -p ras/bin
	@ cp /bin/ls ras/bin
	@ cp /bin/cat ras/bin
	@ make compile-bin

clean-bin:
	@ rm -f /ras

compile-bin:
	@ cc src/bin/noop.cpp -o ras/bin/noop
	@ g++ src/bin/number.cpp -o ras/bin/number
	@ cc src/bin/removetag.cpp -o ras/bin/removetag
	@ cc src/bin/removetag0.cpp -o ras/bin/removetag0
