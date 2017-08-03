#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PUZZLE_RULE slide_rule

int main(int argc, char const *argv[]){
	if(!(argc==5 || argc==6)){
		printf("Usage: %s <seed> <size_x> <size_y> [h][v][d][r] <fname>\n",argv[0]);
		exit(1);
	}

    int seed;
    sscanf(argv[1],"%d",&seed);
    if(seed==-1) srand(time(0));
    else srand(seed);

    int size_x;
    int size_y;
    sscanf(argv[2],"%d",&size_x);
    sscanf(argv[3],"%d",&size_y);

    int mirror = 0;
    int no_mirror_final = 0;
    if(argc==6){
        const char* scanner = argv[4];
        while(*scanner!='\0'){
            if(*scanner=='h') mirror+=1;
            if(*scanner=='v') mirror+=2;
            if(*scanner=='d') mirror+=4;
            if(*scanner=='r') no_mirror_final = 1;
            scanner++;
        }
    }

    pgstate state;
    state.n_pieces = 0;

    pgshell shell;
    init_pgshell(&shell,size_x,size_y);

    int lits = 0;
    lits += pgshell_pepper(&shell,size_x*size_y/4,mirror);
    pgshow_state(&shell.level,&state,0);
    lits += pgshell_pepper(&shell,-1,mirror);
    pgshow_state(&shell.level,&state,0);
    if(no_mirror_final) mirror=0;
    lits += pgshell_pepper(&shell,(size_x*size_y-lits)/3,mirror);
    pgshow_state(&shell.level,&state,0);
}
