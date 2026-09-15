//
// Created by bernardo on 8/21/26.
//

#ifndef GENETIC_ALGORITHM_POPULATION_H
#define GENETIC_ALGORITHM_POPULATION_H

#include <functional>

#include "GeneticTypes.h"
#include "Chromosome.h"
#include <vector>
#include "GeneticAlgorithm.h"
class Population {
    public:
    [[nodiscard]] std::vector<Chromosome>& population1();
    void set_population(const std::vector<Chromosome> &population);

    Population() = default;
        Population(int population_size, const std::vector<GeneBounds> &bounds, std::mt19937& gen);

        void evaluate_population(std::function<double(const std::vector<double>&)> fitness_function);

        std::pair<Chromosome, double> get_best(GeneticAlgorithm::OptimizationGoal goal);

    private:
        double calculate_fitness(const std::vector<double>& values,
        double fitness,
        double penalty_weight);

        std::vector<Chromosome> population;
        void generate_population(int population_size, const std::vector<GeneBounds> &bounds, std::mt19937& gen);

};


#endif //GENETIC_ALGORITHM_POPULATION_H
