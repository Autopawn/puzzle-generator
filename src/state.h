#ifndef PG_STATE_H
#define PG_STATE_H

#define MAX_PIECES 32

typedef unsigned char uchar;

typedef struct{
    uchar kind;
    uchar state;
    uchar p_x;
    uchar p_y;
} pgpiece;

typedef struct{
    pg_piece pieces[MAX_PIECES];
    int n_pieces;
} pgstate;

typedef struct{
    uchar max_x;
    uchar max_y;
    uchar cells[256][256];
} pglevel;

static inline uchar pglevel_at(const pglevel *level, uchar x, uchar y, uchar outside){
    if(x>level->max_x || y>level->max_y) return outside;
    else return level->cells[y][x];
}

#endif
