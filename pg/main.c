#include "exec.h"
#include "../rules/rules.h"

int main(int argc, char const *argv[]){
	if(argc!=2){
		printf("Usage: %s <fname>\n",argv[0]);
		exit(1);
	}
	pglevel level;
	pgstate ini_state;
	pgread_from_file(argv[1],&level,&ini_state);
	pgshow_state(&level,&ini_state);
	pgexectree *tree = compute_exectree(&level,ini_state,slide_rule,10);
	return 0;
}
