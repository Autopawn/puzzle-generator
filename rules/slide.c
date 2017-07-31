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

// Deletes pieces indicated on indexes,
void destroy_pieces(pgstate *state, int *will_die){
    int new_n_pieces = 0;
    for(int i=0;i<state->n_pieces;i++){
        if(will_die[i]) continue;
        if(new_n_pieces!=i) state->pieces[new_n_pieces] = state->pieces[i];
        new_n_pieces++;
    }
    state->n_pieces = new_n_pieces;
}

pgresult slide_rule(const pglevel *level, const pgstate *state){
    int will_die[MAX_PIECES] = {0};
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

    // Normal piece movement:
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
                // If was stopped by a piece:
                if(ocup>=0){
                    // If that piece is a kind 1 (box), start moving it:
                    if(state->pieces[ocup].kind==1){
                        result.next.step.pieces[ocup].stat = piece.stat;
                    }
                    // If that piece is kind 2 (bubble), pop it:
                    if(state->pieces[ocup].kind==2){
                        will_die[ocup] = 1;
                    }
                    // If that piece is kind 0 (person), and I am 3 (bug):
                    if(piece.kind==3 && state->pieces[ocup].kind==0){
                        will_die[ocup] = 1;
                    }
                    // If that piece is kind 3 (bug), and I am 0 (person):
                    if(piece.kind==0 && state->pieces[ocup].kind==3){
                        will_die[ocup] = 1;
                    }
                }
				// Stop moving the piece.
				result.next.step.pieces[k].stat = 0;
			}
		}
	}

    //Check if a piece moved:
	if(moving){
        // Eliminate pieces
        destroy_pieces(&result.next.step,will_die);
        // Return next state on a result
        return result;
    }

    // If no piece is moving, pass to the next phase, use the vars[0] to know
    // when to end.
    if(state->vars[0]==0){
        result.next.step.vars[0] = 1;
        // Time for the enemies to think:
        for(int k=0;k<state->n_pieces;k++){
            pgpiece piece = state->pieces[k];
            if(piece.kind == 3){
                int nearest_dist = 256;
                int nearest_dir = 0;
                for(int j=0;j<state->n_pieces;j++){
                    pgpiece other = state->pieces[j];
                    if(other.kind==0){
                        int delta_x = (int)other.p_x-(int)piece.p_x;
                        int delta_y = (int)other.p_y-(int)piece.p_y;
                        int dist = abs(delta_x)>abs(delta_y)?
                            abs(delta_x):abs(delta_y);
                        int dir = 0;
                        if(abs(delta_x)>abs(delta_y)){
                            dir = (delta_x > 0)? 1 : 3;
                        }else if(abs(delta_x)<abs(delta_y)){
                            dir = (delta_y > 0)? 4 : 2;
                        }
                        if(dist<nearest_dist){
                            nearest_dist = dist;
                            nearest_dir = dir;
                        }else if(dist==nearest_dist){
                            if(nearest_dir!=dir){
                                nearest_dir = 0;
                            }
                        }
                    }
                }
                if(nearest_dist<=1) nearest_dir = 0;
                result.next.step.pieces[k].stat = nearest_dir;
            }
        }
        return result;
    }

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
                // NOTE: After a choice the var[0] is resetted!
                resulting->vars[0] = 0;
                // Add state modification:
				resulting->pieces[k].stat = dir;
				sprintf(result.next.choices[result.n_choices].description,
					"move (%d,%d) %s",piece.p_x,piece.p_y,dir_name[dir]);
				result.n_choices++;
			}
		}
	}
	return result;
}
