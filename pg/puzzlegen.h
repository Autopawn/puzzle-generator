#ifndef PUZZLEGEN_H
#define PUZZLEGEN_H

#include <stdio.h>
#include <stdlib.h>

//##########################################################
//# STATE DEFINITION                                       #
//##########################################################

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

static inline uchar pglevel_at(const pglevel *level,
		uchar x, uchar y, uchar outside){
    // Gets a cell on the given position or returns outside if
	// it is outside the level.
    if(x>level->max_x || y>level->max_y) return outside;
    else return level->cells[y][x];
}

uint pgstate_hash(const pgstate *state);
int pgstate_equals(const pgstate *state_a, const pgstate *state_b);

void pgread_from_file(const char *fname, pglevel *lvl, pgstate *ini);
void pgshow_state(const pglevel *lvl, const pgstate *ini, int mode);

//##########################################################
//# EXECUTION DEFINITION                                   #
//##########################################################

#define MAX_CHOICES 32
// ^ Maximum number of choices that can derivate from a single state.
#define HASH_SLOTS 1299721
// ^ Number of linked lists on the hash table (prime number).
#define QUEUE_SIZE 8388608
// ^ Size of the queue of states to proliferate.
#define CHOICE_DESCRIPTION_BUFFER 128

typedef struct{
	pgstate resulting;
	char description[CHOICE_DESCRIPTION_BUFFER];
} pgchoice;

typedef union{
    pgstate step;
    pgchoice choices[MAX_CHOICES];
} pgnext;

typedef enum{
    WIN, STEP, CHOICE
} pgconclusion;

typedef struct{
    pgconclusion conclusion;
    int n_choices;
    pgnext next;
} pgresult;

typedef struct _pgexecnode pgexecnode;

struct _pgexecnode{
    int deepness;
    pgstate state;
    pgconclusion conclusion;
    pgexecnode *link; // For use on a linked list on a hash slot.
    pgexecnode *nexts[MAX_CHOICES]; // For use on a tree graph.
    int n_nexts;
};

typedef struct{
    pgexecnode *root;
    pgexecnode *hash_slots[HASH_SLOTS];
    pgexecnode *queues[2][QUEUE_SIZE];
    int current_deepness;
    int queue_len[2];
    int current_queue_advance;
} pgexectree;

typedef pgresult (*pgrule)(const pgstate *);
typedef pgresult (*pglevelrule)(const pglevel*, const pgstate *);

pgexectree *compute_exectree(const pglevel *level, pgstate initial,
    	pglevelrule rule, int max_deepness);

void pgexectree_free(pgexectree *tree);

#endif
