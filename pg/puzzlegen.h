#ifndef PUZZLEGEN_H
#define PUZZLEGEN_H

#include <stdio.h>
#include <stdlib.h>

#if THREADS>0
	#include <pthread.h>
#endif

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

uint pgstate_hash(pgstate *state);
int pgstate_equals(pgstate *state_a, pgstate *state_b);
int pgstate_all_pieces_different(
		const pgstate *state_a, const pgstate *state_b, int ignore_stat);

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
// ^ Number of character that describe each choice.
#define MAX_DEEPNESS 128
// ^ Maximum level of deepness that the execution tree will have.

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
	int n_states;
	int win_reached;
	#if THREADS>0
		pthread_mutex_t hash_slots_mutexes[HASH_SLOTS];
		pthread_mutex_t tree_mutex; // Mutex for next queue and tree variables.
		pthread_mutex_t queue_mutex; // Mutex for current queue.
	#endif
} pgexectree;

typedef pgresult (*pgrule)(const pgstate *);
typedef pgresult (*pglevelrule)(const pglevel*, const pgstate *);

const pgexectree *compute_pgexectree(const pglevel *level, pgstate initial,
    	pglevelrule rule, int max_deepness, int max_states, int stop_at_win);

void pgexectree_free(const pgexectree *tree);

//##########################################################
//# EXECUTION ANALYSIS                                     #
//##########################################################

void pgexectree_states_at_deepness(
		const pgexectree *tree, int only_winning,
		int result[MAX_DEEPNESS]);
void pgexectree_all_pieces_different_states(
		const pgexectree *tree, int only_winning, int ignore_stat,
		int result[MAX_DEEPNESS]);

int get_prob_log2(prob probability);

void pgexectree_states_at_deepness(
		const pgexectree *tree, int only_winning,
		int result[MAX_DEEPNESS]);
void pgexectree_all_pieces_different_states(
		const pgexectree *tree, int only_winning, int ignore_stat,
		int result[MAX_DEEPNESS]);


#endif
