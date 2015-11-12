CFLAGS=-Wall -g

.PHONY: build cello test

build:
	@ make clean
	@ mkdir -p bin
	@ make cello
	cc src/main.c src/violin.c src/exec.c src/network.c src/parser.c -Lbuild -lCello -pthread -o bin/main
	@ make setup

go:
	@ cc src/main.c src/violin.c src/exec.c src/network.c src/parser.c -Lbuild -lCello -pthread -o bin/main
	@ cd ras && ./../bin/main

run:
	@ cd ras && ./../bin/main

check:
	@ make build
	@ ./bin/main
	@ valgrind --leak-check=full --track-origins=yes ./bin/main

cello:
	@ rm -rf Cello
	@ mkdir -p build
	@ git clone https://github.com/orangeduck/Cello.git
	@ cd Cello && make
	@ cp Cello/libCello.a build
	@ cp Cello/include/Cello.h src
	@ rm -rf Cello

clean:
	@ rm -f bin/main
	@ rm -f bin/client
	@ rm -rf bin

clean-all:
	@ rm -rf ras
	@ make clean

setup:
	@ make clean-all
	@ mkdir -p ras/
	@ mkdir -p ras/bin
	@ rm -rf ras/*.txt
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
