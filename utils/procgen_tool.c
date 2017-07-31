#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PUZZLE_RULE slide_rule

int main(int argc, char const *argv[]){
	if(!(argc==5 || argc==6)){
		printf("Usage: %s <seed> <size_x> <size_y> [h][v][d] <fname>\n",argv[0]);
		exit(1);
	}
    int seed;
    int size_x;
    int size_y;
    sscanf(argv[1],"%d",&seed);
    if(seed==-1) seed = time(0);
    sscanf(argv[2],"%d",&size_x);
    sscanf(argv[3],"%d",&size_y);
    int min_cells = size_x*size_y*7/10;

    int mirr_h = 0;
    int mirr_v = 0;
    int mirr_d = 0;
    if(argc==6){
        const char* scanner = argv[4];
        while(*scanner!='\0'){
            if(*scanner=='h') mirr_h=1;
            if(*scanner=='v') mirr_v=1;
            if(*scanner=='d') mirr_d=1;
            scanner++;
        }
    }

    pglevel level = pglevel_generate(seed, size_x, size_y, min_cells,
        mirr_h,mirr_v,mirr_d,min_cells/4);

    pgstate state;
    state.n_pieces = 0;
    pgshow_state(&level,&state,0);
}
