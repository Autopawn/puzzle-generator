#ifndef PG_STATE_H
#define PG_STATE_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_PIECES 32
#define STATE_VARS 4

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct{
    uchar kind;
    uchar stat;
    uchar p_x;
    uchar p_y;
} pgpiece;

typedef struct{
    pgpiece pieces[MAX_PIECES];
    int n_pieces;
    int vars[STATE_VARS];
} pgstate;

typedef struct{
    uchar max_x;
    uchar max_y;
    uchar cells[256][256];
} pglevel;

static inline uchar pglevel_at(const pglevel *level, uchar x, uchar y, uchar outside){
    // Gets a cell on the given position or returns outside if it is outside the level.
    if(x>level->max_x || y>level->max_y) return outside;
    else return level->cells[y][x];
}

uint pgstate_hash(const pgstate *state);
int pgstate_equals(const pgstate *state_a, const pgstate *state_b);

#endif
