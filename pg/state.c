#include "state.h"

// NOTE: Both pgpiece_hash and pgpiece_compare functions must change if pgpiece does!

uint pgpiece_hash(const pgpiece *piece){
    // Gets a hash of a given piece.
    return *((uint *) piece);
}

int pgpiece_compare(const void *p_a, const void *p_b){
    // Compares two pieces using their numeric representation.
    uint hash_a = pgpiece_hash((const pgpiece*) p_a);
    uint hash_b = pgpiece_hash((const pgpiece*) p_b);
    return hash_a-hash_b;
}

uint pgstate_sort_and_hash(const pgstate *state){
    // Sorts the pieces of a state and then calculates it's hash.
    qsort((void*)state->pieces,state->n_pieces,sizeof(pgpiece),pgpiece_compare);
    uint hash = 0;
    for(int k=0;k<state->n_pieces;k++){
        hash ^= pgpiece_hash(&state->pieces[k]);
        hash = (hash<<13) & (hash>>19);
    }
    return hash;
}
