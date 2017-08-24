#include "puzzlegen.h"

void pgexectree_states_at_deepness(
		const pgexectree *tree, int only_winning,
		int result[MAX_DEEPNESS]){
	int count[MAX_DEEPNESS] = {0};
	// Iterate each hash slot:
	for(int i=0;i<HASH_SLOTS;i++){
		pgexecnode *node = tree->hash_slots[i];
		while(node!=NULL){
			// Count the node at the deepness level:
			if(!only_winning || node->conclusion==WIN){
				count[node->deepness]+=1;
			}
			// Advance pointer.
			node = node->link;
		}
	}
	for(int k=0;k<MAX_DEEPNESS;k++) result[k] = count[k];
}

void pgexectree_all_pieces_different_states(
		const pgexectree *tree, int only_winning, int ignore_stat,
		int result[MAX_DEEPNESS]){
	// ^ Returns the number of states that doens't have any piece in common with
	// the root state, at each deepness level.
	int count[MAX_DEEPNESS] = {0};
	pgstate init = tree->root->state;
	// Iterate each hash slot:
	for(int i=0;i<HASH_SLOTS;i++){
		pgexecnode *node = tree->hash_slots[i];
		while(node!=NULL){
			// Count the node at the deepness level:
			if(!only_winning || node->conclusion==WIN){
				if(pgstate_all_pieces_different(
						&init,&node->state,ignore_stat)){
					count[node->deepness]+=1;
				}
			}
			// Advance pointer.
			node = node->link;
		}
	}
	for(int k=0;k<MAX_DEEPNESS;k++) result[k] = count[k];
}

prob pgexectree_random_win_probability(const pgexectree *tree){
    // Know how much memory to allocate for pointers to the nodes on each level
    int n_nodes_at_lvl[MAX_DEEPNESS];
    pgexectree_states_at_deepness(tree,0,n_nodes_at_lvl);
    // Allocate that memory
    pgexecnode **nodes_at_lvl[MAX_DEEPNESS];
    for(int k=0;k<MAX_DEEPNESS;k++){
        nodes_at_lvl[k] =
            (pgexecnode**) malloc(n_nodes_at_lvl[k]*sizeof(pgexecnode*));
        n_nodes_at_lvl[k] = 0;
    }
    // Save pointers to the execnodes at each level
    for(int i=0;i<HASH_SLOTS;i++){
		pgexecnode *node = tree->hash_slots[i];
		while(node!=NULL){
            nodes_at_lvl[node->deepness][n_nodes_at_lvl[node->deepness]] = node;
            n_nodes_at_lvl[node->deepness]++;
			// Advance pointer.
			node = node->link;
		}
    }
    // Give the right probs to each node:
    for(int i=MAX_DEEPNESS-1;i>=0;i--){
        for(int k=0;k<n_nodes_at_lvl[i];k++){
            pgexecnode *node = nodes_at_lvl[i][k];
            if(node->conclusion==WIN){
                node->probability = ULLONG_MAX;
            }else{
                int at_least_one_with = 0;
                node->probability = 0;
                for(int j=0;j<node->n_nexts;j++){
                    node->probability +=
                        node->nexts[j]->probability/node->n_nexts;
                    if(node->nexts[j]->probability>0) at_least_one_with = 1;
                }
                if(node->probability==0 && at_least_one_with){
                    node->probability = 1;
                }
            }
        }
    }
    // Save the root prob:
    prob root_prob = tree->root->probability;
    // Free memory
    for(int k=0;k<MAX_DEEPNESS;k++){
        free(nodes_at_lvl[k]);
    }
    // Return the root probability of winning.
    return root_prob;
}

int get_prob_log2(prob probability){
    if(probability==ULLONG_MAX) return 0;
    int max_1 = 0;
    prob max_prob = ULLONG_MAX;
    while(max_prob>0){
        max_prob = max_prob/2;
        max_1++;
    }
    while(probability>0){
        probability = probability/2;
        max_1--;
    }
    return -max_1-1;
}
