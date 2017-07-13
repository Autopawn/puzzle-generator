threads = 0
debug = 2
flags = -pthread -D THREADS=$(threads) -D DEBUG=$(debug)

build: clean puzzlegen puzzlerules puzzleutils
	# Done!
puzzlegen:
	# Compile puzzlegen static library:
	mkdir bin
	gcc -Wall -std=c11 -c pg/state.c -o bin/state.o $(flags)
	gcc -Wall -std=c11 -c pg/exec.c -o bin/exec.o $(flags)
	gcc -Wall -std=c11 -c pg/analysis.c -o bin/analysis.o $(flags)
	ar rcs bin/libpuzzlegen.a bin/*.o
	cp pg/puzzlegen.h bin/puzzlegen.h
	rm bin/*.o
puzzlerules:
	# Compile puzzle rules:
	gcc -Wall -std=c11 -c rules/slide.c -o bin/slide.o $(flags)
	ar rcs bin/libpuzzlerules.a bin/*.o
	cp rules/rules.h bin/rules.h
	rm bin/*.o
puzzleutils:
	# Compile analysis utility:
	gcc -Wall -std=c11 -static utils/analysis_tool.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/analysis_tool.exe $(flags)
	gcc -Wall -std=c11 -static utils/execution_tool.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/execution_tool.exe $(flags)
clean:
	rm -rf bin || true
