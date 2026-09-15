//
// Created by bernardo on 8/21/26.
//

#ifndef GENETIC_ALGORITHM_GENEFACTORY_H
#define GENETIC_ALGORITHM_GENEFACTORY_H

#include "GeneticTypes.h"
#include <random>

class GeneFactory {
    public:
    GeneFactory()= default;
    static Gene generate_random_gene(const GeneBounds &bounds, std::mt19937& gen);
    static double gene_to_double(const Gene& gene);
    static std::vector<double> chromosome_to_doubles(const std::vector<Gene>& genes);
};


#endif //GENETIC_ALGORITHM_GENEFACTORY_H
