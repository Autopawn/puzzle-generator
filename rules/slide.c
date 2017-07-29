#include "rules.h"

// Returns the index of the piece on that position,
// UNTRAVERSABLE if is a solid and TRAVERSABLE if there isn't anything.
int occupying(const pglevel *level, const pgstate *state,
		int p_x, int p_y){
	if(pglevel_at(level,p_x,p_y,WALL)==WALL) return -1;
	for(int j=0;j<state->n_pieces;j++){
		pgpiece other = state->pieces[j];
		if(other.p_x==p_x && other.p_y==p_y) return j;
	}
	return -2;
}

pgresult slide_rule(const pglevel *level, const pgstate *state){
	pgresult result;

	// If there is a piece kind 0 on a goal, return WIN result.
	result.conclusion = WIN;
	for(int k=0;k<state->n_pieces;k++){
		pgpiece piece = state->pieces[k];
		if(piece.kind==0 && pglevel_at(level,piece.p_x,piece.p_y,WALL)==GOAL){
			return result;
		}
	}

	// If there is a piece that is moving, move it, return step result.
	result.conclusion = STEP;
	int moving = 0;
	result.next.step = *state;
	for(int k=0;k<state->n_pieces;k++){
		pgpiece piece = state->pieces[k];
		if(piece.stat>0){
			moving = 1;
			int next_x = piece.p_x + dir_x[piece.stat];
			int next_y = piece.p_y + dir_y[piece.stat];
			// Check if the next position is free:
            int ocup = occupying(level,state,next_x,next_y);
			if(ocup==TRAVERSABLE){
				// Move the piece.
				result.next.step.pieces[k].p_x = next_x;
				result.next.step.pieces[k].p_y = next_y;
			}else{
                // If the other piece is a kind 1 (box), start moving it:
                if(ocup>=0 && state->pieces[ocup].kind==1){
                    result.next.step.pieces[ocup].stat = piece.stat;
                }
				// Stop moving the piece.
				result.next.step.pieces[k].stat = 0;
			}
		}
	}
	if(moving) return result;

	// If no piece is moving, this is a choice:
	result.conclusion = CHOICE;
	result.n_choices = 0;
	for(int k=0;k<state->n_pieces;k++){
		for(int dir=1;dir<5;dir++){
			pgpiece piece = state->pieces[k];
            // Only kind 0 pieces can move.
            if(piece.kind!=0) continue;
			int next_x = piece.p_x + dir_x[dir];
			int next_y = piece.p_y + dir_y[dir];
			// Check if position is free:
			if(occupying(level,state,next_x,next_y)==TRAVERSABLE){
				// Add the choice:
				pgstate *resulting =
					&result.next.choices[result.n_choices].resulting;
				*resulting = *state;
				resulting->pieces[k].stat = dir;
				sprintf(result.next.choices[result.n_choices].description,
					"move (%d,%d) %s",piece.p_x,piece.p_y,dir_name[dir]);
				result.n_choices++;
			}
		}
	}
	return result;
}
