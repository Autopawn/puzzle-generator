#include "state.h"

/* NOTE: pgpiece_value can be used for comparison of pieces because its bijective,
if pgpiece changes it may not be longer useful for that purpose.
*/

uint pgpiece_value(const pgpiece *piece){
    // Gets a hash of a given piece.
    return *((uint *) piece);
}

int pgpiece_compare(const void *p_a, const void *p_b){
    // Compares two pieces using their numeric representation.
    uint hash_a = pgpiece_value((const pgpiece*) p_a);
    uint hash_b = pgpiece_value((const pgpiece*) p_b);
    return hash_a-hash_b;
}

void pgstate_sort(const pgstate *state){
    qsort((void*)state->pieces,state->n_pieces,sizeof(pgpiece),pgpiece_compare);
}

uint pgstate_hash(const pgstate *state){
    pgstate_sort(state);
    uint hash = 0;
    for(int k=0;k<STATE_VARS;k++){
        hash ^= state->vars[k];
        hash = (hash<<15) & (hash>>17);
    }
    for(int k=0;k<state->n_pieces;k++){
        hash ^= pgpiece_value(&state->pieces[k]);
        hash = (hash<<13) & (hash>>19);
    }
    return hash;
}

int pgstate_equals(const pgstate *state_a, const pgstate *state_b){
    if(state_a->n_pieces!=state_b->n_pieces) return 0;
    for(int k=0;k<STATE_VARS;k++){
        if(state_a->vars[k]!=state_b->vars[k]) return 0;
    }
    pgstate_sort(state_a);
    pgstate_sort(state_b);
    for(int k=0;k<state_a->n_pieces;k++){
		uint piece_a_val = pgpiece_value(&state_a->pieces[k]);
		uint piece_b_val = pgpiece_value(&state_b->pieces[k]);
        if(piece_a_val!=piece_b_val) return 0;
    }
    return 1;
}

void pgread_from_file(const char *fname, pglevel *lvl, pgstate *ini){
	// Check that the output pointers aren't NULL.
	if(lvl==NULL || ini==NULL){
		printf("Error: NULL pointer passed to pgread_from_file.\n");
		exit(1);
	}
	// Try to open the file.
	FILE *file = fopen(fname,"r");
	if(file==NULL){
		printf("Error opening \"%s\".\n",fname);
		exit(1);
	}
	// Reset the output data structures.
	lvl->max_x = 0;
	lvl->max_y = 0;
	ini->n_pieces = 0;
	// Read the file according to the mode:
	int current_x=0;
	int current_y=0;
	char ch;
	while((ch=fgetc(file))!=EOF){
		uchar value = 0;
		if(ch=='\n'){
			// Pass to the next line:
			current_x = 0;
			current_y++;
			for(int i=0;i<256;i++) lvl->cells[current_y][i] = 0;
			continue;
		}else if('1'<=ch && ch<='9'){
			value = (uchar)(ch-48);
		}else if(ch=='0'){
			value = 10;
		}else if(ch==' '){
			value = 0;
		}else if('A'<=ch && ch<='~'){
			// Create a new piece:
			value = (uchar)(ch-65);
			if(ini->n_pieces==MAX_PIECES){
				printf("Error: MAX_PIECES reached on pgread_from_file.\n");
				exit(1);
			}
			ini->pieces[ini->n_pieces].p_x = current_x;
			ini->pieces[ini->n_pieces].p_y = current_y;
			ini->pieces[ini->n_pieces].stat = 0;
			ini->pieces[ini->n_pieces].kind = value;
			ini->n_pieces++;
			value = 0;
		}
		lvl->cells[current_y][current_x] = value;
		if(current_x>lvl->max_x) lvl->max_x = current_x;
		current_x++;
	}
	lvl->max_y = current_y-(current_x==0);
	fclose(file);
	printf("Level \"%s\" loaded.\n",fname);
}

void pgshow_state(const pglevel *lvl, const pgstate *state, int mode){
	for(int y=-1;y<=lvl->max_y+1;y++){
		for(int x=-1;x<=lvl->max_x+1;x++){
			if(y==-1 || y==lvl->max_y+1 || x==-1 || x==lvl->max_x+1){
				printf("\e[47m#");
			}else{
				// Set background color according to cell.
				uchar cell = lvl->cells[y][x];
				if(0<=cell && cell<=7) printf("\e[4%dm",cell);
				else if(cell==8) printf("\e[100m");
				else if(cell==9) printf("\e[107m");
				else printf("\e[40m");
				// Check if there is a piece to print.
				int piece = -1;
				for(int k=0;k<state->n_pieces;k++){
					if(state->pieces[k].p_x==x && state->pieces[k].p_y==y){
						if(piece!=-1){
							// More than one piece here.
							piece=-2;
							break;
						}
						piece = state->pieces[k].kind;
					}
				}
				if(piece==-2){
					// Draw more than one piece symbol.
					printf("\e[1;37m+");
				}else if(piece>=0){
					// Draw the piece.
					printf("\e[1;37m%c",'A'+piece);
				}else{
					// Draw the cell.
					if(cell==0) printf(" ");
					else if(1<=cell && cell<=7) printf("%c",'0'+cell);
					else if(cell==8) printf("8");
					else if(cell==9) printf("9");
					else if(cell==10) printf("0");
					else printf("?");
				}
			}
			// Reset colors.
			printf("\e[0m");
		}
		printf("\n");
	}
	if(mode==1){
		for(int k=0;k<state->n_pieces;k++){
			pgpiece pie = state->pieces[k];
			printf("p: %d (%d,%d) %d\n",pie.kind,pie.p_x,pie.p_y,pie.stat);
		}
	}
}
