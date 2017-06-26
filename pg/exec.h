#ifndef PG_EXEC_H
#define PG_EXEC_H

#include <state.h>

typedef struct{
    int victory;
    pgstate nexts[MAX_CHOICES];
    int n_nexts;
} pgresult;

#endif
