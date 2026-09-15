//
// Created by bernardo on 8/22/26.
//

#ifndef GENETIC_ALGORITHM_GENETICALGORITHM_H
#define GENETIC_ALGORITHM_GENETICALGORITHM_H
#include <functional>
#include "Chromosome.h"
#include <random>


class GeneticAlgorithm {
    public:

    using CrossoverFunction = std::function<std::pair<Chromosome, Chromosome>( Chromosome&, Chromosome&, std::mt19937&)>;
    using ConstraintFunction = std::function<double(const std::vector<double>&)>;
    enum class OptimizationGoal { Minimize, Maximize };
    OptimizationGoal goal;

    GeneticAlgorithm(OptimizationGoal g,
        CrossoverFunction cf = uniform_crossover,
        std::vector<ConstraintFunction> constraints = {},
        double penalty_weight = 1000.0);

    std::pair<Chromosome, double> evolve(int max_generations,
        int population_size,
        double mutation_rate,
        int tournament_size, std::vector<GeneBounds>& bounds,
        std::function<double(const std::vector<double>&)> fitness_function);

    static std::pair<Chromosome, Chromosome> uniform_crossover(const Chromosome& parent1,
                const Chromosome& parent2,
                std::mt19937& gen);
private:
    CrossoverFunction crossover_fn;
    std::vector<ConstraintFunction> constraints;
    double penalty_weight;
    bool is_better(const Chromosome& a, const Chromosome& b);
    Chromosome tournament_selection(const std::vector<Chromosome>& population, int tournament_size, std::mt19937& gen);
    void mutate(Chromosome& chromosome, const std::vector<GeneBounds>& bounds, double mutation_rate, std::mt19937& gen);
    double adjust_fitness(const std::vector<double>& values, double fitness);

};


#endif //GENETIC_ALGORITHM_GENETICALGORITHM_H
