CFLAGS=-Wall -g

.PHONY: build

build:
	@ make clean
	@ mkdir -p bin
	@ cc src/main.c src/violin.c src/exec.c src/network.c src/parser.c -Lbuild -lCello -o bin/main

go:
	@ make build
	@ cd ras && ./../bin/main

check:
	@ make build
	@ ./bin/main
	@ valgrind --leak-check=full --track-origins=yes ./bin/main

clean:
	@ rm -f bin/main
	@ rm -f bin/client

clean-all:
	@ rm -rf ras
	@ make clean

setup:
	@ make clean-all
	@ mkdir -p ras/
	@ mkdir -p ras/bin
	@ cp /bin/ls ras/bin
	@ cp /bin/cat ras/bin
	@ make compile-ras
	@ cp test-src/test.html ras/

compile-ras:
	@ cc src/bin/noop.cpp -o ras/bin/noop
	@ g++ src/bin/number.cpp -o ras/bin/number
	@ cc src/bin/removetag.cpp -o ras/bin/removetag
	@ cc src/bin/removetag0.cpp -o ras/bin/removetag0

test:
	@ cc test-src/client.c -o bin/client
	@ ./bin/client 127.0.0.1 4444 test-src/test/test1.txt
	@ ./bin/client 127.0.0.1 4444 test-src/test/test2.txt
	@ ./bin/client 127.0.0.1 4444 test-src/test/test3.txt
	@ ./bin/client 127.0.0.1 4444 test-src/test/test4.txt
	@ ./bin/client 127.0.0.1 4444 test-src/test/test5.txt
	@ ./bin/client 127.0.0.1 4444 test-src/test/test6.txt
