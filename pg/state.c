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
