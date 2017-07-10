#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#include <string.h>

int main(int argc, char const *argv[]){
	if(!(argc==2 || (argc==3 && strcmp(argv[2],"-f")==0))){
		printf("Usage: %s <fname> [-f]\n",argv[0]);
		exit(1);
	}
	int stop_at_win = 1;
	if(argc==3 && strcmp(argv[2],"-f")==0) stop_at_win = 0;
	pglevel level;
	pgstate ini_state;
	// Read level:
	pgread_from_file(argv[1],&level,&ini_state);
	// Show it:
	pgshow_state(&level,&ini_state,0);
	// Compute execution tree:
	const pgexectree *tree = compute_pgexectree(
		&level,ini_state,slide_rule,-1,-1,stop_at_win);
	// Check number of new states for each deepness level:
	int n_states[MAX_DEEPNESS];
	int n_win_states[MAX_DEEPNESS];
	int n_different_win_states[MAX_DEEPNESS];
	int min_turns_win = -1;
	pgexectree_states_at_deepness(tree,0,n_states);
	pgexectree_states_at_deepness(tree,1,n_win_states);
	pgexectree_all_pieces_different_states(tree,1,1,n_different_win_states);
	printf("Computed different states:\n  %d\n",tree->n_states);
	printf("New states:\n");
	printf("  LEVEL   ALLDIFF WINNING  OTHERS\n");
	for(int i=0;i<tree->current_deepness;i++){
		if(min_turns_win==-1 && n_states[i]==0) min_turns_win=-2;
		printf("  L%5d: %7d %7d %7d\n",i,
			n_different_win_states[i],n_win_states[i],n_states[i]);
		if(n_win_states[i]>0 && min_turns_win==-1) min_turns_win = i;
		if(n_states[i]==0) break;
	}
	// Display minimum turns to win:
	printf("Minimum turns to win:\n");
	if(min_turns_win==-1) printf("  ?\n");
	else if(min_turns_win==-2) printf("  NO\n");
	else printf("  %d\n",min_turns_win);
	// Display number of different possible ways to win on minimum moves:
	printf("States that win on min choices:\n");
	if(min_turns_win==-1) printf("  ?\n");
	else if(min_turns_win==-2) printf("  0\n");
	else printf("  %d\n",n_win_states[min_turns_win]);
	// Display if is needed to move all the pieces to win with minimum moves:
	printf("Necessary to move all pieces to win on min choices:\n");
	if(min_turns_win==-1) printf("  ?\n");
	else if(min_turns_win==-2) printf("  X\n");
	else if(n_different_win_states[min_turns_win]==n_win_states[min_turns_win]){
		printf("  YES\n");
	}else printf("  NO\n");
	// Free tree:
	pgexectree_free(tree);
	return 0;
}
