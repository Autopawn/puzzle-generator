#include "exec.h"

int main(int argc, char const *argv[]){
	if(argc!=2){
		printf("Usage: %s <fname>\n",argv[0]);
		exit(1);
	}
	pglevel level;
	pgstate ini_state;
	pgread_from_file(argv[1],0, &level, &ini_state);
	return 0;
}
