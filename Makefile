threads = 4
debug = 0
flags = -pthread -D THREADS=$(threads) -D DEBUG=$(debug)

game: clean puzzlerules
	gcc -Wall -std=c11 game/main.c \
		-Lbin -lpuzzlegen -lpuzzlerules -lSDL2 -lSDL2_image -o bin/game.o
rebuild: clean puzzlerules puzzleutils
	# Done!
puzzlegen:
	# Compile puzzlegen static library:
	mkdir bin
	gcc -Wall -std=c11 -c pg/state.c -o bin/state.o $(flags)
	gcc -Wall -std=c11 -c pg/exec.c -o bin/exec.o $(flags)
	gcc -Wall -std=c11 -c pg/analysis.c -o bin/analysis.o $(flags)
	gcc -Wall -std=c11 -c pg/proc.c -o bin/proc.o $(flags)
	ar rcs bin/libpuzzlegen.a bin/*.o
	cp pg/puzzlegen.h bin/puzzlegen.h
	rm bin/*.o
puzzlerules: puzzlegen
	# Compile puzzle rules:
	gcc -Wall -std=c11 -c rules/slide.c -o bin/slide.o $(flags)
	ar rcs bin/libpuzzlerules.a bin/*.o
	cp rules/rules.h bin/rules.h
	rm bin/*.o
puzzleutils: puzzlegen
	# Compile analysis utility:
	gcc -Wall -std=c11 -static utils/analysis_tool.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/analysis_tool.exe $(flags)
	gcc -Wall -std=c11 -static utils/execution_tool.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/execution_tool.exe $(flags)
	gcc -Wall -std=c11 -static utils/procgen_tool.c \
		-Lbin -lpuzzlegen -lpuzzlerules -o bin/procgen_tool.exe $(flags)
clean:
	rm -rf bin || true
