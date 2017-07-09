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
