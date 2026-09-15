//
// Created by bernardo on 8/21/26.
//


#ifndef GENETIC_ALGORITHM_CHROMOSOME_H
#define GENETIC_ALGORITHM_CHROMOSOME_H

#include <vector>
#include <random>
#include "GeneticTypes.h"

class Chromosome {
private:
    std::vector<Gene> genes{};
    double fitness = 0.0;
public:
    Chromosome();
    Chromosome(const std::vector<Gene>& g, double f);
    bool set_gene(int index, const Gene& gene);
    const std::vector<Gene>& get_genes () const;
    const Gene get_gene(int index) const;
    void set_fitness(double f);
    const double get_fitness() const;
    size_t size() const;
    static Chromosome generate_random_chromosome(const std::vector<GeneBounds> &bounds, std::mt19937 &gen);


};


#endif //GENETIC_ALGORITHM_CHROMOSOME_H
