#include "puzzlegen.h"

void pgstate_step(pgrule rule, pgstate *state, pgconclusion *conclu){
    // Advances all the steps of a given state until a choice,
	// victory or loop is reached.
    // TODO: Check for loop, return something when that happens.
	pgresult result;
    while(1){
        result = rule(state);
        if(result.conclusion!=STEP) break;
        *state = result.next.step;
    }
	*conclu = result.conclusion;
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
		// Raise win_reached flag if it is a WIN.
		if(conclu==WIN) tree->win_reached = 1;
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
	if(node->conclusion==CHOICE){
		// Recover result of this state:
	    pgresult result = rule(&node->state);
	    // Add the derivated states to the hashtable and to the queue:
        for(int k=0;k<result.n_choices;k++){
			// Step forward the state of the choice, and insert it on the
			// execution tree.
			pgstate res_state = result.next.choices[k].resulting;
			pgconclusion res_conclusion;
			pgstate_step(rule,&res_state,&res_conclusion);
			pgexecnode *deriv =
				pgexectree_insert(tree,res_state,res_conclusion);
			node->nexts[node->n_nexts] = deriv;
			node->n_nexts++;
        }
    }
	return 0;
}

pgexectree *compute_pgexectree(const pglevel *level, pgstate initial,
    	pglevelrule rule, int max_deepness, int stop_at_win){
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
	tree->win_reached = 0;
    // Add initial state to the first queue:
    pgresult rule_with_level(const pgstate *state){
        return rule(level,state);
    }
	pgstate res_state = initial;
	pgconclusion res_conclusion;
    pgstate_step(rule_with_level,&res_state,&res_conclusion);
    tree->root = pgexectree_insert(tree,res_state,res_conclusion);
    // Iterate through the different levels of deepness
	int terminate = 0;
    while(!terminate){
        int over = pgexectree_proliferate_next(tree,rule_with_level);
		// Pass to the next deepness level:
		if(over){
			// Free the current queue:
			int current_queue = tree->current_deepness%2;
			tree->queue_len[current_queue] = 0;
			// Advance to the next queue:
			tree->current_queue_advance = 0;
			tree->current_deepness++;
			#if DEBUG>=1
			printf("Level %3d has %5d new states.\n",
				tree->current_deepness,tree->queue_len[!current_queue]);
			#endif
			#if DEBUG>=2
			for(int i=0;i<tree->queue_len[!current_queue];i++){
				pgstate *state = &tree->queues[!current_queue][i]->state;
				pgshow_state(level,state,1);
			}
			#endif
			int a = (tree->current_deepness>=max_deepness);
			int b = (tree->current_deepness>=MAX_DEEPNESS);
			int c = (stop_at_win && tree->win_reached);
			int d = (tree->queue_len[!current_queue]==0);
			terminate = (a || b || c || d);
		}
    }
    return tree;
}

void pgexectree_free(pgexectree *tree){
	for(int i=0;i<HASH_SLOTS;i++){
		pgexecnode *node = tree->hash_slots[i];
		while(node!=NULL){
			pgexecnode *old = node;
			node = node->link;
			free(old);
		}
	}
	free(tree);
}
