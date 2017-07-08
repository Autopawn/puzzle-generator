build:
	rm -rf bin || true
	mkdir bin
	gcc -Wall -std=c11 -c pg/state.c -o bin/state.o
	gcc -Wall -std=c11 -c pg/exec.c -o bin/exec.o
	gcc -Wall -std=c11 -c rules/slide.c -o bin/exec.o
	gcc -Wall -std=c11 bin/*.o pg/main.c -o bin/puzzlegen
