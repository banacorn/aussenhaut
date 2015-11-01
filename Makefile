CFLAGS=-Wall -g

go:
	@ make clean
	@ mkdir -p build
	@ cc -o build/main src/main.c src/type.c src/string.c src/parser.c src/net.c src/env.c
	# @ ./build/main

check:
	@ make go
	@ valgrind --leak-check=full --track-origins=yes ./build/main

clean:
	@ rm -f ./build/main

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
