build:
	rm -rf bin || true
	mkdir bin
	gcc -Wall -std=c11 -c pg/state.c -o bin/state.o
	gcc -Wall -std=c11 -c pg/exec.c -D DEBUG=1 -o bin/exec.o
	gcc -Wall -std=c11 -c rules/slide.c -o bin/slide.o
	gcc -Wall -std=c11 bin/state.o bin/exec.o bin/slide.o pg/main.c \
		-o bin/puzzlegen
memcheck:
	gcc -Wall -g -std=c11 -c pg/state.c -o bin/state.o
	gcc -Wall -g -std=c11 -c pg/exec.c -D DEBUG=1 -o bin/exec.o
	gcc -Wall -g -std=c11 -c rules/slide.c -o bin/slide.o
	gcc -Wall -g -std=c11 bin/state.o bin/exec.o bin/slide.o pg/main.c \
		-o bin/puzzlegen
	valgrind --tool=memcheck --track-origins=yes --leak-check=full \
		./bin/puzzlegen lvls/slide00.txt
