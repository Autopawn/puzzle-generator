#include "exec.h"

typedef pgresult (*pgrule)(const pgstate *);

struct _pgexectree{
    pgexecnode *root;
    pgexecnode *hash_slots[HASH_SLOTS];
    pgexecnode *queues[2][QUEUE_SIZE];
    int current_deepness;
    int queue_len[2];
    int current_queue_advance;
};

pgresult pgstate_step(pgstate *state, pgrule rule){
    // Advances all the steps of a given state until a choice, victory or loop is reached.
    // TODO: Check for loop, return NULL when that happens.
    pgresult result;
    while(1){
        result = rule(state);
        if(result.conclusion!=STEP) break;
        state = &result.next.step;
    }
    return result;
}

pgexecnode *pgexectree_insert(pgexectree *tree, pgstate state,
		pgconclusion conclu){
    // ^ Adds a node with a given state to the execution tree and to queue if
	// it is new.
    // If the state already exists, just return its pointer.
    int exists = 0;
    uint hash = pgstate_hash(&state);
    int slot = hash%HASH_SLOTS;
    pgexecnode **linked_pointer = &tree->hash_slots[slot];
    while(*linked_pointer!=NULL){
        exists = pgstate_equals(&state,&(*linked_pointer)->state);
        if(exists) break;
        // Advance the linked pointer:
        linked_pointer = &((*linked_pointer)->link);
    }
    // If it doesn't exists, add it to the hashtable and its next queue.
    if(!exists){
        // Initialize an execution node:
        pgexecnode* node = malloc(sizeof(pgexecnode));
        if(!node){
            printf("Could not allocate memory for a execution node.\n");
            exit(1);
        }
        node->deepness = tree->current_deepness;
        node->state = state;
        node->link = NULL;
        node->conclusion = conclu;
		// Unknown yet:
        node->n_nexts = 0;
        // Add it to the linked list.
        *linked_pointer = node;
        // Add it to the next queue.
        int next_queue = (tree->current_deepness+1)%2;
        if(tree->queue_len[next_queue]>=QUEUE_SIZE){
            printf("Queue size not large enough.\n");
            exit(1);
        }
        tree->queues[next_queue][tree->queue_len[next_queue]] = node;
        tree->queue_len[next_queue]++;
    }
    //
    return *linked_pointer;
}

int pgexectree_proliferate_next(pgexectree *tree, pgrule rule){
    // ^Proliferates the next state on the current queue, returns 1 if the queue
	// is over.
    // "pop" the next state in the queue:
    int current_queue = tree->current_deepness%2;
	// Check if the queue is over, or else advance it.
	if(tree->current_queue_advance>=tree->queue_len[current_queue]){
		return 1;
	}
	pgexecnode *node =
		tree->queues[current_queue][tree->current_queue_advance];
	tree->current_queue_advance++;
	// Check the result of this state:
    pgresult result = rule(&node->state);
    // Add the derivated states to the hashtable and to the queue:
    if(result.conclusion==CHOICES){
        // Step forward the states of the choice:
        pgconclusion conclus[MAX_CHOICES];
        for(int k=0;k<result.n_choices;k++){
            pgresult res = pgstate_step(&result.next.choices[k].resulting,rule);
            conclus[k] = res.conclusion;
        }
        // Insert the states as nodes on the execution tree:
        for(int k=0;k<result.n_choices;k++){
            pgexecnode *deriv = pgexectree_insert(tree,
				result.next.choices[k].resulting,conclus[k]);
            node->nexts[node->n_nexts] = deriv;
            node->n_nexts++;
        }
    }
	return 0;
}

pgexectree *compute_exectree(const pglevel *level, pgstate initial,
    	pglevelrule rule, int max_deepness){
    // Initialize tree:
    pgexectree *tree = (pgexectree*) malloc(sizeof(pgexectree));
    if(!tree){
        printf("Could not allocate memory for the execution tree.\n");
        exit(1);
    }
    for(int k=0;k<HASH_SLOTS;k++){
        tree->hash_slots[k] = NULL;
    }
    tree->current_deepness = 0;
    tree->queue_len[0] = 0;
    tree->queue_len[1] = 0;
    tree->current_queue_advance = 0;
    // Add initial state to the first queue:
    pgresult rule_with_level(const pgstate *state){
        return rule(level,state);
    }
    pgresult result = pgstate_step(&initial,rule_with_level);
    tree->root = pgexectree_insert(tree,initial,result.conclusion);
    // Iterate through the different levels of deepness
    while(tree->current_deepness<=max_deepness){
        int over = pgexectree_proliferate_next(tree,rule_with_level);
		// Pass to the next deepness level:
		if(over){
			// Free the current queue:
			int current_queue = tree->current_deepness%2;
			tree->queue_len[current_queue] = 0;
			// Advance to the next queue:
			tree->current_queue_advance = 0;
			tree->current_deepness++;
		}
    }
    return tree;
}
