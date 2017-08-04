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

// Deletes pieces indicated on indexes, returns the number of deleted pieces.
int destroy_pieces(pgstate *state, int *will_die){
    int new_n_pieces = 0;
    for(int i=0;i<state->n_pieces;i++){
        if(will_die[i]) continue;
        if(new_n_pieces!=i) state->pieces[new_n_pieces] = state->pieces[i];
        new_n_pieces++;
    }
    int delta = state->n_pieces-new_n_pieces;
    state->n_pieces = new_n_pieces;
    return delta;
}

int cannot_move_time(int kind){
    if(kind==3 || kind==4) return 1;
    else return -1;
}

pgresult slide_rule(const pglevel *level, const pgstate *state){
    int will_die[MAX_PIECES] = {0};
	pgresult result;

	// If there is a piece kind 0 on a goal, return WIN result.
	result.conclusion = WIN;
	for(int k=0;k<state->n_pieces;k++){
		pgpiece piece = state->pieces[k];
		if((piece.kind==0 || piece.kind==5)
                && pglevel_at(level,piece.p_x,piece.p_y,WALL)==GOAL){
			return result;
		}
	}

    int moving = 0;
	result.conclusion = STEP;
    pgstate next = *state;

    // Get pieces sorted by position:
    int indexes_sorted_x[MAX_PIECES];
    int indexes_sorted_y[MAX_PIECES];
    for(int k=0;k<state->n_pieces;k++){
        indexes_sorted_x[k] = k;
        indexes_sorted_y[k] = k;
    }
    int compare_x(const void *a, const void *b){
        int i_a = *(int*)a;
        int i_b = *(int*)b;
        return state->pieces[i_a].p_x - state->pieces[i_b].p_x;
    }
    int compare_y(const void *a, const void *b){
        int i_a = *(int*)a;
        int i_b = *(int*)b;
        return state->pieces[i_a].p_y - state->pieces[i_b].p_y;
    }
    qsort(indexes_sorted_x,state->n_pieces,sizeof(int),compare_x);
    qsort(indexes_sorted_y,state->n_pieces,sizeof(int),compare_y);

    // Move pieces one direction at the time:
    for(int d=1;d<5;d++){
        // Get the right order:
        int mv_idx = (d==1 || d==4)? state->n_pieces-1 : 0;
        // Move the pieces:
        while(0<=mv_idx && mv_idx<state->n_pieces){
            // Get next piece on sorted list
            int idx = (d==1 || d==3)?
                indexes_sorted_x[mv_idx] : indexes_sorted_y[mv_idx];
            pgpiece piece = state->pieces[idx];
            // Normal piece movement:
            if(piece.stat==d && cannot_move_time(piece.kind)!=state->vars[0]){
                moving = 1;
                int next_x = piece.p_x + dir_x[piece.stat];
                int next_y = piece.p_y + dir_y[piece.stat];
                // Check if the next position is free:
                int ocup = occupying(level,&next,next_x,next_y);
                // Don't count pieces that are moving on the opposite direction.
                if(next.pieces[ocup].stat!=0 && next.pieces[ocup].stat!=d &&
                        cannot_move_time(next.pieces[ocup].kind)!=
                        state->vars[0]){
                    ocup = TRAVERSABLE;
                }
                if(ocup==TRAVERSABLE || (ocup!=UNTRAVERSABLE && piece.kind==4)){
                    // Move the piece.
                    next.pieces[idx].p_x = next_x;
                    next.pieces[idx].p_y = next_y;
                }
                if(ocup!=TRAVERSABLE || piece.kind==4){
                    // If was stopped by a piece:
                    if(ocup>=0){
                        // If that piece is a kind 1 (box), start moving it:
                        if(state->pieces[ocup].kind==1){
                            next.pieces[ocup].stat = piece.stat;
                        }
                        // If that piece is kind 2 (bubble), pop it:
                        if(state->pieces[ocup].kind==2){
                            will_die[ocup] = 1;
                        }
                        // If that piece is kind 0 (person), and I am 3 (sun):
                        if(piece.kind==3 && (state->pieces[ocup].kind==0 ||
                                state->pieces[ocup].kind==5)){
                            will_die[ocup] = 1;
                        }
                        // If that piece is kind 3 (sun), and I am 0 (person):
                        if((piece.kind==0 || piece.kind==5) &&
                                state->pieces[ocup].kind==3){
                            will_die[ocup] = 1;
                        }
                        // If that piece is kind 4 (fire), I die:
                        if(state->pieces[ocup].kind==4){
                            will_die[idx] = 1;
                        }
                        // If I am kind 4 (fire), the other dies:
                        if(piece.kind==4 && state->pieces[ocup].kind!=4){
                            will_die[ocup] = 1;
                        }
                    }
                    // Stop moving the piece.
                    next.pieces[idx].stat = 0;
                }
            }
            // Move on the sorted list:
            mv_idx += (d==1 || d==4)? -1 : 1;
        }
    }

    if(state->vars[0]==1){
        // Enemy thinking:
    	for(int k=0;k<state->n_pieces;k++){
    		pgpiece piece = state->pieces[k];
            if(piece.kind==3 || piece.kind==4){
                int move_dir = -1;
                for(int j=0;j<state->n_pieces;j++){
                    pgpiece other = state->pieces[j];
                    if(other.kind==0 || other.kind==5){
                        int dir_force = 0;
                        if(other.p_x==piece.p_x){
                            dir_force = other.p_y>piece.p_y? 4 : 2;
                        }
                        if(other.p_y==piece.p_y){
                            dir_force = other.p_x>piece.p_x? 1 : 3;
                        }
                        if(dir_force!=0){
                            if(move_dir==-1 || dir_force==move_dir){
                                // Move
                                move_dir = dir_force;
                            }else{
                                // Choose not to move, because of confusion
                                move_dir = 0;
                                break;
                            }
                        }
                    }
                }
                if(move_dir!=-1) next.pieces[k].stat = move_dir;
            }
    	}
    }

	if(moving || next.vars[0]==1){
        // If not moving, go to next phase:
        if(!moving) next.vars[0] = 1-next.vars[0];
        // Return next state on a result
        destroy_pieces(&next,will_die);
        result.next.step = next;
        return result;
    }

	// If no piece is moving, this is a choice:

    // Reset piece movement
    next.vars[0] = 1;
    for(int k=0;k<next.n_pieces;k++){
        next.pieces[k].stat = 0;
    }

    // Create choices
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
				*resulting = next;
                // Add state modification:
				resulting->pieces[k].stat = dir;
                // Move the clones too:
                for(int i=0;i<resulting->n_pieces;i++){
                    if(resulting->pieces[i].kind==5){
                        resulting->pieces[i].stat = dir;
                    }
                }
				sprintf(result.next.choices[result.n_choices].description,
					"move (%d,%d) %s",piece.p_x,piece.p_y,dir_name[dir]);
				result.n_choices++;
			}
		}
	}
	return result;
}
