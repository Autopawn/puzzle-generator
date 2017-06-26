#include "state.h"

uint pgpiece_hash(const pgpiece *piece){
    // This function must be modified if the definition of the pgpiece changes!
    return *((uint *) piece);
}

int pgpiece_compare(const void *p_a, const void *p_b){
    uint hash_a = pgpiece_hash((const pgpiece*) p_a);
    uint hash_b = pgpiece_hash((const pgpiece*) p_b);
    return hash_a-hash_b;
}

uint pgstate_sort_and_hash(const pgstate *state){
    qsort((void*)state->pieces,state->n_pieces,sizeof(pgpiece),pgpiece_compare);
    uint hash = 0;
    for(int k=0;k<state->n_pieces;k++){
        hash ^= pgpiece_hash(&state->pieces[k]);
        hash = (hash<<13) & (hash>>19);
    }
    return hash;
}
