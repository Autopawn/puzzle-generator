#include "../bin/puzzlegen.h"
#include "../bin/rules.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define PUZZLE_RULE slide_rule

int main(int argc, char const *argv[]){
	if(argc!=2){
		printf("Usage: %s <fname>\n",argv[0]);
		exit(1);
	}
	// Load level:
	pglevel level;
	pgstate state;
	int n_moves = 0;
	pgread_from_file(argv[1],&level,&state);
    pgstate_hash(&state);
	//Draw level:
	while(1){
		printf("\033[2J\033[1;1H");
		pgshow_state(&level,&state,0);
		usleep(30000);
		pgresult result;
		while(1){
			result = PUZZLE_RULE(&level,&state);
			if(result.conclusion!=STEP) break;
			state = result.next.step;
            pgstate_hash(&state);
			printf("\033[2J\033[1;1H");
			pgshow_state(&level,&state,0);
			usleep(30000);
		}
		if(result.conclusion==CHOICE){
			if(result.n_choices<=0){
				printf("NO CHOICES!\n");
				break;
			}else{
				printf("CHOICE %d:\n",++n_moves);
				for(int i=0;i<result.n_choices;i++){
					printf("%6d) %s\n",i,result.next.choices[i].description);
				}
				while(1){
					int answer;
					printf("Choice: ");
					scanf("%d",&answer);
					if(0<=answer && answer<result.n_choices){
						state = result.next.choices[answer].resulting;
                        pgstate_hash(&state);
						break;
					}else{
						printf("Enter a valid number!\n");
					}
				}
			}
		}else if(result.conclusion==WIN){
			printf("WIN ON %d CHOICES!\n",n_moves);
			break;
		}else{
			printf("STRANGE CONCLUSION: %d\n",result.conclusion);
			break;
		}
	}
}
