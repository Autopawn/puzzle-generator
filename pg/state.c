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

void pgread_from_file(const char *fname, int mode, pglevel *lvl, pgstate *ini){
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
	if(mode==0){
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
				continue;
			}
			lvl->cells[current_y][current_x] = value;
			if(current_x>lvl->max_x) lvl->max_x = current_x;
			current_x++;
		}
		lvl->max_y = current_y-(current_x==0);
	}else{
		printf("Error: pgread_from_file mode %d invalid.\n",mode);
		exit(1);
	}
	fclose(file);
	printf("Level \"%s\" loaded.\n",fname);
}
