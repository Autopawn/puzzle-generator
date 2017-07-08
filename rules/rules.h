#ifndef PG_RULES_H
#define PG_RULES_H

#include "../pg/exec.h"

#define OPEN 0
#define WALL 1
#define GOAL 2

static const int dir_x[] = {0,1,0,-1,0};
static const int dir_y[] = {0,0,-1,0,1};
static const char *dir_name[] = {"nowhere","right","up","left","down"};

pgresult slide_rule(const pglevel *level, const pgstate *state);

#endif
