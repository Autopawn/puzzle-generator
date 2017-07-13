#include "rules.h"

int is_position_free(const pglevel *level, const pgstate *state,
		int p_x, int p_y){
	if(pglevel_at(level,p_x,p_y,WALL)==WALL) return 0;
	for(int j=0;j<state->n_pieces;j++){
		pgpiece other = state->pieces[j];
		if(other.p_x==p_x && other.p_y==p_y) return 0;
	}
	return 1;
}

pgresult slide_rule(const pglevel *level, const pgstate *state){
	pgresult result;

	// If there is a piece on a goal, return WIN result.
	result.conclusion = WIN;
	for(int k=0;k<state->n_pieces;k++){
		pgpiece piece = state->pieces[k];
		if(pglevel_at(level,piece.p_x,piece.p_y,WALL)==GOAL){
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
			if(is_position_free(level,state,next_x,next_y)){
				// Move the piece.
				result.next.step.pieces[k].p_x = next_x;
				result.next.step.pieces[k].p_y = next_y;
			}else{
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
			int next_x = piece.p_x + dir_x[dir];
			int next_y = piece.p_y + dir_y[dir];
			// Check if position is free:
			if(is_position_free(level,state,next_x,next_y)){
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
