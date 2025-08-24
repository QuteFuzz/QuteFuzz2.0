#ifndef GENOME_H
#define GENOME_H

#include <dag.h>

struct Genome {
    Dag::Dag dag;
    int dag_score;
};

#endif
