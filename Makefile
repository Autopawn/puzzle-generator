build:
	rm -rf bin || true
	mkdir bin
	gcc -Wall -c pg/state.c -o bin/state.o
