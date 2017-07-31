#include "puzzlegen.h"

#define TRANSF(X,Y) ((Y)*256+(X))

typedef struct {
    pglevel level;
    int n_sets;
    int rank[65536];
    int parent[65536];
    int n_lits;
} pgshell;


void init_pgshell(pgshell *shell, int s_x, int s_y){
    for(int y=0;y<256;y++){
        for(int x=0;x<256;x++){
            shell->level.cells[y][x] = 1;
        }
    }
    shell->level.max_x = (uchar) s_x-1;
    shell->level.max_y = (uchar) s_y-1;
    shell->n_sets = 0;
    shell->n_lits = 0;
}

int pgshell_find(pgshell *shell, int current){
    if(shell->parent[current]!=current){
        shell->parent[current] = pgshell_find(shell,shell->parent[current]);
    }
    return shell->parent[current];
}

void pgshell_union(pgshell *shell, int a, int b){
    a = pgshell_find(shell,a);
    b = pgshell_find(shell,b);
    if(a!=b){
        shell->n_sets -= 1;
        if(shell->rank[a]>shell->rank[b]){
            shell->parent[b] = a;
        }else{
            shell->parent[a] = b;
            if(shell->rank[a]>shell->rank[b]){
                shell->rank[b] += 1;
            }
        }
    }
}

int pgshell_is_lit(pgshell* shell, int p_x, int p_y){
    if(p_x<0) return 0;
    if(p_y<0) return 0;
    if(p_x>shell->level.max_x) return 0;
    if(p_y>shell->level.max_y) return 0;
    return shell->level.cells[p_y][p_x]!=1;
}

void pgshell_lit(pgshell *shell, int p_x, int p_y, uchar val){
    if(val==1) return;
    if(p_x<0) return;
    if(p_y<0) return;
    if(p_x>shell->level.max_x) return;
    if(p_y>shell->level.max_y) return;
    if(shell->level.cells[p_y][p_x]==1){
        shell->n_lits += 1;
        shell->n_sets += 1;
        int current = TRANSF(p_x,p_y);
        shell->parent[current] = current;
        shell->rank[current] = 0;
        if(pgshell_is_lit(shell,p_x+1,p_y)){
            pgshell_union(shell,TRANSF(p_x+1,p_y),current);
        }
        if(pgshell_is_lit(shell,p_x-1,p_y)){
            pgshell_union(shell,TRANSF(p_x-1,p_y),current);
        }
        if(pgshell_is_lit(shell,p_x,p_y+1)){
            pgshell_union(shell,TRANSF(p_x,p_y+1),current);
        }
        if(pgshell_is_lit(shell,p_x,p_y-1)){
            pgshell_union(shell,TRANSF(p_x,p_y-1),current);
        }
    }
    shell->level.cells[p_y][p_x] = val;
}

pglevel pglevel_generate(int seed, int s_x, int s_y, int min_cells,
        int mirr_x, int mirr_y, int mirr_diag, int pepper){
    pgshell shell;
    init_pgshell(&shell,s_x,s_y);
    srand(seed);
    while(shell.n_lits<min_cells || shell.n_sets!=1){
        int p_x = rand()%(shell.level.max_x+1);
        int p_y = rand()%(shell.level.max_y+1);
        pgshell_lit(&shell,p_x,p_y,0);
        if(shell.n_lits>=pepper){
            if(mirr_x==1){
                int r_x = shell.level.max_x-p_x;
                pgshell_lit(&shell,r_x,p_y,0);
            }
            if(mirr_y==1){
                int r_y = shell.level.max_y-p_y;
                pgshell_lit(&shell,p_x,r_y,0);
            }
            if(mirr_diag==1){
                int r_x = shell.level.max_x-p_x;
                int r_y = shell.level.max_y-p_y;
                pgshell_lit(&shell,r_x,r_y,0);
            }
        }
    }
    return shell.level;
}
