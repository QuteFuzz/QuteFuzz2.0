#ifndef GENOME_H
#define GENOME_H

#include <dag.h>

struct Genome {
    Dag::Dag dag;
    std::vector<Dag::Dag> subroutine_dags;
    int dag_score;
};

#endif
