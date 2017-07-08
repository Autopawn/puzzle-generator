build: clean puzzlegen puzzlerules puzzleutils
	# Done!
puzzlegen:
	# Compile puzzlegen static library:
	mkdir bin
	gcc -Wall -std=c11 -c pg/state.c -o bin/state.o
	gcc -Wall -std=c11 -c pg/exec.c -o bin/exec.o
	ar rcs bin/libpuzzlegen.a bin/*.o
	cp pg/puzzlegen.h bin/puzzlegen.h
	rm bin/*.o
puzzlerules:
	# Compile puzzle rules:
	gcc -Wall -std=c11 -c rules/slide.c -o bin/slide.o
	ar rcs bin/libpuzzlerules.a bin/*.o
	cp rules/rules.h bin/rules.h
puzzleutils:
	# Compile analysis utility
	gcc -Wall -std=c11 -static utils/analysis.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/analysis.exe
clean:
	rm -rf bin || true
